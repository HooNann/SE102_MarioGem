#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "QuestionBlock.h"
#include "Game.h"
#include "MarioIdleState.h"
#include "MarioWalkState.h"
#include "MarioRunState.h"
#include "MarioJumpState.h"
#include "MarioFallState.h"
#include "MarioDuckState.h"
#include "MarioDeadState.h"
#include "PlayScene.h"
#include "Camera.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "Burner.h"
#include "Blaster.h"
#include "CannonBall.h"

#include "Collision.h"
#include "FireBall.h"
#include	"PlayScene.h"
#include "SoundEvents.h"
#include "SoundSubject.h"
#include "MarioFallState.h"

namespace
{
	constexpr int ToInt(GoombaState state)
	{
		return static_cast<int>(state);
	}
}

CMario::CMario(float x, float y) : CGameObject(x, y)
{
	isSitting = false;
	maxVx = 0.0f;
	ax = 0.0f;
	ay = MARIO_GRAVITY;

	untouchable = 0;
	untouchable_start = -1;
	isOnPlatform = false;
	isThrowingFire = false;
	throwingFireStartTime = 0;
	currentState = new CMarioIdleState();

	isTransforming = false;
	transformStartTime = 0;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Khi đang biến hình: freeze vật lý, chỉ chạy timer
	if (isTransforming)
	{
		// Dừng chuyển động hoàn toàn
		vx = 0.0f;
		vy = 0.0f;
		ax = 0.0f;

		// Kiểm tra kết thúc biến hình (timer dùng wall-clock, không cần dt)
		if (GetTickCount64() - transformStartTime >= MARIO_TRANSFORM_DURATION)
		{
			isTransforming = false;
		}
		return;
	}

	HandlePMeter(dt);
	UpdateThrowingFireTime(dt);
	vy += ay * dt;
	vx += ax * dt;
	
	if (currentState) {
		currentState->Update(this, dt);
	}

	if (vx > 0 && maxVx > 0 && vx > maxVx) vx = maxVx;
	if (vx < 0 && maxVx < 0 && vx < maxVx) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);

	if (CGame::GetInstance()->GetCurrentScene() != NULL && dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene()))
	{
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		CCamera* camera = CCamera::GetInstance();
		float cx, cy;
		camera->GetCamPos(cx, cy);
		float screenWidth = camera->GetWidth();

		float ml, mt, mr, mb;
		GetBoundingBox(ml, mt, mr, mb);
		float mario_width = mr - ml;

		float left_edge = cx + mario_width / 2.0f;
		if (scene->IsCameraBlockingLeftEdge() && x < left_edge)
		{
			x = left_edge;
			// Ép vận tốc về mức đi bộ để xả P-Meter và giữ animation
			if (vx < -MARIO_WALKING_SPEED) vx = -MARIO_WALKING_SPEED;
		}

		float right_edge = cx + screenWidth - mario_width / 2.0f;
		if (!scene->IsCourseClear() && scene->IsCameraBlockingRightEdge() && x > right_edge)
		{
			x = right_edge;
			if (vx > MARIO_WALKING_SPEED) vx = MARIO_WALKING_SPEED;
		}
	}
}



void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;

    if (vy > 0 && currentState)
    {
        MarioStateID curID = currentState->GetID();
        if (curID == MarioStateID::Idle || curID == MarioStateID::Walk || curID == MarioStateID::Run || curID == MarioStateID::Skid)
        {
            ChangeState(new CMarioFallState());
        }
    }
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else 
	if (e->nx != 0 && e->obj->IsBlocking())
	{
		if (e->nx < 0 && vx > MARIO_WALKING_SPEED) 
			vx = MARIO_WALKING_SPEED;
		else if (e->nx > 0 && vx < -MARIO_WALKING_SPEED) 
			vx = -MARIO_WALKING_SPEED;
	}

	if (currentState) {
		currentState->OnCollisionWith(this, e);
	}
}


void CMario::ShootFireBall()
{
	if (this->GetLevel() != MarioLevel::Fire) return;
	if (isThrowingFire) return;

	this->isThrowingFire = true;
	this->throwingFireStartTime = GetTickCount64();

	float fireX = this->x + (this->nx * 8.0f);
	float fireY = this->y - 2.0f;

	CFireBall* fireball = new CFireBall(fireX, fireY, this->nx);

	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	currentScene->AddObject(fireball);
	CSoundSubject::GetInstance()->Notify(EVENT_FIREBALL);
}

void CMario::UpdateThrowingFireTime(DWORD dt)
{
	if (isThrowingFire)
	{
		if (GetTickCount64() - throwingFireStartTime >= MARIO_THROWING_FIRE_TIME)
		{
			isThrowingFire = false;
			throwingFireStartTime = 0;
		}
	}
}

void CMario::HandlePMeter(DWORD dt)
{
	if (currentState && currentState->GetID() == MarioStateID::Run)
	{
		if (isOnPlatform && abs(vx) >= MARIO_RUNNING_SPEED - 0.02f)
		{
			pMeter += dt;
			if (pMeter > MARIO_PMETER_MAX) pMeter = MARIO_PMETER_MAX;
            
            // Nếu đang có cờ bay mà lại đạt max tốc độ trên mặt đất thì reset timer
            if (pMeter == MARIO_PMETER_MAX && isFlyingPowerActive)
            {
                flyStartTime = GetTickCount64();
            }
		}
        else if (pMeter > 0 && !isFlyingPowerActive)
        {
            // Đang Run nhưng chưa đủ tốc độ, tụt pin nếu chưa bay
            pMeter -= dt * 2;
            if (pMeter < 0) pMeter = 0;
        }
	}
	else 
	{
		// Xả pin nếu không chạy và không trong trạng thái fly timer
		if (pMeter > 0 && !isFlyingPowerActive)
		{
			pMeter -= dt * 2;
			if (pMeter < 0) pMeter = 0;
		}
	}

    if (isFlyingPowerActive) 
    {
        if (GetTickCount64() - flyStartTime > MARIO_FLYING_TIME_MAX) 
        {
            isFlyingPowerActive = false;
            pMeter = 0; // Hết giờ bay thì tụt sạch P-Meter để ép phải chạy lại
        }
    } 
    else 
    {
        if (pMeter == MARIO_PMETER_MAX) 
        {
            isFlyingPowerActive = true;
            flyStartTime = GetTickCount64();
        }
    }
}









void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (currentState) {
		aniId = currentState->GetAnimationId(this);
	}

	// Hiệu ứng chớp chớp khi đang biến hình: ẩn mỗi BLINK_INTERVAL ms
	if (isTransforming)
	{
		ULONGLONG elapsed = GetTickCount64() - transformStartTime;
		ULONGLONG blinkPhase = (elapsed / MARIO_TRANSFORM_BLINK_INTERVAL) % 2;
		if (blinkPhase == 1) return; // Frame ẩn
	}

	float timeScale = 1.0f;
	if (isOnPlatform && abs(vx) > 0)
	{
		timeScale = abs(vx) / MARIO_WALKING_SPEED;
		// Giới hạn để chân không quạt quá chậm hoặc quá nhanh
		if (timeScale < 0.5f) timeScale = 0.5f;
		if (timeScale > 3.0f) timeScale = 3.0f;
	}

	if (aniId != -1) {
		animations->Get(aniId)->Render(x, y, nx, 1, timeScale);
	}

	//RenderBoundingBox();
	
	//DebugOutTitle(L"Coins: %d", GetCoins());
}



void CMario::ChangeState(CMarioState* newState)
{
    if (currentState && currentState->GetID() == MarioStateID::Dead) {
        delete newState;
        return;
    }
    
    if (currentState && currentState->GetID() == newState->GetID()) {
        delete newState;
        return;
    }
    
    if (currentState) {
        currentState->Exit(this);
        delete currentState;
    }
    currentState = newState;
    currentState->Enter(this);
}

void CMario::TakeDamage()
{
	if (untouchable != 0) return;
	if (currentState && currentState->GetID() == MarioStateID::Dead) return;

	if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon)
	{
		SetLevel(MarioLevel::Big);
		StartUntouchable();
		CSoundSubject::GetInstance()->Notify(EVENT_POWERDOWN);
	}
	else if (GetLevel() == MarioLevel::Big)
	{
		SetLevel(MarioLevel::Small);
		StartUntouchable();
		CSoundSubject::GetInstance()->Notify(EVENT_POWERDOWN);
	}
	else
	{
		ChangeState(new CMarioDeadState());
	}
}

int CMario::IsCollidable()
{
    return (currentState && currentState->GetID() != MarioStateID::Dead);
}

int CMario::IsBlocking()
{
    return (currentState && currentState->GetID() != MarioStateID::Dead && untouchable == 0);
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (GetLevel() != MarioLevel::Small)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else 
		{
			left = x - MARIO_BIG_BBOX_WIDTH/2;
			top = y - MARIO_BIG_BBOX_HEIGHT/2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH/2;
		top = y - MARIO_SMALL_BBOX_HEIGHT/2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(MarioLevel l)
{
	// Adjust position to avoid falling off platform when growing
	if (this->GetLevel() == MarioLevel::Small && l != MarioLevel::Small)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	CGameData::GetInstance()->SetLevel(l);

	// Bắt đầu hiệu ứng biến hình: freeze scene + chớp chớp
	StartTransforming();
}

LPGAMEOBJECT CMario::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	return new CMario(x, y);
}



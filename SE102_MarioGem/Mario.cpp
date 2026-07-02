#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "QuestionBlock.h"
#include "Game.h"
#include "CMarioIdleState.h"
#include "CMarioWalkState.h"
#include "CMarioRunState.h"
#include "CMarioJumpState.h"
#include "CMarioFallState.h"
#include "CMarioDuckState.h"
#include "CMarioDeadState.h"
#include "CMarioIdleState.h"
#include "CMarioWalkState.h"
#include "CMarioRunState.h"
#include "CMarioJumpState.h"
#include "CMarioFallState.h"
#include "CMarioDuckState.h"
#include "CMarioDeadState.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "Burner.h"
#include "Blaster.h"
#include "CannonBall.h"
#include "CItem.h"

#include "Collision.h"
#include "FireBall.h"
#include	"PlayScene.h"

namespace
{
	constexpr int ToInt(GoombaState state)
	{
		return static_cast<int>(state);
	}
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	HandlePMeter(dt);
	vy += ay * dt;
	vx += ax * dt;
	
	if (currentState) {
		currentState->Update(this, dt);
	}

	if (abs(vx) > abs(maxVx)) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
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
		vx = 0;
	}

	if (currentState) {
		currentState->OnCollisionWith(this, e);
	}
}


void CMario::ShootFireBall()
{
	if (this->GetLevel() != MarioLevel::Fire) return;

	this->isThrowingFire = true;
	this->throwingFireStartTime = GetTickCount64();

	float fireX = this->x + (this->nx * 8.0f);
	float fireY = this->y - 2.0f;

	CFireBall* fireball = new CFireBall(fireX, fireY, this->nx);

	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	currentScene->AddObject(fireball);
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
	if (currentState && (currentState->GetID() == MarioStateID::Run))
	{
		if (abs(vx) >= MARIO_RUNNING_SPEED - 0.02f)
		{
			pMeter += dt;
			if (pMeter > MARIO_PMETER_MAX) pMeter = MARIO_PMETER_MAX; // Khóa trần pin
		}
	}
	else 
	{
		if (isOnPlatform) // Chỉ xả pin nhanh khi đã đáp đất an toàn
		{
			pMeter -= dt * 2; // Tốc độ xả pin nhanh gấp đôi sạc
			if (pMeter < 0) pMeter = 0;
		}
	}
}

void CMario::FlyUp()
{
	// Chỉ cho phép bay nếu đang là Mario Chồn VÀ thanh năng lượng đã nạp đầy 100%
	if (this->GetLevel() != MarioLevel::Raccoon || pMeter < MARIO_PMETER_MAX) return;

	if (currentState && (currentState->GetID() == MarioStateID::Run));
	{
		flyStartTime = GetTickCount64();
	}

	// Trong vòng giới hạn 4 giây, mỗi lần nhấp phím sẽ đẩy Mario lên tiếp
	if (GetTickCount64() - flyStartTime < MARIO_FLYING_TIME_MAX)
	{
		vy = -MARIO_JUMP_SPEED_Y * 0.75f; // Đẩy một lực Y âm để cất cánh hướng lên trên
		isOnPlatform = false;             // Rời đất
	}
}


void CMario::FloatDown()
{
	if (this->GetLevel() == MarioLevel::Raccoon && vy > 0 && isOnPlatform == false)
	{
		// SetState(static_cast<int>(MarioState::Float)); // Chuyển sang hành động vỗ đuôi
		vy = 0.03f;                  // Gán một vận tốc rơi cực kỳ nhỏ (hãm phanh trọng lực)
	}
}





void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (currentState) {
		aniId = currentState->GetAnimationId(this);
	}

	if (aniId != -1) {
		animations->Get(aniId)->Render(x, y);
	}

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", GetCoins());
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
}

LPGAMEOBJECT CMario::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	return new CMario(x, y);
}



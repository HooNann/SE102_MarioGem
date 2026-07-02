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

CMario::CMario(float x, float y) : CGameObject(x, y)
{
	isSitting = false;
	maxVx = 0.0f;
	ax = 0.0f;
	ay = MARIO_GRAVITY;

	untouchable = 0;
	untouchable_start = -1;
	isOnPlatform = false;
	currentState = new CMarioIdleState();
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

#include "CMarioFallState.h"

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
	if (currentState && currentState->GetID() == MarioStateID::Run)
	{
		if (isOnPlatform && abs(vx) >= MARIO_RUNNING_SPEED - 0.02f)
		{
			pMeter += dt;
			if (pMeter > MARIO_PMETER_MAX) pMeter = MARIO_PMETER_MAX;
		}
	}
	else 
	{
		// Xả pin nếu không chạy
		if (pMeter > 0)
		{
			pMeter -= dt * 2;
			if (pMeter < 0) pMeter = 0;
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



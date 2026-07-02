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

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<CBurner*>(e->obj))
		OnCollisionWithBurner(e);
	else if (dynamic_cast<CBlaster*>(e->obj))
		OnCollisionWithBlaster(e);
	else if (dynamic_cast<CCannonBall*>(e->obj))
		OnCollisionWithCannonBall(e);
	else if (dynamic_cast<CItem*>(e->obj))
		OnCollisionWithItem(e);
	else if (dynamic_cast<CQuestionBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	CGameData::GetInstance()->AddCoin(1);
	CGameData::GetInstance()->AddScore(100);
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	// Mario nhảy húc đầu vào từ dưới lên
	if (e->ny > 0)
	{
		vy = 0; // Chặn Mario lại, rơi xuống ngay lập tức
		CQuestionBlock* block = dynamic_cast<CQuestionBlock*>(e->obj);
		if (block->GetState() != QUESTION_BLOCK_STATE_EMPTY)
		{
			block->SetState(QUESTION_BLOCK_STATE_HIT);
		}
	}
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != ToInt(GoombaState::Die))
		{
			goomba->SetState(GoombaState::Die);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != ToInt(GoombaState::Die))
			{
				if (GetLevel() > MarioLevel::Small)
				{
<<<<<<< HEAD
					if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon)
						SetLevel(MarioLevel::Big);
					else if (GetLevel() == MarioLevel::Big)
						SetLevel(MarioLevel::Small);
=======
					SetLevel(MarioLevel::Small);
>>>>>>> dev
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					ChangeState(new CMarioDeadState());
				}
			}
		}
	}
}

void CMario::OnCollisionWithEnemy(LPCOLLISIONEVENT e)
{
	CGameObject* enemy = e->obj;
	if (e->ny < 0)
	{
		if (dynamic_cast<CGoomba*>(enemy))
		{
			CGoomba* goomba = dynamic_cast<CGoomba*>(enemy);
			if (goomba->GetState() != ToInt(GoombaState::Die))
			{
				goomba->SetState(GoombaState::Die); // Ép quái chuyển sang trạng thái dẹp lép
				vy = -MARIO_JUMP_DEFLECT_SPEED;     // Mario nảy tưng lên trời một chút
			}
		}
	}
	//else if (dynamic_cast<CKoopa*>(enemy))
	//{
	//	CKoopa* koopa = dynamic_cast<CKoopa*>(enemy);

	//	// Nếu rùa đang đi bộ bình thường -> Đạp phát đầu tiên bắt nó rụt vào mai rùa
	//	if (koopa->GetState() == KOOPA_STATE_WALKING)
	//	{
	//		koopa->SetState(KOOPA_STATE_SHELL);
	//		vy = -MARIO_JUMP_DEFLECT_SPEED; // Mario nảy lên
	//	}
	//	// Nếu rùa đang nằm im trong mai -> Đạp phát nữa để đá cái mai rùa bay đi
	//	else if (koopa->GetState() == KOOPA_STATE_SHELL)
	//	{
	//		// Trực quan hóa: Xác định hướng đá dựa theo hướng mặt của Mario
	//		koopa->SetKickDirection(this->nx);
	//		koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
	//	}
	//}

	else
	{
		int isHarmful = 1; // Mặc định tất cả các quái đều nguy hiểm

		// Kiểm tra xem thực thể va chạm có phải là Rùa hay không
		//if (dynamic_cast<CKoopa*>(enemy))
		//{
		//	CKoopa* koopa = dynamic_cast<CKoopa*>(enemy);
		//	// Nếu là rùa, ta bốc hàm IsHarmful của riêng lớp Koopa ra check
		//	isHarmful = koopa->IsHarmful();
		//}

		// Nếu thực sự nguy hiểm thì mới trừ máu Mario
		if (isHarmful == 1)
		{
			if (untouchable == 0)
			{
<<<<<<< HEAD
				if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon) 
				{
					SetLevel(MarioLevel::Big); 
					StartUntouchable();      
				}
				else if (GetLevel() == MarioLevel::Big) 
				{
					SetLevel(MarioLevel::Small); 
=======
				if (GetLevel() > MarioLevel::Big) // Nếu đang ở dạng đặc biệt (Chồn, Ếch...)
				{
					SetLevel(MarioLevel::Big); // Rớt về Mario Lớn
					StartUntouchable();      // Bật bất tử tạm thời
				}
				else if (GetLevel() == MarioLevel::Big) // Nếu đang ở Mario Lớn
				{
					SetLevel(MarioLevel::Small); // Rớt về Mario Nhỏ
>>>>>>> dev
					StartUntouchable();
				}
				else 
				{
					ChangeState(new CMarioDeadState());
				}
			}
		}
	}
}



void CMario::OnCollisionWithItem(LPCOLLISIONEVENT e)
{
	CItem* item = dynamic_cast<CItem*>(e->obj);

	if (item)
	{
		item->Delete();
		switch (item->GetItemType())
		{
		case ITEM_TYPE_FLOWER:
<<<<<<< HEAD
			SetLevel(MarioLevel::Fire);
			break;
		case ITEM_TYPE_LEAF:
			SetLevel(MarioLevel::Raccoon);
=======
			if (GetLevel() < MarioLevel::Fire)
			{
				SetLevel(MarioLevel::Fire);
			}
			break;
		case ITEM_TYPE_LEAF:
			if (GetLevel() < MarioLevel::Raccoon)
			{
				SetLevel(MarioLevel::Raccoon);
			}
>>>>>>> dev
			break;
		}
	}
	item->Delete();
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



void CMario::OnCollisionWithBlaster(LPCOLLISIONEVENT e)
{
	if (untouchable != 0) return;

	CBlaster* blaster = dynamic_cast<CBlaster*>(e->obj);
	if (!blaster || blaster->GetState() != static_cast<int>(BlasterState::Firing)) return;

<<<<<<< HEAD
	if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon)
=======
	if (GetLevel() > MarioLevel::Big)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Big);
		StartUntouchable();
	}
<<<<<<< HEAD
	else if (GetLevel() == MarioLevel::Big)
=======
	else if (GetLevel() > MarioLevel::Small)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Small);
		StartUntouchable();
	}
	else
		ChangeState(new CMarioDeadState());
}

void CMario::OnCollisionWithBurner(LPCOLLISIONEVENT e)
{
	if (untouchable != 0) return;

	// Only deal damage when the Burner is actively Firing.
	// IsCollidable() returns 0 during Idle but the collision framework checks
	// IsCollidable() only on the source object (Mario), not on targets — so we
	// must guard here against collisions that fire while the Burner is Idle.
	CBurner* burner = dynamic_cast<CBurner*>(e->obj);
	if (!burner || burner->GetState() != static_cast<int>(BurnerState::Firing)) return;

<<<<<<< HEAD
	if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon)
=======
	if (GetLevel() > MarioLevel::Big)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Big);
		StartUntouchable();
	}
<<<<<<< HEAD
	else if (GetLevel() == MarioLevel::Big)
=======
	else if (GetLevel() > MarioLevel::Small)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Small);
		StartUntouchable();
	}
	else
		ChangeState(new CMarioDeadState());
}

void CMario::OnCollisionWithCannonBall(LPCOLLISIONEVENT e)
{
	if (untouchable != 0) return;

<<<<<<< HEAD
	if (GetLevel() == MarioLevel::Fire || GetLevel() == MarioLevel::Raccoon)
=======
	if (GetLevel() > MarioLevel::Big)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Big);
		StartUntouchable();
	}
<<<<<<< HEAD
	else if (GetLevel() == MarioLevel::Big)
=======
	else if (GetLevel() > MarioLevel::Small)
>>>>>>> dev
	{
		SetLevel(MarioLevel::Small);
		StartUntouchable();
	}
	else
		ChangeState(new CMarioDeadState());
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = ID_ANI_MARIO_BRACE_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = ID_ANI_MARIO_BRACE_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = ID_ANI_MARIO_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = ID_ANI_MARIO_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return aniId;
}

///
/// get animation ID for fire Mario
/// 
int CMario::GetAniIdFire()
{
	int aniId = -1;


	if (isThrowingFire)
	{
		if (nx >= 0)
			aniId = ID_ANI_MARIO_FIRE_THROW_RIGHT;
		else
			aniId = ID_ANI_MARIO_FIRE_THROW_LEFT;
	}
	else if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_LEFT;
		}
	}
	else
	{
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_FIRE_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_SIT_LEFT;
		}
		else if (vx == 0)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_IDLE_LEFT;
		}
		else if (vx > 0)
		{
			if (ax < 0)
				aniId = ID_ANI_MARIO_FIRE_BRACE_RIGHT;
			else if (ax == MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_MARIO_FIRE_RUNNING_RIGHT;
			else if (ax == MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_MARIO_FIRE_WALKING_RIGHT;
		}
		else // vx < 0
		{
			if (ax > 0)
				aniId = ID_ANI_MARIO_FIRE_BRACE_LEFT;
			else if (ax == -MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_MARIO_FIRE_RUNNING_LEFT;
			else if (ax == -MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_MARIO_FIRE_WALKING_LEFT;
		}
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;

	return aniId;
}

/// 
/// Raccoon Mario
/// 
int CMario::GetAniIdRaccoon()
{
	int aniId = -1;
	// MarioState marioState = static_cast<MarioState>(state);
	if (!isOnPlatform)
	{
		// Kiểm tra nếu đang thực sự ở trạng thái Bay hoặc Vỗ đuôi rơi chậm
		if (false) // (marioState == MarioState::Fly || marioState == MarioState::Float)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_FLY_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_FLY_LEFT;
		}
		else if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
		}
	}
	else
	{
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_RACCOON_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_SIT_LEFT;
		}
		else if (vx == 0)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_IDLE_LEFT;
		}
		else if (vx > 0)
		{
			if (ax < 0)
				aniId = ID_ANI_MARIO_RACCOON_BRACE_RIGHT;
			else if (ax == MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_MARIO_RACCOON_RUNNING_RIGHT;
			else if (ax == MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_MARIO_RACCOON_WALKING_RIGHT;
		}
		else // vx < 0
		{
			if (ax > 0)
				aniId = ID_ANI_MARIO_RACCOON_BRACE_LEFT;
			else if (ax == -MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_MARIO_RACCOON_RUNNING_LEFT;
			else if (ax == -MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_MARIO_RACCOON_WALKING_LEFT;
		}
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;

	return aniId;
}

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (currentState && currentState->GetID() == MarioStateID::Dead)
		aniId = ID_ANI_MARIO_DIE;
	else if (GetLevel() == MarioLevel::Big)
		aniId = GetAniIdBig();
	else if (GetLevel() == MarioLevel::Small)
		aniId = GetAniIdSmall();
	else if (GetLevel() == MarioLevel::Fire)
		aniId = GetAniIdFire();
	else if (GetLevel() == MarioLevel::Raccoon)
		aniId = GetAniIdRaccoon();
	animations->Get(aniId)->Render(x, y);

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



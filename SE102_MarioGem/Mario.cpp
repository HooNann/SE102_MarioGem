#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"

#include "Collision.h"
#include "FireBall.h"
#include	"PlayScene.h"

namespace
{
	constexpr int ToInt(MarioState state)
	{
		return static_cast<int>(state);
	}

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
				if (level > MarioLevel::Small)
				{
					level = MarioLevel::Small;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MarioState::Die);
				}
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
		if (untouchable == 0) // Chỉ dính sát thương nếu KHÔNG trong trạng thái nhấp nháy bất tử
		{
			if (level > MarioLevel::Big) // Nếu đang ở dạng đặc biệt (Chồn, Ếch...)
			{
				level = MarioLevel::Big; // Rớt về Mario Lớn
				StartUntouchable();      // Bật bất tử tạm thời
			}
			else if (level == MarioLevel::Big) // Nếu đang ở Mario Lớn
			{
				level = MarioLevel::Small; // Rớt về Mario Nhỏ
				StartUntouchable();
			}
			else // Đang là Mario Nhỏ mà bị cắn
			{
				SetState(MarioState::Die); 
			}
		}
};

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
				if (level > MarioLevel::Big) // Nếu đang ở dạng đặc biệt (Chồn, Ếch...)
				{
					level = MarioLevel::Big; // Rớt về Mario Lớn
					StartUntouchable();      // Bật bất tử tạm thời
				}
				else if (level == MarioLevel::Big) // Nếu đang ở Mario Lớn
				{
					level = MarioLevel::Small; // Rớt về Mario Nhỏ
					StartUntouchable();
				}
				else 
				{
					SetState(MarioState::Die);
				}
			}
		}
	}
}


void CMario::ShootFireBall()
{
	if (this->level != MarioLevel::Fire) return;

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
	if (state == static_cast<int>(MarioState::RunningRight) || state == static_cast<int>(MarioState::RunningLeft))
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
	if (this->level != MarioLevel::Raccoon || pMeter < MARIO_PMETER_MAX) return;

	// Nếu đây là cái nhấp nút cất cánh đầu tiên trên không
	if (state != static_cast<int>(MarioState::Fly))
	{
		SetState(static_cast<int>(MarioState::Fly));
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
	if (this->level == MarioLevel::Raccoon && vy > 0 && isOnPlatform == false)
	{
		SetState(static_cast<int>(MarioState::Float)); // Chuyển sang hành động vỗ đuôi
		vy = 0.03f;                  // Gán một vận tốc rơi cực kỳ nhỏ (hãm phanh trọng lực)
	}
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

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == ToInt(MarioState::Die))
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MarioLevel::Big)
		aniId = GetAniIdBig();
	else if (level == MarioLevel::Small)
		aniId = GetAniIdSmall();

	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(MarioState state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == ToInt(MarioState::Die)) return; 

	switch (state)
	{
	case MarioState::RunningRight:
		if (isSitting) break;
		maxVx = MARIO_RUNNING_SPEED;
		ax = MARIO_ACCEL_RUN_X;
		nx = 1;
		break;
	case MarioState::RunningLeft:
		if (isSitting) break;
		maxVx = -MARIO_RUNNING_SPEED;
		ax = -MARIO_ACCEL_RUN_X;
		nx = -1;
		break;
	case MarioState::WalkingRight:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED;
		ax = MARIO_ACCEL_WALK_X;
		nx = 1;
		break;
	case MarioState::WalkingLeft:
		if (isSitting) break;
		maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCEL_WALK_X;
		nx = -1;
		break;
	case MarioState::Jump:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		break;

	case MarioState::ReleaseJump:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;

	case MarioState::Sit:
		if (isOnPlatform && level != MarioLevel::Small)
		{
			state = MarioState::Idle;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y +=MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MarioState::SitRelease:
		if (isSitting)
		{
			isSitting = false;
			state = MarioState::Idle;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MarioState::Idle:
		ax = 0.0f;
		vx = 0.0f;
		break;

	case MarioState::Die:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	}

	CGameObject::SetState(ToInt(state));
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (level==MarioLevel::Big)
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

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MarioLevel::Small)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = static_cast<MarioLevel>(l);
}

LPGAMEOBJECT CMario::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	return new CMario(x, y);
}


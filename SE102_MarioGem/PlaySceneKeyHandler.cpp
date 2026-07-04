#include "PlaySceneKeyHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"
#include "MarioIdleState.h"
#include "MarioWalkState.h"
#include "MarioRunState.h"
#include "MarioJumpState.h"
#include "MarioFallState.h"
#include "MarioDuckState.h"
#include "MarioDeadState.h"
#include "MarioSkidState.h"
#include "MarioFlyState.h"
#include "MarioFloatState.h"
#include "MarioPipeState.h"
#include "Pipe.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

static bool CanMarioFitInPipe(CMario* mario, CPipe* pipe)
{
	if (pipe == nullptr) return false;
	
	int frontAniId = ID_ANI_MARIO_SMALL_FRONT;
	switch (mario->GetLevel())
	{
	case MarioLevel::Big:     frontAniId = ID_ANI_MARIO_BIG_FRONT;     break;
	case MarioLevel::Fire:    frontAniId = ID_ANI_MARIO_FIRE_FRONT;    break;
	case MarioLevel::Raccoon: frontAniId = ID_ANI_MARIO_RACCOON_FRONT; break;
	default: break;
	}

	LPANIMATION ani = CAnimations::GetInstance()->Get(frontAniId);
	if (ani == nullptr) return true; // fallback: cho phép

	float spriteHalfW = ani->GetSpriteWidth() / 2.0f;
	float mx, my;
	mario->GetPosition(mx, my);
	float sprite_left  = mx - spriteHalfW;
	float sprite_right = mx + spriteHalfW;

	float pl, pt, pr, pb;
	pipe->GetBoundingBox(pl, pt, pr, pb);

	return pl < sprite_left && sprite_right < pr;
}

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (scene->IsCourseClear()) return;

	CMario* mario = (CMario*)scene->GetPlayer();

	if (mario == NULL) return;

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead) return;
    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Pipe) return;

	switch (KeyCode)
	{
	case DIK_DOWN:
	{
		CPipe* pipe = scene->GetOverlappingPipe(mario, PipeDirection::Down);
		if (pipe != nullptr && CanMarioFitInPipe(mario, pipe))
			mario->ChangeState(new CMarioPipeState(pipe));
		else
			mario->ChangeState(new CMarioDuckState());
		break;
	}
	case DIK_Z:
		if (!mario->IsOnPlatform())
		{
			if (mario->GetLevel() == MarioLevel::Raccoon)
			{
                MarioStateID curID = mario->currentState ? mario->currentState->GetID() : MarioStateID::Idle;
				if (mario->IsFlyingPowerActive())
                {
                    if (curID != MarioStateID::Fly)
                    {
					    mario->ChangeState(new CMarioFlyState());
                    }
                    else
                    {
                        // Đã bay rồi, đập cánh tiếp
                        mario->StartFlapping();
                        mario->SetVelocityY(-MARIO_FLY_SPEED_Y);
                        mario->SetAccelerationY(0);
                        CSoundSubject::GetInstance()->Notify(EVENT_TWIRL);
                    }
                }
				else
                {
                    if (mario->GetVelocityY() > 0)
                    {
                        if (curID != MarioStateID::Float)
                        {
                            mario->ChangeState(new CMarioFloatState());
                        }
                        else
                        {
                            // Đã vẫy đuôi rồi, đập cánh lơ lửng tiếp (làm mới float)
                            mario->StartFlapping();
                            mario->SetVelocityY(MARIO_FLOAT_SPEED_Y);
                            mario->SetAccelerationY(0);
                            CSoundSubject::GetInstance()->Notify(EVENT_TWIRL);
                        }
                    }
                }
			}
		}
		else
		{
			mario->ChangeState(new CMarioJumpState());
		}
		break;
	case DIK_1:
		mario->SetLevel(MarioLevel::Small);
		break;
	case DIK_2:
		mario->SetLevel(MarioLevel::Big);
		break;
	case DIK_3:
		mario->SetLevel(MarioLevel::Raccoon);
		break;
	case DIK_4:
		mario->SetLevel(MarioLevel::Fire);
		break;
	case DIK_X:
		mario->ShootFireBall();
		break;
	case DIK_0:
		mario->ChangeState(new CMarioDeadState());
		break;
	case DIK_R: // reset
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->ReloadAssets();
		break;
	}
}

void CPlaySceneKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (scene->IsCourseClear()) return;

	CMario* mario = (CMario*)scene->GetPlayer();

	if (mario == NULL) return;

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead) return;
    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Pipe) return;

	switch (KeyCode)
	{
	case DIK_Z:
        if (!mario->IsOnPlatform())
        {
            MarioStateID curID = mario->currentState ? mario->currentState->GetID() : MarioStateID::Idle;
            if (mario->GetLevel() == MarioLevel::Raccoon && (curID == MarioStateID::Fly || curID == MarioStateID::Float))
            {
                // Bỏ qua chuyển sang FallState để tránh đứt animation, timer sẽ tự lo
            }
            else
            {
		        mario->ChangeState(new CMarioFallState());
            }
        }
		break;
	case DIK_DOWN:
        // Exiting DuckState is handled in Exit(), but we need to transition out to Idle
		mario->ChangeState(new CMarioIdleState());
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE *states)
{
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (scene->IsCourseClear()) return;

	LPGAME game = CGame::GetInstance();
	CMario* mario = (CMario*)scene->GetPlayer();

	if (mario == NULL) return;

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Duck)
        return;
    
    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead)
        return;

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Pipe)
        return;

    MarioStateID curID = mario->currentState ? mario->currentState->GetID() : MarioStateID::Idle;

    // Không ngăn đổi state ở đây nữa để giữ Air Control (di chuyển trái phải).
    // Các logic Air Control bên dưới không làm đổi state (chỉ đổi vận tốc/gia tốc).

	if (game->IsKeyDown(DIK_UP))
	{
		CPipe* pipe = scene->GetOverlappingPipe(mario, PipeDirection::Up);
		if (pipe != nullptr && CanMarioFitInPipe(mario, pipe))
		{
			mario->ChangeState(new CMarioPipeState(pipe));
			return;
		}
	}

	if (!mario->IsOnPlatform())
	{
		// HÀNH VI TRÊN KHÔNG (Air Control)
		// Không thay đổi State (để giữ Jump/Fall/Fly/Float), chỉ tác động lên gia tốc và vận tốc ngang
		if (game->IsKeyDown(DIK_RIGHT))
		{
			mario->SetDirection(1);
			float maxSpeed = game->IsKeyDown(DIK_X) ? MARIO_RUNNING_SPEED : MARIO_WALKING_SPEED;
			mario->SetMaxVelocityX(maxSpeed);
			mario->SetAccelerationX(MARIO_ACCEL_WALK_X);
		}
		else if (game->IsKeyDown(DIK_LEFT))
		{
			mario->SetDirection(-1);
			float maxSpeed = game->IsKeyDown(DIK_X) ? -MARIO_RUNNING_SPEED : -MARIO_WALKING_SPEED;
			mario->SetMaxVelocityX(maxSpeed);
			mario->SetAccelerationX(-MARIO_ACCEL_WALK_X);
		}
		else
		{
			// Không bấm phím -> trôi theo quán tính
			mario->SetAccelerationX(0.0f);
		}
	}
	else
	{
		// HÀNH VI TRÊN MẶT ĐẤT (Ground Control)
		if (game->IsKeyDown(DIK_RIGHT))
		{
			mario->SetDirection(1);
			if (mario->GetVelocityX() < 0)
			{
				if (curID != MarioStateID::Skid) mario->ChangeState(new CMarioSkidState());
			}
			else if (game->IsKeyDown(DIK_X))
			{
				if (curID != MarioStateID::Run) mario->ChangeState(new CMarioRunState());
			}
			else
			{
				if (curID != MarioStateID::Walk) mario->ChangeState(new CMarioWalkState());
			}
		}
		else if (game->IsKeyDown(DIK_LEFT))
		{
			mario->SetDirection(-1);
			if (mario->GetVelocityX() > 0)
			{
				if (curID != MarioStateID::Skid) mario->ChangeState(new CMarioSkidState());
			}
			else if (game->IsKeyDown(DIK_X))
			{
				if (curID != MarioStateID::Run) mario->ChangeState(new CMarioRunState());
			}
			else
			{
				if (curID != MarioStateID::Walk) mario->ChangeState(new CMarioWalkState());
			}
		}
		else if (curID != MarioStateID::Idle)
		{
			mario->ChangeState(new CMarioIdleState());
		}
	}
}

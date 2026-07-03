#include "PlaySceneKeyHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"
#include "CMarioIdleState.h"
#include "CMarioWalkState.h"
#include "CMarioRunState.h"
#include "CMarioJumpState.h"
#include "CMarioFallState.h"
#include "CMarioDuckState.h"
#include "CMarioDeadState.h"
#include "CMarioSkidState.h"
#include "CMarioFlyState.h"
#include "CMarioFloatState.h"
#include "CMarioPipeState.h"
#include "Pipe.h"

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (scene->IsCourseClear()) return;

	CMario* mario = (CMario*)scene->GetPlayer(); 

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead) return;
    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Pipe) return;

	switch (KeyCode)
	{
	case DIK_DOWN:
	{
		CPipe* pipe = scene->GetOverlappingPipe(mario);
		if (pipe != nullptr)
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

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Duck)
        return;
    
    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead)
        return;

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Pipe)
        return;

    MarioStateID curID = mario->currentState ? mario->currentState->GetID() : MarioStateID::Idle;

    // Không ngăn đổi state ở đây nữa để giữ Air Control (di chuyển trái phải).
    // Các logic Air Control bên dưới không làm đổi state (chỉ đổi vận tốc/gia tốc).

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

#include "CMarioFlyState.h"
#include "CMarioFallState.h"
#include "Mario.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

void CMarioFlyState::Enter(CMario* mario)
{
    // Cất cánh
    mario->StartFlapping();
    mario->SetVelocityY(-MARIO_FLY_SPEED_Y);
    mario->SetOnPlatform(false);
    mario->SetAccelerationY(0);
    CSoundSubject::GetInstance()->Notify(EVENT_TWIRL);
}

void CMarioFlyState::Exit(CMario* mario)
{
    mario->SetAccelerationY(MARIO_GRAVITY);
}

void CMarioFlyState::Update(CMario* mario, DWORD dt)
{
    // Nếu hết thời gian bay, tự rơi
    if (!mario->IsFlyingPowerActive())
    {
        // pMeter is already reset in HandlePMeter
        mario->ChangeState(new CMarioFallState());
        return;
    }

    if (GetTickCount64() - mario->GetFlapStartTime() > 250)
    {
        mario->SetAccelerationY(MARIO_GRAVITY_JUMP);
    }

    // Nếu rớt thì rơi
    if (mario->GetVelocityY() > 0)
    {
        mario->ChangeState(new CMarioFallState());
    }
}

int CMarioFlyState::GetAnimationId(CMario* mario)
{
    if (mario->GetLevel() == MarioLevel::Raccoon) return ID_ANI_MARIO_RACCOON_FLY;
    return ID_ANI_MARIO_SMALL_JUMP;
}

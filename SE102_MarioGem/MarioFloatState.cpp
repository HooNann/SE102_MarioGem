#include "MarioFloatState.h"
#include "MarioFallState.h"
#include "Mario.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

void CMarioFloatState::Enter(CMario* mario)
{
    mario->StartFlapping();
    mario->SetVelocityY(MARIO_FLOAT_SPEED_Y);
    mario->SetAccelerationY(0);
    CSoundSubject::GetInstance()->Notify(EVENT_TWIRL);
}

void CMarioFloatState::Exit(CMario* mario)
{
    mario->SetAccelerationY(MARIO_GRAVITY);
}

void CMarioFloatState::Update(CMario* mario, DWORD dt)
{
    if (GetTickCount64() - mario->GetFlapStartTime() > 250)
    {
        mario->ChangeState(new CMarioFallState());
    }
}

int CMarioFloatState::GetAnimationId(CMario* mario)
{
    if (mario->GetLevel() == MarioLevel::Raccoon) return ID_ANI_MARIO_RACCOON_FLOAT;
    return ID_ANI_MARIO_SMALL_JUMP;
}

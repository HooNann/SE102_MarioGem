#include "CMarioFloatState.h"
#include "CMarioFallState.h"
#include "Mario.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

void CMarioFloatState::Enter(CMario* mario)
{
    // Hãm đà rơi lại (rơi chậm)
    mario->StartFlapping();
    mario->SetVelocityY(MARIO_FLOAT_SPEED_Y);
    mario->SetAccelerationY(0); // Chuyển động đều
    CSoundSubject::GetInstance()->Notify(EVENT_TWIRL);
}

void CMarioFloatState::Exit(CMario* mario)
{
    mario->SetAccelerationY(MARIO_GRAVITY);
}

void CMarioFloatState::Update(CMario* mario, DWORD dt)
{
    // Sau 1 khoảng vẫy đuôi, tự động trả về rơi tự do
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

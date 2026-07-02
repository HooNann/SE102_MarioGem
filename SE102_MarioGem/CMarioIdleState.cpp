#include "CMarioIdleState.h"
#include "Mario.h"

void CMarioIdleState::Enter(CMario* mario)
{
    mario->SetAccelerationX(0.0f);
    mario->SetVelocityX(0.0f);
}

void CMarioIdleState::Update(CMario* mario, DWORD dt)
{
}

int CMarioIdleState::GetAnimationId(CMario* mario)
{
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_IDLE;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_IDLE;
    case MarioLevel::Fire:
        if (mario->IsThrowingFire())
            return ID_ANI_MARIO_FIRE_THROW;
        return ID_ANI_MARIO_FIRE_IDLE;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_IDLE;
    }
    return ID_ANI_MARIO_SMALL_IDLE;
}

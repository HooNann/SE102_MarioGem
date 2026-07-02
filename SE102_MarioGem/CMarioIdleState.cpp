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
    int nx = mario->GetDirection();
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return (nx > 0) ? ID_ANI_MARIO_SMALL_IDLE_RIGHT : ID_ANI_MARIO_SMALL_IDLE_LEFT;
    case MarioLevel::Big: return (nx > 0) ? ID_ANI_MARIO_IDLE_RIGHT : ID_ANI_MARIO_IDLE_LEFT;
    case MarioLevel::Fire:
        if (mario->IsThrowingFire())
            return (nx > 0) ? ID_ANI_MARIO_FIRE_THROW_RIGHT : ID_ANI_MARIO_FIRE_THROW_LEFT;
        return (nx > 0) ? ID_ANI_MARIO_FIRE_IDLE_RIGHT : ID_ANI_MARIO_FIRE_IDLE_LEFT;
    case MarioLevel::Raccoon: return (nx > 0) ? ID_ANI_MARIO_RACCOON_IDLE_RIGHT : ID_ANI_MARIO_RACCOON_IDLE_LEFT;
    }
    return ID_ANI_MARIO_SMALL_IDLE_RIGHT;
}

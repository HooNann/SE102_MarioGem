#include "CMarioDuckState.h"
#include "Mario.h"

void CMarioDuckState::Enter(CMario* mario)
{
    if (mario->IsOnPlatform() && mario->GetLevel() != MarioLevel::Small)
    {
        mario->SetSitting(true);
        mario->SetVelocityX(0.0f);
        mario->SetVelocityY(0.0f);
        mario->SetY(mario->GetY() + MARIO_SIT_HEIGHT_ADJUST);
    }
}

void CMarioDuckState::Update(CMario* mario, DWORD dt)
{
}

int CMarioDuckState::GetAnimationId(CMario* mario)
{
    int nx = mario->GetDirection();
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return (nx > 0) ? ID_ANI_MARIO_SMALL_IDLE_RIGHT : ID_ANI_MARIO_SMALL_IDLE_LEFT;
    case MarioLevel::Big: return (nx > 0) ? ID_ANI_MARIO_SIT_RIGHT : ID_ANI_MARIO_SIT_LEFT;
    case MarioLevel::Fire: return (nx > 0) ? ID_ANI_MARIO_FIRE_SIT_RIGHT : ID_ANI_MARIO_FIRE_SIT_LEFT;
    case MarioLevel::Raccoon: return (nx > 0) ? ID_ANI_MARIO_RACCOON_SIT_RIGHT : ID_ANI_MARIO_RACCOON_SIT_LEFT;
    }
    return ID_ANI_MARIO_SMALL_IDLE_RIGHT;
}

void CMarioDuckState::Exit(CMario* mario)
{
    if (mario->IsSitting())
    {
        mario->SetSitting(false);
        mario->SetY(mario->GetY() - MARIO_SIT_HEIGHT_ADJUST);
    }
}

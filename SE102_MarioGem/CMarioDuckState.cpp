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
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_CROUCH;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_CROUCH;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_CROUCH;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_CROUCH;
    }
    return ID_ANI_MARIO_SMALL_CROUCH;
}

void CMarioDuckState::Exit(CMario* mario)
{
    if (mario->IsSitting())
    {
        mario->SetSitting(false);
        mario->SetY(mario->GetY() - MARIO_SIT_HEIGHT_ADJUST);
    }
}

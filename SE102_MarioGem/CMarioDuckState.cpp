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

void CMarioDuckState::Exit(CMario* mario)
{
    if (mario->IsSitting())
    {
        mario->SetSitting(false);
        mario->SetY(mario->GetY() - MARIO_SIT_HEIGHT_ADJUST);
    }
}

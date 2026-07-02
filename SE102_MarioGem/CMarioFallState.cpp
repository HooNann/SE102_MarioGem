#include "CMarioFallState.h"
#include "Mario.h"

void CMarioFallState::Enter(CMario* mario)
{
    if (mario->GetVelocityY() < 0) 
    {
        mario->SetVelocityY(mario->GetVelocityY() + MARIO_JUMP_SPEED_Y / 2);
    }
}

void CMarioFallState::Update(CMario* mario, DWORD dt)
{
}

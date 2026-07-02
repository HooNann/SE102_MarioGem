#include "CMarioJumpState.h"
#include "Mario.h"
#include <math.h>

void CMarioJumpState::Enter(CMario* mario)
{
    if (mario->IsOnPlatform())
    {
        if (abs(mario->GetVelocityX()) == MARIO_RUNNING_SPEED)
            mario->SetVelocityY(-MARIO_JUMP_RUN_SPEED_Y);
        else
            mario->SetVelocityY(-MARIO_JUMP_SPEED_Y);
        
        mario->SetOnPlatform(false);
    }
}

void CMarioJumpState::Update(CMario* mario, DWORD dt)
{
}

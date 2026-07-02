#include "CMarioRunState.h"
#include "Mario.h"

void CMarioRunState::Enter(CMario* mario)
{
}

void CMarioRunState::Update(CMario* mario, DWORD dt)
{
    float dir = (float)mario->GetDirection();
    mario->SetMaxVelocityX(MARIO_RUNNING_SPEED * dir);
    mario->SetAccelerationX(MARIO_ACCEL_RUN_X * dir);
}

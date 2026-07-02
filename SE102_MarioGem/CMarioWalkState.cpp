#include "CMarioWalkState.h"
#include "Mario.h"

void CMarioWalkState::Enter(CMario* mario)
{
}

void CMarioWalkState::Update(CMario* mario, DWORD dt)
{
    float dir = (float)mario->GetDirection();
    mario->SetMaxVelocityX(MARIO_WALKING_SPEED * dir);
    mario->SetAccelerationX(MARIO_ACCEL_WALK_X * dir);
}

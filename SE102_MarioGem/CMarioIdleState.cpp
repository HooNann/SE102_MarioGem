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

#include "CMarioDeadState.h"
#include "Mario.h"

void CMarioDeadState::Enter(CMario* mario)
{
    mario->SetVelocityY(-MARIO_JUMP_DEFLECT_SPEED);
    mario->SetVelocityX(0.0f);
    mario->SetAccelerationX(0.0f);
}

void CMarioDeadState::Update(CMario* mario, DWORD dt)
{
}

int CMarioDeadState::GetAnimationId(CMario* mario)
{
    return ID_ANI_MARIO_DIE;
}

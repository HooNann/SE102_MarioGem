#include "MarioPitDeadState.h"
#include "Mario.h"

void CMarioPitDeadState::Enter(CMario* mario)
{
	mario->SetVelocityX(0.0f);
	mario->SetAccelerationX(0.0f);
	mario->SetAccelerationY(MARIO_GRAVITY);
}

void CMarioPitDeadState::Update(CMario* mario, DWORD dt)
{
}

int CMarioPitDeadState::GetAnimationId(CMario* mario)
{
	return ID_ANI_MARIO_SMALL_DIE;
}

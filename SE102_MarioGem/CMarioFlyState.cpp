#include "CMarioFlyState.h"
#include "CMarioFallState.h"
#include "Mario.h"

void CMarioFlyState::Enter(CMario* mario)
{
    // Cất cánh
    mario->StartFlying();
    mario->SetVelocityY(-MARIO_JUMP_MAX_SPEED_Y);
    mario->SetOnPlatform(false);
    mario->SetAccelerationY(MARIO_GRAVITY_JUMP); // Trọng lực thấp khi giữ phím Z
}

void CMarioFlyState::Update(CMario* mario, DWORD dt)
{
    // Nếu hết thời gian bay, tự rơi
    if (GetTickCount64() - mario->GetFlyStartTime() > MARIO_FLYING_TIME_MAX)
    {
        mario->ResetPMeter(); // Cạn pin
        mario->ChangeState(new CMarioFallState());
        return;
    }

    // Nếu rớt thì rơi
    if (mario->GetVelocityY() > 0)
    {
        mario->ChangeState(new CMarioFallState());
    }
}

int CMarioFlyState::GetAnimationId(CMario* mario)
{
    if (mario->GetLevel() == MarioLevel::Raccoon) return ID_ANI_MARIO_RACCOON_FLY;
    return ID_ANI_MARIO_SMALL_JUMP;
}

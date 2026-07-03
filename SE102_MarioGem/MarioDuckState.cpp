#include "MarioDuckState.h"
#include "Mario.h"

void CMarioDuckState::Enter(CMario* mario)
{
    if (mario->IsOnPlatform() && mario->GetLevel() != MarioLevel::Small)
    {
        mario->SetSitting(true);
        mario->SetY(mario->GetY() + MARIO_SIT_HEIGHT_ADJUST);
    }

    // Luôn áp dụng ma sát trượt thay vì dừng đột ngột
    float vx = mario->GetVelocityX();
    if (vx != 0)
    {
        float friction = (vx > 0) ? -MARIO_ACCEL_WALK_X : MARIO_ACCEL_WALK_X;
        mario->SetAccelerationX(friction);
    }
    else
    {
        mario->SetAccelerationX(0.0f);
    }
}

void CMarioDuckState::Update(CMario* mario, DWORD dt)
{
    float vx = mario->GetVelocityX();
    float ax = mario->GetAccelerationX();

    // Giống IdleState: Nếu ma sát kéo vận tốc qua 0 thì dừng hẳn
    if (vx * ax > 0 || vx == 0)
    {
        mario->SetVelocityX(0.0f);
        mario->SetAccelerationX(0.0f);
    }
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

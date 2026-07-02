#include "CMarioIdleState.h"
#include "Mario.h"

#include <math.h>

void CMarioIdleState::Enter(CMario* mario)
{
    float vx = mario->GetVelocityX();
    if (vx != 0)
    {
        // Áp dụng ma sát ngược chiều di chuyển
        float friction = (vx > 0) ? -MARIO_ACCEL_WALK_X : MARIO_ACCEL_WALK_X;
        mario->SetAccelerationX(friction);
    }
    else
    {
        mario->SetAccelerationX(0.0f);
    }
}

void CMarioIdleState::Update(CMario* mario, DWORD dt)
{
    float vx = mario->GetVelocityX();
    float ax = mario->GetAccelerationX();

    // Nếu ma sát đã kéo vận tốc vượt quá 0, hoặc vận tốc đã chạm 0 -> dừng hẳn
    if (vx * ax > 0 || vx == 0)
    {
        mario->SetVelocityX(0.0f);
        mario->SetAccelerationX(0.0f);
    }
}

int CMarioIdleState::GetAnimationId(CMario* mario)
{
    if (!mario->IsOnPlatform())
    {
        switch (mario->GetLevel())
        {
        case MarioLevel::Small: return ID_ANI_MARIO_SMALL_JUMP;
        case MarioLevel::Big: return mario->GetVelocityY() > 0 ? ID_ANI_MARIO_BIG_FALL : ID_ANI_MARIO_BIG_JUMP;
        case MarioLevel::Fire: return mario->GetVelocityY() > 0 ? ID_ANI_MARIO_FIRE_FALL : ID_ANI_MARIO_FIRE_JUMP;
        case MarioLevel::Raccoon: return mario->GetVelocityY() > 0 ? ID_ANI_MARIO_RACCOON_FALL : ID_ANI_MARIO_RACCOON_JUMP;
        }
    }

    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_IDLE;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_IDLE;
    case MarioLevel::Fire:
        if (mario->IsThrowingFire())
            return ID_ANI_MARIO_FIRE_THROW;
        return ID_ANI_MARIO_FIRE_IDLE;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_IDLE;
    }
    return ID_ANI_MARIO_SMALL_IDLE;
}

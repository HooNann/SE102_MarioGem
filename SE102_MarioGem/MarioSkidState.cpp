#include "MarioSkidState.h"
#include "MarioWalkState.h"
#include "MarioIdleState.h"
#include "Mario.h"

void CMarioSkidState::Enter(CMario* mario)
{
    // Reset P-Meter immediately
    mario->ResetPMeter();

    // Apply skid acceleration in opposite direction of current velocity
    float vx = mario->GetVelocityX();
    float ax = (vx > 0) ? -MARIO_ACCEL_SKID_X : MARIO_ACCEL_SKID_X;
    mario->SetAccelerationX(ax);
}

void CMarioSkidState::Update(CMario* mario, DWORD dt)
{
    float vx = mario->GetVelocityX();
    float ax = mario->GetAccelerationX();

    // If velocity and acceleration have same sign, it means we have stopped skidding and started moving in the new direction
    // Or if vx reaches 0, we stopped
    if (vx * ax > 0 || vx == 0)
    {
        mario->SetVelocityX(0); // Snap to 0
        mario->ChangeState(new CMarioWalkState()); // Hand back control (will pick up Run or Walk on next frame)
    }
}

int CMarioSkidState::GetAnimationId(CMario* mario)
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
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_SKID;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_SKID;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_SKID;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_SKID;
    }
    return ID_ANI_MARIO_SMALL_SKID;
}

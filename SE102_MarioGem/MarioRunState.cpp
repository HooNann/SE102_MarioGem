#include "MarioRunState.h"
#include "Mario.h"

void CMarioRunState::Enter(CMario* mario)
{
}

void CMarioRunState::Update(CMario* mario, DWORD dt)
{
    float dir = (float)mario->GetDirection();
    float maxSpeed = (mario->GetPMeter() == MARIO_PMETER_MAX) ? MARIO_MAX_SPEED : MARIO_RUNNING_SPEED;
    
    mario->SetMaxVelocityX(maxSpeed * dir);
    mario->SetAccelerationX(MARIO_ACCEL_RUN_X * dir);
}

int CMarioRunState::GetAnimationId(CMario* mario)
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

    if (mario->GetPMeter() == MARIO_PMETER_MAX)
    {
        switch (mario->GetLevel())
        {
        case MarioLevel::Small: return ID_ANI_MARIO_SMALL_RUN;
        case MarioLevel::Big: return ID_ANI_MARIO_BIG_HIGH_SPEED;
        case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_HIGH_SPEED;
        case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_RUN; // Raccoon run is speed up
        }
    }

    // Normal running (not max P-Meter) uses Walk animation but plays faster (engine dependent)
    // Small Mario only has Walk and Run. Run is used for max speed.
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_WALK; // Walk animation for normal run
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_RUN; // Big Mario run is normal run, high speed is sprint
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_RUN;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_WALK;
    }
    return ID_ANI_MARIO_SMALL_WALK;
}

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

int CMarioRunState::GetAnimationId(CMario* mario)
{
    float vx = mario->GetVelocityX();
    float ax = mario->GetAccelerationX();
    
    // Check skid (brace)
    if (vx * ax < 0) {
        switch (mario->GetLevel()) {
            case MarioLevel::Small: return ID_ANI_MARIO_SMALL_SKID;
            case MarioLevel::Big: return ID_ANI_MARIO_BIG_SKID;
            case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_SKID;
            case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_SKID;
        }
    }
    
    // Normal running
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_RUN;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_RUN;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_RUN;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_RUN;
    }
    return ID_ANI_MARIO_SMALL_RUN;
}

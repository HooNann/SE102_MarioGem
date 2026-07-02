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
    int nx = mario->GetDirection();
    float vx = mario->GetVelocityX();
    float ax = mario->GetAccelerationX();
    
    // Check brace (skidding)
    if (vx > 0 && ax < 0) {
        switch (mario->GetLevel()) {
            case MarioLevel::Small: return ID_ANI_MARIO_SMALL_BRACE_RIGHT;
            case MarioLevel::Big: return ID_ANI_MARIO_BRACE_RIGHT;
            case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_BRACE_RIGHT;
            case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_BRACE_RIGHT;
        }
    } else if (vx < 0 && ax > 0) {
        switch (mario->GetLevel()) {
            case MarioLevel::Small: return ID_ANI_MARIO_SMALL_BRACE_LEFT;
            case MarioLevel::Big: return ID_ANI_MARIO_BRACE_LEFT;
            case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_BRACE_LEFT;
            case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_BRACE_LEFT;
        }
    }
    
    // Normal running
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return (nx > 0) ? ID_ANI_MARIO_SMALL_RUNNING_RIGHT : ID_ANI_MARIO_SMALL_RUNNING_LEFT;
    case MarioLevel::Big: return (nx > 0) ? ID_ANI_MARIO_RUNNING_RIGHT : ID_ANI_MARIO_RUNNING_LEFT;
    case MarioLevel::Fire: return (nx > 0) ? ID_ANI_MARIO_FIRE_RUNNING_RIGHT : ID_ANI_MARIO_FIRE_RUNNING_LEFT;
    case MarioLevel::Raccoon: return (nx > 0) ? ID_ANI_MARIO_RACCOON_RUNNING_RIGHT : ID_ANI_MARIO_RACCOON_RUNNING_LEFT;
    }
    return ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
}

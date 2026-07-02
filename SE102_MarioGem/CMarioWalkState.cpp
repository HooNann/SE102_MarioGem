#include "CMarioWalkState.h"
#include "Mario.h"

void CMarioWalkState::Enter(CMario* mario)
{
}

void CMarioWalkState::Update(CMario* mario, DWORD dt)
{
    float dir = (float)mario->GetDirection();
    mario->SetMaxVelocityX(MARIO_WALKING_SPEED * dir);
    mario->SetAccelerationX(MARIO_ACCEL_WALK_X * dir);
}

int CMarioWalkState::GetAnimationId(CMario* mario)
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
    
    // Normal walking
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return (nx > 0) ? ID_ANI_MARIO_SMALL_WALKING_RIGHT : ID_ANI_MARIO_SMALL_WALKING_LEFT;
    case MarioLevel::Big: return (nx > 0) ? ID_ANI_MARIO_WALKING_RIGHT : ID_ANI_MARIO_WALKING_LEFT;
    case MarioLevel::Fire: return (nx > 0) ? ID_ANI_MARIO_FIRE_WALKING_RIGHT : ID_ANI_MARIO_FIRE_WALKING_LEFT;
    case MarioLevel::Raccoon: return (nx > 0) ? ID_ANI_MARIO_RACCOON_WALKING_RIGHT : ID_ANI_MARIO_RACCOON_WALKING_LEFT;
    }
    return ID_ANI_MARIO_SMALL_WALKING_RIGHT;
}

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
    
    // Normal walking
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_WALK;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_WALK;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_WALK;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_WALK;
    }
    return ID_ANI_MARIO_SMALL_WALK;
}

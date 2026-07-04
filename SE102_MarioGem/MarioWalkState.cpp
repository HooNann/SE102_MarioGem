#include "MarioWalkState.h"
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

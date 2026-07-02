#include "CMarioJumpState.h"
#include "Mario.h"
#include <math.h>

void CMarioJumpState::Enter(CMario* mario)
{
    if (mario->IsOnPlatform())
    {
        if (abs(mario->GetVelocityX()) == MARIO_RUNNING_SPEED)
            mario->SetVelocityY(-MARIO_JUMP_RUN_SPEED_Y);
        else
            mario->SetVelocityY(-MARIO_JUMP_SPEED_Y);
        
        mario->SetOnPlatform(false);
    }
}

void CMarioJumpState::Update(CMario* mario, DWORD dt)
{
}

int CMarioJumpState::GetAnimationId(CMario* mario)
{
    int nx = mario->GetDirection();
    float ax = mario->GetAccelerationX();
    bool isRunJump = (abs(ax) == MARIO_ACCEL_RUN_X);
    
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: 
        if (isRunJump) return (nx >= 0) ? ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT : ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
        return (nx >= 0) ? ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT : ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
    case MarioLevel::Big: 
        if (isRunJump) return (nx >= 0) ? ID_ANI_MARIO_JUMP_RUN_RIGHT : ID_ANI_MARIO_JUMP_RUN_LEFT;
        return (nx >= 0) ? ID_ANI_MARIO_JUMP_WALK_RIGHT : ID_ANI_MARIO_JUMP_WALK_LEFT;
    case MarioLevel::Fire:
        if (isRunJump) return (nx >= 0) ? ID_ANI_MARIO_FIRE_JUMP_RUN_RIGHT : ID_ANI_MARIO_FIRE_JUMP_RUN_LEFT;
        return (nx >= 0) ? ID_ANI_MARIO_FIRE_JUMP_WALK_RIGHT : ID_ANI_MARIO_FIRE_JUMP_WALK_LEFT;
    case MarioLevel::Raccoon: 
        if (isRunJump) return (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
        return (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
    }
    return ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
}

#include "Goomba.h"

void CMarioJumpState::OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e)
{
    // Đạp lên đầu quái vật
    if (e->ny < 0)
    {
        if (dynamic_cast<CGoomba*>(e->obj))
        {
            CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
            if (goomba->GetState() != static_cast<int>(GoombaState::Die))
            {
                goomba->SetState(GoombaState::Die);
                mario->SetVelocityY(-MARIO_JUMP_DEFLECT_SPEED);
                return;
            }
        }
    }

    CMarioState::OnCollisionWith(mario, e);
}

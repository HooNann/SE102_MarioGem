#include "CMarioFallState.h"
#include "Mario.h"

void CMarioFallState::Enter(CMario* mario)
{
    if (mario->GetVelocityY() < 0) 
    {
        mario->SetVelocityY(mario->GetVelocityY() + MARIO_JUMP_SPEED_Y / 2);
    }
}

void CMarioFallState::Update(CMario* mario, DWORD dt)
{
}

int CMarioFallState::GetAnimationId(CMario* mario)
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
        // For Raccoon, falling can be floating (fly_left/right) if Z is pressed, but let's stick to basic animation for now. 
        // In the original code, `isFlying` changes animation to FLY. Here, we check falling normally.
        if (isRunJump) return (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
        return (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
    }
    return ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
}

#include "Goomba.h"
void CMarioFallState::OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e)
{
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

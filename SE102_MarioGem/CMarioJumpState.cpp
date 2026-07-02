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
    // float ax = mario->GetAccelerationX();
    // bool isRunJump = (abs(ax) == MARIO_ACCEL_RUN_X);
    // Usually new_mario.txt only has 1 jump animation per form, except maybe Raccoon?
    // Let's map everything to JUMP. If we need a specific RUN JUMP we can adjust later,
    // but the provided list only has one JUMP.
    
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_JUMP;
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_JUMP;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_JUMP;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_JUMP;
    }
    return ID_ANI_MARIO_SMALL_JUMP;
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

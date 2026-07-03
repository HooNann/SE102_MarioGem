#include "MarioFallState.h"
#include "Mario.h"
#include "Goomba.h"

void CMarioFallState::Enter(CMario* mario)
{
    mario->SetAccelerationY(MARIO_GRAVITY);
}

void CMarioFallState::Update(CMario* mario, DWORD dt)
{
}

int CMarioFallState::GetAnimationId(CMario* mario)
{
    switch (mario->GetLevel())
    {
    case MarioLevel::Small: return ID_ANI_MARIO_SMALL_JUMP; // new_mario.txt uses jump for small fall too
    case MarioLevel::Big: return ID_ANI_MARIO_BIG_FALL;
    case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_FALL;
    case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_FALL;
    }
    return ID_ANI_MARIO_SMALL_JUMP;
}


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

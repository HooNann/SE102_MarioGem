#include "MarioState.h"
#include "Mario.h"
#include "Game.h"
#include "Coin.h"
#include "Portal.h"
#include "Item.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Blaster.h"
#include "Burner.h"
#include "CannonBall.h"
#include "RedVenus.h"
#include "VenusFireBall.h"
#include "QuestionBlock.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

void CMarioState::OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e)
{
    // Húc gạch từ dưới lên (luôn đúng ở mọi trạng thái nếu va chạm từ dưới)
    if (e->ny > 0)
    {
        if (dynamic_cast<CQuestionBlock*>(e->obj))
        {
            mario->SetVelocityY(0); 
            CQuestionBlock* block = dynamic_cast<CQuestionBlock*>(e->obj);
            if (block->GetState() != QUESTION_BLOCK_STATE_EMPTY)
            {
                block->SetState(QUESTION_BLOCK_STATE_HIT);
                CSoundSubject::GetInstance()->Notify(EVENT_BLOCK);
            }
            return;
        }
    }

    if (dynamic_cast<CCoin*>(e->obj))
    {
        e->obj->Delete();
        CGameData::GetInstance()->AddCoin(1);
        CGameData::GetInstance()->AddScore(100);
        CSoundSubject::GetInstance()->Notify(EVENT_COIN);
    }
    else if (dynamic_cast<CPortal*>(e->obj))
    {
        CPortal* p = (CPortal*)e->obj;
        CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
    }
    else if (dynamic_cast<CItem*>(e->obj))
    {
        CItem* item = dynamic_cast<CItem*>(e->obj);
        if (item)
        {
            item->Delete();
            switch (item->GetItemType())
            {
            case ITEM_TYPE_FLOWER:
                mario->SetLevel(MarioLevel::Fire);
                CSoundSubject::GetInstance()->Notify(EVENT_POWERUP);
                break;
            case ITEM_TYPE_LEAF:
                mario->SetLevel(MarioLevel::Raccoon);
                CSoundSubject::GetInstance()->Notify(EVENT_TANOOKI);
                break;
            }
        }
    }
    // Xử lý mặc định khi chạm trúng quái vật / bẫy (Nếu các state không chặn lại)
    else if (dynamic_cast<CGoomba*>(e->obj) || 
             dynamic_cast<CKoopas*>(e->obj) ||
             dynamic_cast<CBlaster*>(e->obj) || 
             dynamic_cast<CBurner*>(e->obj) || 
             dynamic_cast<CCannonBall*>(e->obj) ||
             dynamic_cast<CRedVenus*>(e->obj) ||
             dynamic_cast<CVenusFireBall*>(e->obj))
    {
        if (dynamic_cast<CKoopas*>(e->obj)) {
            CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
            if (koopas && koopas->IsShellIdle()) {
                float mx, my, kx, ky;
                mario->GetPosition(mx, my);
                koopas->GetPosition(kx, ky);
                koopas->Kick(mx < kx ? 1 : -1);
                CSoundSubject::GetInstance()->Notify(EVENT_KICK);
                return;
            }

            if (!koopas || !koopas->IsDangerousToMario()) return;
        }

        if (dynamic_cast<CBurner*>(e->obj)) {
            CBurner* burner = dynamic_cast<CBurner*>(e->obj);
            if (!burner || burner->GetState() != static_cast<int>(BurnerState::Firing)) return;
        }
        if (dynamic_cast<CBlaster*>(e->obj)) {
            CBlaster* blaster = dynamic_cast<CBlaster*>(e->obj);
            if (!blaster || blaster->GetState() != static_cast<int>(BlasterState::Firing)) return;
        }

        if (dynamic_cast<CVenusFireBall*>(e->obj))
            e->obj->Delete();

        mario->TakeDamage();
    }
}

int CMarioState::GetAnimationId(CMario* mario)
{
    // Fallback animation
    return ID_ANI_MARIO_SMALL_IDLE;
}

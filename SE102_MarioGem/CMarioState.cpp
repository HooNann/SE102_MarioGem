#include "CMarioState.h"
#include "Mario.h"
#include "Game.h"
#include "Coin.h"
#include "Portal.h"
#include "CItem.h"
#include "Goomba.h"
#include "Blaster.h"
#include "Burner.h"
#include "CannonBall.h"
#include "CMarioDeadState.h"
#include "QuestionBlock.h"

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
            }
            return;
        }
    }

    if (dynamic_cast<CCoin*>(e->obj))
    {
        e->obj->Delete();
        CGameData::GetInstance()->AddCoin(1);
        CGameData::GetInstance()->AddScore(100);
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
                break;
            case ITEM_TYPE_LEAF:
                mario->SetLevel(MarioLevel::Raccoon);
                break;
            }
        }
    }
    // Xử lý mặc định khi chạm trúng quái vật / bẫy (Nếu các state không chặn lại)
    else if (dynamic_cast<CGoomba*>(e->obj) || 
             dynamic_cast<CBlaster*>(e->obj) || 
             dynamic_cast<CBurner*>(e->obj) || 
             dynamic_cast<CCannonBall*>(e->obj))
    {
        if (dynamic_cast<CBurner*>(e->obj)) {
            CBurner* burner = dynamic_cast<CBurner*>(e->obj);
            if (!burner || burner->GetState() != static_cast<int>(BurnerState::Firing)) return;
        }
        if (dynamic_cast<CBlaster*>(e->obj)) {
            CBlaster* blaster = dynamic_cast<CBlaster*>(e->obj);
            if (!blaster || blaster->GetState() != static_cast<int>(BlasterState::Firing)) return;
        }

        if (mario->untouchable == 0)
        {
            if (mario->GetLevel() == MarioLevel::Fire || mario->GetLevel() == MarioLevel::Raccoon)
            {
                mario->SetLevel(MarioLevel::Big);
                mario->StartUntouchable();
            }
            else if (mario->GetLevel() == MarioLevel::Big)
            {
                mario->SetLevel(MarioLevel::Small);
                mario->StartUntouchable();
            }
            else
            {
                mario->ChangeState(new CMarioDeadState());
            }
        }
    }
}

int CMarioState::GetAnimationId(CMario* mario)
{
    // Fallback animation
    return ID_ANI_MARIO_SMALL_IDLE;
}

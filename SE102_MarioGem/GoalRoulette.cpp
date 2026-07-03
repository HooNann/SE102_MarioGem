#include "GoalRoulette.h"
#include "Sprites.h"
#include "GoalRewardItem.h"
#include "PlayScene.h"
#include "Game.h"

#define SPRITE_ID_ROULETTE_MUSHROOM 71052
#define SPRITE_ID_ROULETTE_FLOWER   71055
#define SPRITE_ID_ROULETTE_STAR     71057

CGoalRoulette::CGoalRoulette(float x, float y) : CGameObject(x, y)
{
	isHit = false;
	currentItem = ItemType::Mushroom;
	lastSwitchTime = GetTickCount64();
}

void CGoalRoulette::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isHit)
	{
		// Switch items every 150ms
		if (GetTickCount64() - lastSwitchTime > 150)
		{
			lastSwitchTime = GetTickCount64();
			if (currentItem == ItemType::Mushroom) currentItem = ItemType::Flower;
			else if (currentItem == ItemType::Flower) currentItem = ItemType::Star;
			else currentItem = ItemType::Mushroom;
		}

		// Check collision manually because it's not a blocking object
		// and Mario might just pass through it.
		// Wait, it's better to let Mario's Update detect this if we want,
		// but CPlayScene doesn't check AABB for non-blocking usually?
		// Actually, Mario's OnCollisionWith is triggered by SweepAABB.
		// If it's non-blocking, SweepAABB doesn't trigger OnCollisionWith for non-blocking.
		// We must check AABB overlap.
		
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		CMario* mario = (CMario*)scene->GetPlayer();
		if (mario != NULL)
		{
			float ml, mt, mr, mb;
			mario->GetBoundingBox(ml, mt, mr, mb);
			float l, t, r, b;
			GetBoundingBox(l, t, r, b);

			// AABB overlap check
			if (!(mr < l || ml > r || mb < t || mt > b))
			{
				isHit = true;
				scene->TriggerCourseClear(static_cast<int>(currentItem));
				
				// Spawn the floating reward item
				CGoalRewardItem* reward = new CGoalRewardItem(x, y, currentItem);
				scene->AddObject(reward);
			}
		}
	}
}

void CGoalRoulette::Render()
{
	if (isHit) return; // Hide when hit, because reward item will show up

	int spriteId = SPRITE_ID_ROULETTE_MUSHROOM;
	if (currentItem == ItemType::Flower) spriteId = SPRITE_ID_ROULETTE_FLOWER;
	else if (currentItem == ItemType::Star) spriteId = SPRITE_ID_ROULETTE_STAR;

	CSprites::GetInstance()->Get(spriteId)->Draw(x, y);
}

void CGoalRoulette::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - GOAL_ROULETTE_BBOX_WIDTH / 2;
	t = y - GOAL_ROULETTE_BBOX_HEIGHT / 2;
	r = l + GOAL_ROULETTE_BBOX_WIDTH;
	b = t + GOAL_ROULETTE_BBOX_HEIGHT;
}

LPGAMEOBJECT CGoalRoulette::CreateFromJSON(const float cx, const float cy)
{
	return new CGoalRoulette(cx, cy);
}

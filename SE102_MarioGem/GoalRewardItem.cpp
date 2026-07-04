#include "GoalRewardItem.h"
#include "Sprites.h"

#define SPRITE_ID_REWARD_MUSHROOM 71052
#define SPRITE_ID_REWARD_FLOWER   71055
#define SPRITE_ID_REWARD_STAR     71057

CGoalRewardItem::CGoalRewardItem(float x, float y, ItemType t) : CGameObject(x, y)
{
	type = t;
	vy = -0.1f;
	vx = 0.0f;
	startTime = GetTickCount64();
	
	// Add to inventory
	CGameData::GetInstance()->AddItem(t);
}

void CGoalRewardItem::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	x += vx * dt;
	y += vy * dt;
	
	// Delete itself after flying for a while, or just let it fly out of screen
	if (GetTickCount64() - startTime > 3000)
	{
		isDeleted = true;
	}
}

void CGoalRewardItem::Render()
{
	int spriteId = SPRITE_ID_REWARD_MUSHROOM;
	if (type == ItemType::Flower) spriteId = SPRITE_ID_REWARD_FLOWER;
	else if (type == ItemType::Star) spriteId = SPRITE_ID_REWARD_STAR;

	CSprites::GetInstance()->Get(spriteId)->Draw(x, y);
}

void CGoalRewardItem::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = t = r = b = 0; // Not collidable
}

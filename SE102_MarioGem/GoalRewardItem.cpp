#include "GoalRewardItem.h"
#include "Animations.h"

#define ANI_ID_GOAL_MUSHROOM 91016
#define ANI_ID_GOAL_FLOWER   91017
#define ANI_ID_GOAL_STAR     91018

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
	int aniId = ANI_ID_GOAL_MUSHROOM;
	if (type == ItemType::Flower) aniId = ANI_ID_GOAL_FLOWER;
	else if (type == ItemType::Star) aniId = ANI_ID_GOAL_STAR;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
}

void CGoalRewardItem::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = t = r = b = 0; // Not collidable
}

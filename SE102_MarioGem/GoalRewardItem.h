#pragma once
#include "GameObject.h"
#include "GameData.h"

class CGoalRewardItem : public CGameObject
{
protected:
	ItemType type;
	ULONGLONG startTime;

public:
	CGoalRewardItem(float x, float y, ItemType t);
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual int IsBlocking() { return 0; }
};

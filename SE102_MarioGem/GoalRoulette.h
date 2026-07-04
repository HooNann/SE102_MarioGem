#pragma once
#include "GameObject.h"
#include "GameData.h"

#define GOAL_ROULETTE_BBOX_WIDTH  16
#define GOAL_ROULETTE_BBOX_HEIGHT 16

class CGoalRoulette : public CGameObject
{
protected:
	bool isHit;
	ItemType currentItem;
	ULONGLONG lastSwitchTime;

public:
	CGoalRoulette(float x, float y);
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual int IsBlocking() { return 0; } // Cho phép Mario đi xuyên qua
	
	virtual void OnCollisionWith(LPCOLLISIONEVENT e) {}
    
    // Tạo qua JSON
    static LPGAMEOBJECT CreateFromJSON(const float cx, const float cy);
};

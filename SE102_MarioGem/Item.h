#pragma once

#include "GameObject.h"

enum class ItemState : int
{
	Appearing = 0,
	Active = 1
};

constexpr DWORD DEFAULT_ITEM_APPEAR_DURATION = 300;
constexpr float DEFAULT_ITEM_APPEAR_DISTANCE = 16.0f;

class CItem : public CGameObject
{
protected:
	float appearStartY;
	float appearTargetY;
	DWORD appearElapsed;

	virtual DWORD GetAppearDuration();
	virtual float GetAppearDistance();
	virtual void UpdateAppearing(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void OnAppearFinished();

public:
	CItem(float x, float y);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual int IsCollidable() { return state == static_cast<int>(ItemState::Active); }
	virtual int IsBlocking() { return 0; }

	void SetState(ItemState state);
	virtual void SetState(int state) override { SetState(static_cast<ItemState>(state)); }
};

#pragma once

#include "Item.h"

constexpr int ID_ANI_ITEM_MUSHROOM = 60003;
constexpr int MUSHROOM_BBOX_WIDTH = 16;
constexpr int MUSHROOM_BBOX_HEIGHT = 14;
constexpr DWORD MUSHROOM_APPEAR_DURATION = 500;
constexpr float MUSHROOM_APPEAR_DISTANCE = 16.0f;
constexpr float MUSHROOM_GRAVITY = 0.002f;
constexpr float MUSHROOM_WALKING_SPEED = 0.05f;

class CMushroom : public CItem
{
protected:
	virtual DWORD GetAppearDuration();
	virtual float GetAppearDistance();
	virtual void OnAppearFinished();

public:
	CMushroom(float x, float y);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

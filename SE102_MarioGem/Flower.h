#pragma once

#include "GameObject.h"

constexpr int ID_ANI_ITEM_FLOWER = 91006;
constexpr int FLOWER_BBOX_WIDTH = 16;
constexpr int FLOWER_BBOX_HEIGHT = 16;

class CFlower : public CGameObject
{
public:
	CFlower(float x, float y);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {}
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
};

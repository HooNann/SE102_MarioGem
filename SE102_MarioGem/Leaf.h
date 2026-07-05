#pragma once

#include "GameObject.h"

constexpr int ID_ANI_ITEM_LEAF = 91007;
constexpr int LEAF_BBOX_WIDTH = 16;
constexpr int LEAF_BBOX_HEIGHT = 16;

class CLeaf : public CGameObject
{
public:
	CLeaf(float x, float y);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {}
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
};

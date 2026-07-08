#pragma once

#include "Item.h"

constexpr int ID_ANI_ITEM_LEAF = 91007;
constexpr int LEAF_BBOX_WIDTH = 16;
constexpr int LEAF_BBOX_HEIGHT = 14;
constexpr DWORD LEAF_APPEAR_DURATION = 750;

class CLeaf : public CItem
{
protected:
	virtual DWORD GetAppearDuration();
	virtual float GetAppearDistance();

public:
	CLeaf(float x, float y);

	void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

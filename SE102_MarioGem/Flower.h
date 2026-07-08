#pragma once

#include "Item.h"

constexpr int ID_ANI_ITEM_FLOWER = 91006;
constexpr int FLOWER_BBOX_WIDTH = 16;
constexpr int FLOWER_BBOX_HEIGHT = 16;
constexpr DWORD FLOWER_APPEAR_DURATION = 750;
constexpr float FLOWER_APPEAR_DISTANCE = 16.0f;

class CFlower : public CItem
{
protected:
	virtual DWORD GetAppearDuration();
	virtual float GetAppearDistance();

public:
	CFlower(float x, float y);

	void Render();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

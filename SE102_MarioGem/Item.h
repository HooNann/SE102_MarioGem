#pragma once
#include "GameObject.h"

// Định nghĩa mã loại Item (Số do bạn tự quy ước)
constexpr int ITEM_TYPE_FLOWER = 1;
constexpr int ITEM_TYPE_LEAF = 2;

// Định nghĩa mã ID Animation tương ứng trong file text
constexpr int ID_ANI_ITEM_FLOWER = 60001;
constexpr int ID_ANI_ITEM_LEAF = 60002;

class CItem : public CGameObject
{
protected:
	int itemType; // Biến cốt lõi để phân biệt Hoa hay Lá

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}
	virtual void Render();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; } 

public:
	CItem(float x, float y, int type);
	int GetItemType() { return itemType; }
};
#pragma once
#include "GameObject.h"

// Lớp cơ sở cho mọi vật thể trên bản đồ thế giới (World Map).
// Khác với GameObject thông thường, MapObject không có va chạm vật lý và trọng lực.
class CMapObject : public CGameObject
{
protected:
	int aniId;

public:
	CMapObject(float x, float y, int aniId = -1);
	
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	
	// Các object trên map mặc định không va chạm kiểu SweptAABB
	virtual int IsCollidable() { return 0; }
	virtual int IsBlocking() { return 0; }
};

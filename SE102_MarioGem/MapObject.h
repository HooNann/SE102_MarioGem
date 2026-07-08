#pragma once
#include "GameObject.h"

class CMapObject : public CGameObject
{
protected:
	int aniId;

public:
	CMapObject(float x, float y, int aniId = -1);
	
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	
	virtual int IsCollidable() { return 0; }
	virtual int IsBlocking() { return 0; }
};

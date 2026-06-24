#pragma once

#include "GameObject.h"

class CCollisionBox : public CGameObject
{
	float width;
	float height;

public:
	CCollisionBox(float x, float y, float width, float height) :CGameObject(x, y)
	{
		this->width = width;
		this->height = height;
	}

	void Render() {}
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	int IsBlocking() { return 1; }
};

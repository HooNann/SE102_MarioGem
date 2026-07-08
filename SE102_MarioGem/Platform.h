#pragma once

#include "GameObject.h"

class CPlatform : public CGameObject
{
	float width;
	float height;

public:
	CPlatform(float x, float y, float width, float height) : CGameObject(x, y)
	{
		this->width = width;
		this->height = height;
	}

	void Render() {}
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	
	int IsDirectionColliable(float nx, float ny);
	int IsBlocking() { return 1; }
};

typedef CPlatform* LPPLATFORM;
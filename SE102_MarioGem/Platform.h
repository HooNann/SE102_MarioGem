#pragma once

#include "GameObject.h"

// Platform giờ đây là một khối tàng hình chỉ lo xử lý vật lý (nhảy xuyên từ dưới lên)
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

	void Render() {} // Tàng hình
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	
	// Cốt lõi của Platform: Chỉ cản Mario khi rơi từ trên xuống
	int IsDirectionColliable(float nx, float ny);
	int IsBlocking() { return 1; }
};

typedef CPlatform* LPPLATFORM;
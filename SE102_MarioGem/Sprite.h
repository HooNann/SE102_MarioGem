#pragma once

#include "Texture.h"

class CSprite
{
	int id;				// Sprite ID in the sprite database

	int left;
	int top;
	int right;
	int bottom;

	LPTEXTURE texture;
	D3DX10_SPRITE sprite;
	D3DXMATRIX matScaling;
public:
	CSprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex);

	void Draw(float x, float y, int nx = 1, int ny = 1);
	
	int GetWidth() { return right - left + 1; }
	int GetHeight() { return bottom - top + 1; }
};

typedef CSprite* LPSPRITE;
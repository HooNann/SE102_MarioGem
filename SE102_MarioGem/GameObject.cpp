#include <d3dx9.h>
#include <algorithm>


#include "debug.h"
#include "Textures.h"
#include "Camera.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

CGameObject::CGameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;	
	state = -1;
	isDeleted = false;
}

void CGameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l,t,r,b; 

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy; 
	CCamera::GetInstance()->GetCamPos(cx, cy);

	float drawX = (float)floor(x) - (float)floor(cx);
	float drawY = (float)floor(y) - (float)floor(cy);
	CGame::GetInstance()->Draw(drawX, drawY, bbox, &rect, BBOX_ALPHA, rect.right, rect.bottom);
}

CGameObject::~CGameObject()
{

}
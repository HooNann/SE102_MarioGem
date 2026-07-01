#include "Portal.h"
#include "Game.h"
#include "Camera.h"
#include "Textures.h"

CPortal::CPortal(float l, float t, float r, float b, int scene_id )
{
	this->scene_id = scene_id;
	width = r - l;
	height = b - t;
	
	// Game engine uses x, y as center for bounding box calculation
	x = l + width / 2.0f;
	y = t + height / 2.0f;
}

void CPortal::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy;
	CCamera::GetInstance()->GetCamPos(cx, cy);

	CGame::GetInstance()->Draw(x - cx, y - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CPortal::Render()
{
	RenderBoundingBox();
}

void CPortal::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x - width/2;
	t = y - height/2;
	r = x + width/2;
	b = y + height/2;
}

LPGAMEOBJECT CPortal::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	float r = (float)atof(tokens[3].c_str());
	float b = (float)atof(tokens[4].c_str());

	int scene_id = atoi(tokens[5].c_str());

	return new CPortal(
		x,
		y,
		r,
		b,
		scene_id
	);
}
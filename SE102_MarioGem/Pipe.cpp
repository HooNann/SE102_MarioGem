#include "Pipe.h"
#include "Camera.h"
#include "Game.h"
#include "Textures.h"

CPipe::CPipe(float l, float t, float r, float b, float targetX, float targetY,
	PipeDirection entryDirection, PipeDirection exitDirection)
{
	this->targetX = targetX;
	this->targetY = targetY;
	this->entryDirection = entryDirection;
	this->exitDirection = exitDirection;

	width = r - l;
	height = b - t;

	x = l + width / 2.0f;
	y = t + height / 2.0f;
}

void CPipe::RenderBoundingBox()
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

void CPipe::Render()
{
	//RenderBoundingBox();
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - width / 2;
	t = y - height / 2;
	r = x + width / 2;
	b = y + height / 2;
}

PipeDirection CPipe::ParseDirection(const string& direction)
{
	if (direction == "up") return PipeDirection::Up;
	if (direction == "left") return PipeDirection::Left;
	if (direction == "right") return PipeDirection::Right;
	return PipeDirection::Down;
}

LPGAMEOBJECT CPipe::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());
	float r = (float)atof(tokens[3].c_str());
	float b = (float)atof(tokens[4].c_str());
	float targetX = (float)atof(tokens[5].c_str());
	float targetY = (float)atof(tokens[6].c_str());

	PipeDirection entryDirection = PipeDirection::Down;
	PipeDirection exitDirection = PipeDirection::Up;

	if (tokens.size() > 7) entryDirection = ParseDirection(tokens[7]);
	if (tokens.size() > 8) exitDirection = ParseDirection(tokens[8]);

	return new CPipe(x, y, r, b, targetX, targetY, entryDirection, exitDirection);
}

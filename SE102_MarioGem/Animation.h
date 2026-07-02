#pragma once

#include <Windows.h>
#include <unordered_map>

#include "Sprites.h"
#include "AnimationFrame.h"

using namespace std;

class CAnimation
{
	ULONGLONG lastFrameTime;
	int defaultTime;
	int currentFrame;
	vector<LPANIMATION_FRAME> frames;
public:
	CAnimation(int defaultTime = 100) { this->defaultTime = defaultTime; lastFrameTime = -1; currentFrame = -1; }
	void Add(int spriteId, DWORD time = 0);
	void Render(float x, float y, int nx = 1, int ny = 1, float timeScale = 1.0f);

	int GetSpriteWidth() { return frames.size() > 0 ? frames[0]->GetSprite()->GetWidth() : 0; }
	int GetSpriteHeight() { return frames.size() > 0 ? frames[0]->GetSprite()->GetHeight() : 0; }
};

typedef CAnimation* LPANIMATION;
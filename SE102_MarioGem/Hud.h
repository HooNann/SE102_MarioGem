#pragma once
#include "Texture.h"

class CMario;

constexpr int ID_TEX_UI = 30;
constexpr int ID_TEX_BLACK = 31;

class CHud
{
	LPTEXTURE tex;
	LPTEXTURE texBlack;

	void DrawRegion(float centerX, float centerY, int srcLeft, int srcTop, int srcW, int srcH);
	bool DrawGlyph(char c, float screenX, float screenY);
	void DrawString(const char* s, float x, float y);
	void DrawNumber(int value, float rightX, float y, int minDigits);
	void DrawPMeter(float x, float y);
	void DrawPanel(float& fx, float& fy);

public:
	CHud();
	void Render(CMario* mario, int timeLeft, const char* world);
	void RenderWorldMap(const char* world);
	void RenderCourseClear(int rewardCard);
};

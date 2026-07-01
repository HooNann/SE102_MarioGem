#include "Hud.h"
#include "Game.h"
#include "Textures.h"
#include "Mario.h"
#include "GameData.h"

#include <string>
#include <cmath>

static const int COL_X[12] = { 32, 53, 71, 90, 108, 127, 145, 162, 179, 197, 215, 234 };
static const int ROW_Y[3] = { 24, 39, 55 };
constexpr int GLYPH_W = 8;
constexpr int GLYPH_H = 8;
constexpr int DIGIT_PITCH = 8;

constexpr int FRAME_SRC_L = 12;
constexpr int FRAME_SRC_T = 128;
constexpr int FRAME_W = 152;
constexpr int FRAME_H = 28;

constexpr int DASH_SRC_L = 115;
constexpr int DASH_SRC_T = 128;
constexpr int DASH_W = 6;
constexpr int DASH_H = 2;

constexpr int ROW_TOP = 2;
constexpr int ROW_BOT = 15;

constexpr int WORLD_X = 40;
constexpr int COINS_RIGHT = 133;
constexpr int SCORE_RIGHT = 112;
constexpr int LIVES_RIGHT = 42;
constexpr int TIME_RIGHT = 126;
constexpr int PMETER_X = 50;

constexpr int PMETER_SEGMENTS = 6;
constexpr int PMETER_SEG_PITCH = 5;

static bool GlyphCell(char c, int& left, int& top)
{
	int row, col;
	if (c >= '0' && c <= '9') { row = 0; col = c - '0'; }
	else if (c == 'A' || c == 'B') { row = 0; col = 10 + (c - 'A'); }
	else if (c >= 'C' && c <= 'N') { row = 1; col = c - 'C'; }
	else if (c >= 'O' && c <= 'Z') { row = 2; col = c - 'O'; }
	else return false;
	left = COL_X[col];
	top = ROW_Y[row];
	return true;
}

CHud::CHud()
{
	tex = CTextures::GetInstance()->Get(ID_TEX_UI);
}

void CHud::DrawRegion(float centerX, float centerY, int srcLeft, int srcTop, int srcW, int srcH)
{
	if (tex == NULL) return;
	CGame::GetInstance()->Draw(centerX, centerY, tex,
		srcLeft, srcTop, srcLeft + srcW - 1, srcTop + srcH - 1);
}

bool CHud::DrawGlyph(char c, float screenX, float screenY)
{
	if (c == '-')
	{
		DrawRegion(screenX + DASH_W / 2.0f, screenY + GLYPH_H / 2.0f,
			DASH_SRC_L, DASH_SRC_T, DASH_W, DASH_H);
		return true;
	}

	int l, t;
	if (!GlyphCell(c, l, t)) return false;
	DrawRegion(screenX + GLYPH_W / 2.0f, screenY + GLYPH_H / 2.0f, l, t, GLYPH_W, GLYPH_H);
	return true;
}

void CHud::DrawString(const char* s, float x, float y)
{
	for (const char* p = s; *p; p++)
	{
		DrawGlyph(*p, x, y);
		x += DIGIT_PITCH;
	}
}

void CHud::DrawNumber(int value, float rightX, float y, int minDigits)
{
	if (value < 0) value = 0;
	std::string s = std::to_string(value);
	while ((int)s.size() < minDigits) s = "0" + s;

	float left = rightX - (float)s.size() * DIGIT_PITCH;
	DrawString(s.c_str(), left, y);
}

void CHud::DrawPMeter(int pmeter, float x, float y)
{
	DrawGlyph('P', x, y);
	float barX = x + GLYPH_W + 2;

	int filled = (int)std::lround((double)pmeter / MARIO_PMETER_MAX * PMETER_SEGMENTS);
	if (filled > PMETER_SEGMENTS) filled = PMETER_SEGMENTS;
	for (int i = 0; i < filled; i++)
		DrawRegion(barX + i * PMETER_SEG_PITCH + DASH_W / 2.0f, y + GLYPH_H / 2.0f,
			DASH_SRC_L, DASH_SRC_T, DASH_W, DASH_H);
}

void CHud::Render(CMario* mario, int timeLeft, const char* world)
{
	if (tex == NULL || mario == NULL) return;

	CGame* game = CGame::GetInstance();
	int W = game->GetBackBufferWidth();
	int H = game->GetBackBufferHeight();

	float fx = (W - FRAME_W) / 2.0f;
	float fy = (float)(H - FRAME_H - 6);

	DrawRegion(fx + FRAME_W / 2.0f, fy + FRAME_H / 2.0f, FRAME_SRC_L, FRAME_SRC_T, FRAME_W, FRAME_H);

	float topY = fy + ROW_TOP;
	float botY = fy + ROW_BOT;

	DrawString(world ? world : "1-1", fx + WORLD_X, topY);
	DrawNumber(CGameData::GetInstance()->GetCoin(), fx + COINS_RIGHT, topY, 2);
	DrawNumber(CGameData::GetInstance()->GetScore(), fx + SCORE_RIGHT, topY, 6);
	DrawNumber(CGameData::GetInstance()->GetLives(), fx + LIVES_RIGHT, botY, 1);
	DrawNumber(timeLeft, fx + TIME_RIGHT, botY, 3);
	DrawPMeter(mario->GetPMeter(), fx + PMETER_X, botY);
}

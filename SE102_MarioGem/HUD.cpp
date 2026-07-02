#include "Hud.h"
#include "Game.h"
#include "Textures.h"
#include "Mario.h"
#include "GameData.h"

#include <string>

static const int COL_X[12] = { 32, 53, 71, 90, 108, 127, 145, 162, 179, 197, 215, 234 };
static const int ROW_Y[3] = { 24, 39, 55 };
constexpr int GLYPH_W = 8;
constexpr int GLYPH_H = 8;
constexpr int DIGIT_PITCH = 8;

constexpr int FRAME_SRC_L = 11;
constexpr int FRAME_SRC_T = 127;
constexpr int FRAME_W = 232;
constexpr int FRAME_H = 30;

constexpr int BAND_H = 34;

constexpr int DASH_SRC_L = 92;
constexpr int DASH_SRC_T = 165;
constexpr int DASH_W = 4;
constexpr int DASH_H = 2;

constexpr int ROW_TOP = 8;
constexpr int ROW_BOT = 16;

constexpr int WORLD_X     = 40;
constexpr int PMETER_X    = 52;
constexpr int COINS_RIGHT = 150;
constexpr int LIVES_RIGHT = 40;
constexpr int SCORE_RIGHT = 104;
constexpr int TIME_RIGHT  = 150;

constexpr int PMETER_SEGMENTS  = 6;
constexpr int PM_SRC_T         = 162;
constexpr int PM_H             = 8;
constexpr int PM_ARROW_W       = 8;
constexpr int PM_ARROW_PITCH   = 6;
constexpr int PM_ARROW_UNLIT_L = 64;
constexpr int PM_P_W           = 18;
constexpr int PM_P_UNLIT_L     = 72;

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
	texBlack = CTextures::GetInstance()->Get(ID_TEX_BLACK);
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

void CHud::DrawPMeter(float x, float y)
{
	for (int i = 0; i < PMETER_SEGMENTS; i++)
	{
		DrawRegion(x + i * PM_ARROW_PITCH + PM_ARROW_W / 2.0f, y + PM_H / 2.0f,
			PM_ARROW_UNLIT_L, PM_SRC_T, PM_ARROW_W, PM_H);
	}

	float px = x + PMETER_SEGMENTS * PM_ARROW_PITCH + 1;
	DrawRegion(px + PM_P_W / 2.0f, y + PM_H / 2.0f, PM_P_UNLIT_L, PM_SRC_T, PM_P_W, PM_H);
}

void CHud::Render(CMario* mario, int timeLeft, const char* world)
{
	if (tex == NULL || mario == NULL) return;

	CGame* game = CGame::GetInstance();
	int W = game->GetBackBufferWidth();
	int H = game->GetBackBufferHeight();

	if (texBlack != NULL)
		game->Draw(W / 2.0f, H - BAND_H / 2.0f, texBlack, (RECT*)NULL, 1.0f, W, BAND_H);

	float fx = (float)((W - FRAME_W) / 2);
	float fy = (float)(H - BAND_H + (BAND_H - FRAME_H) / 2);

	DrawRegion(fx + FRAME_W / 2.0f, fy + FRAME_H / 2.0f, FRAME_SRC_L, FRAME_SRC_T, FRAME_W, FRAME_H);

	float topY = fy + ROW_TOP;
	float botY = fy + ROW_BOT;

	DrawString(world ? world : "1", fx + WORLD_X, topY);
	DrawPMeter(fx + PMETER_X, topY);
	DrawNumber(CGameData::GetInstance()->GetCoin(), fx + COINS_RIGHT, topY, 2);

	DrawNumber(CGameData::GetInstance()->GetLives(), fx + LIVES_RIGHT, botY, 1);
	DrawNumber(CGameData::GetInstance()->GetScore(), fx + SCORE_RIGHT, botY, 6);
	DrawNumber(timeLeft, fx + TIME_RIGHT, botY, 3);
}

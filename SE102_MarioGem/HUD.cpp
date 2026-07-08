#include "Hud.h"
#include "Game.h"
#include "Textures.h"
#include "Mario.h"
#include "GameData.h"
#include "Camera.h"
#include "Sprites.h"

#include <string>
#include <vector>

static const int COL_X[12] = { 32, 53, 71, 90, 108, 127, 145, 162, 179, 197, 215, 234 };
static const int ROW_Y[3] = { 24, 39, 55 };
constexpr int GLYPH_W = 8;
constexpr int GLYPH_H = 8;
constexpr int DIGIT_PITCH = 8;

constexpr int FRAME_SRC_L = 11;
constexpr int FRAME_SRC_T = 127;
constexpr int FRAME_W = 232;
constexpr int FRAME_H = 30;

constexpr int BAND_H = 32;

constexpr int DASH_SRC_L = 92;
constexpr int DASH_SRC_T = 165;
constexpr int DASH_W = 4;
constexpr int DASH_H = 2;

constexpr int ROW_TOP = 8;
constexpr int ROW_BOT = 16;

constexpr int WORLD_X     = 40;
constexpr int PMETER_X    = 54;
constexpr int COINS_RIGHT = 150;
constexpr int LIVES_RIGHT = 40;
constexpr int SCORE_RIGHT = 104;
constexpr int TIME_RIGHT  = 150;

constexpr int PMETER_SEGMENTS  = 6;
constexpr int PM_H             = 8;
constexpr int PM_ARROW_W       = 8;
constexpr int PM_ARROW_PITCH   = 6;
constexpr int PM_P_W           = 18;
constexpr int SPRITE_ID_PM_ARROW_UNLIT = 200057;
constexpr int SPRITE_ID_PM_ARROW_LIT = 200056;
constexpr int SPRITE_ID_PM_P_UNLIT = 200049;
constexpr int SPRITE_ID_PM_P_LIT = 200048;

constexpr int CARD_SLOT_X[3] = { 173, 197, 221 };
constexpr int CARD_SLOT_Y = 15;

constexpr int SPRITE_ID_REWARD_MUSHROOM = 200051;
constexpr int SPRITE_ID_REWARD_FLOWER = 200052;
constexpr int SPRITE_ID_REWARD_STAR = 200053;

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

static int GetRewardSpriteId(ItemType item)
{
	if (item == ItemType::Flower) return SPRITE_ID_REWARD_FLOWER;
	if (item == ItemType::Star) return SPRITE_ID_REWARD_STAR;
	if (item == ItemType::Mushroom) return SPRITE_ID_REWARD_MUSHROOM;
	return -1;
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

void CHud::DrawPMeter(float x, float y, int pMeter)
{
	float cx, cy;
	CCamera::GetInstance()->GetCamPos(cx, cy);

	int litSegments = 0;
	if (pMeter > 0)
	{
		litSegments = (pMeter * PMETER_SEGMENTS + MARIO_PMETER_MAX - 1) / MARIO_PMETER_MAX;
		if (litSegments > PMETER_SEGMENTS) litSegments = PMETER_SEGMENTS;
	}

	for (int i = 0; i < PMETER_SEGMENTS; i++)
	{
		int spriteId = (i < litSegments) ? SPRITE_ID_PM_ARROW_LIT : SPRITE_ID_PM_ARROW_UNLIT;
		LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
		if (sprite != NULL)
			sprite->Draw(cx + x + i * PM_ARROW_PITCH + PM_ARROW_W / 2.0f, cy + y + PM_H / 2.0f);
	}

	float px = x + PMETER_SEGMENTS * PM_ARROW_PITCH + 1;
	int pSpriteId = (pMeter >= MARIO_PMETER_MAX) ? SPRITE_ID_PM_P_LIT : SPRITE_ID_PM_P_UNLIT;
	LPSPRITE pSprite = CSprites::GetInstance()->Get(pSpriteId);
	if (pSprite != NULL)
		pSprite->Draw(cx + px + PM_P_W / 2.0f, cy + y + PM_H / 2.0f);
}

void CHud::DrawPanel(float& fx, float& fy)
{
	CGame* game = CGame::GetInstance();
	int W = game->GetBackBufferWidth();
	int H = game->GetBackBufferHeight();

	if (texBlack != NULL)
		game->Draw(W / 2.0f, H - BAND_H / 2.0f, texBlack, (RECT*)NULL, 1.0f, W, BAND_H);

	fx = (float)((W - FRAME_W) / 2);
	fy = (float)(H - BAND_H + (BAND_H - FRAME_H) / 2);

	DrawRegion(fx + FRAME_W / 2.0f, fy + FRAME_H / 2.0f, FRAME_SRC_L, FRAME_SRC_T, FRAME_W, FRAME_H);
}

void CHud::DrawInventory(float fx, float fy)
{
	std::vector<ItemType> inventory = CGameData::GetInstance()->GetInventory();
	int count = (inventory.size() < 3) ? (int)inventory.size() : 3;

	float cx, cy;
	CCamera::GetInstance()->GetCamPos(cx, cy);

	for (int i = 0; i < count; i++)
	{
		int spriteId = GetRewardSpriteId(inventory[i]);
		if (spriteId < 0) continue;

		LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
		if (sprite != NULL)
			sprite->Draw(cx + fx + CARD_SLOT_X[i], cy + fy + CARD_SLOT_Y);
	}
}

void CHud::Render(CMario* mario, int timeLeft, const char* world)
{
	if (tex == NULL || mario == NULL) return;

	float fx, fy;
	DrawPanel(fx, fy);

	float topY = fy + ROW_TOP;
	float botY = fy + ROW_BOT;

	DrawString(world ? world : "1", fx + WORLD_X, topY);
	DrawPMeter(fx + PMETER_X, topY, mario->GetPMeter());
	DrawNumber(CGameData::GetInstance()->GetCoin(), fx + COINS_RIGHT, topY, 2);

	DrawNumber(CGameData::GetInstance()->GetLives(), fx + LIVES_RIGHT, botY, 1);
	DrawNumber(CGameData::GetInstance()->GetScore(), fx + SCORE_RIGHT, botY, 6);
	DrawNumber(timeLeft, fx + TIME_RIGHT, botY, 3);

	DrawInventory(fx, fy);
}

void CHud::RenderWorldMap(const char* world)
{
	if (tex == NULL) return;

	float fx, fy;
	DrawPanel(fx, fy);

	float topY = fy + ROW_TOP;
	float botY = fy + ROW_BOT;

	DrawString(world ? world : "1", fx + WORLD_X, topY);
	DrawPMeter(fx + PMETER_X, topY, 0);
	DrawNumber(CGameData::GetInstance()->GetCoin(), fx + COINS_RIGHT, topY, 2);

	DrawNumber(CGameData::GetInstance()->GetLives(), fx + LIVES_RIGHT, botY, 1);
	DrawNumber(CGameData::GetInstance()->GetScore(), fx + SCORE_RIGHT, botY, 6);

	DrawInventory(fx, fy);
}
void CHud::RenderCourseClear(int rewardCard)
{
	CCamera* camera = CCamera::GetInstance();
	float screenW = (float)camera->GetWidth();
	float screenH = (float)camera->GetHeight();

	DrawString("COURSE CLEAR", screenW / 2 - 48, screenH / 2 - 30);
	DrawString("YOU GOT A CARD", screenW / 2 - 56, screenH / 2);
}

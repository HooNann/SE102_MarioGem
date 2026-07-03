#include <fstream>
#include <cstdlib>
#include <cmath>

#include "IntroScene.h"
#include "IntroKeyHandler.h"
#include "IntroDefs.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"
#include "Camera.h"
#include "Game.h"
#include "SoundManager.h"
#include "debug.h"

using namespace std;

#define INTRO_SECTION_UNKNOWN -1
#define INTRO_SECTION_ASSETS 1

#define INTRO_ASSETS_SECTION_UNKNOWN -1
#define INTRO_ASSETS_SECTION_SPRITES 1
#define INTRO_ASSETS_SECTION_ANIMATIONS 2

#define INTRO_MAX_LINE 1024

static const float BG_COLORS[4][3] = {
	{ 91.0f, 33.0f, 0.0f },
	{ 160.0f, 81.0f, 0.0f },
	{ 242.0f, 158.0f, 27.0f },
	{ 250.0f, 217.0f, 163.0f }
};

CIntroScene::CIntroScene(int id, LPCWSTR filePath) : CScene(id, filePath)
{
	timer = 0;
	menuActive = false;
	menuRow = 0;
	key_handler = new CIntroKeyHandler(this);
}

void CIntroScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return;

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CIntroScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i + 1 < (int)tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CIntroScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	LoadAssets(path.c_str());
}

void CIntroScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading intro assets from : %s \n", assetFile);

	ifstream f;
	f.open(assetFile);

	int section = INTRO_ASSETS_SECTION_UNKNOWN;

	char str[INTRO_MAX_LINE];
	while (f.getline(str, INTRO_MAX_LINE))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;

		if (line == "[SPRITES]") { section = INTRO_ASSETS_SECTION_SPRITES; continue; }
		if (line == "[ANIMATIONS]") { section = INTRO_ASSETS_SECTION_ANIMATIONS; continue; }
		if (line[0] == '[') { section = INTRO_ASSETS_SECTION_UNKNOWN; continue; }

		switch (section)
		{
		case INTRO_ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case INTRO_ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}

	f.close();
}

void CIntroScene::ResetActors()
{
	mario = { MARIO_SPAWN_X, GROUND_Y, 0, 0, INTRO_GRAVITY, -1, 1, false, true, ANI_BIG_IDLE };
	luigi = { LUIGI_SPAWN_X, GROUND_Y, 0, 0, INTRO_GRAVITY, 1, 1, false, true, ANI_LUIGI_IDLE };
	goomba = { GOOMBA_SPAWN_X, ITEM_SPAWN_FEET, 0, 0, INTRO_GRAVITY, -1, 1, false, false, ANI_GOOMBA_WALK };
	koopaWalker = { 0, ITEM_SPAWN_FEET, 0, 0, INTRO_GRAVITY, -1, 1, false, false, ANI_GREEN_SHELL_IDLE };
	redShell = { RED_SHELL_SPAWN_X, ITEM_SPAWN_FEET, 0, 0, INTRO_GRAVITY, 1, 1, false, false, ANI_RED_SHELL_IDLE };
	luigiShell = { 0, GROUND_Y, 0, 0, INTRO_GRAVITY, -1, 1, false, true, ANI_GREEN_SHELL_IDLE };
	mushRed = { MUSH_RED_SPAWN_X, ITEM_SPAWN_FEET, 0, 0, INTRO_GRAVITY_MUSHROOM, -1, 1, false, false, ANI_MUSH_RED };
	mushGreen = { MUSH_GREEN_SPAWN_X, ITEM_SPAWN_FEET, 0, 0, INTRO_GRAVITY_MUSHROOM, 1, 1, false, false, ANI_MUSH_GREEN };
	leaf = { LEAF_SPAWN_X, LEAF_SPAWN_Y, 0, 0, 0, 1, 1, false, false, ANI_LEAF };

	for (int i = 0; i < 3; i++)
		lateShells[i] = { OFFSCREEN_L, GROUND_Y, 0, 0, INTRO_GRAVITY, 1, 1, false, true, ANI_GREEN_SHELL_SPIN };
}

void CIntroScene::Load()
{
	DebugOut(L"[INFO] Start loading intro scene from : %s \n", sceneFilePath);

	timer = 0;
	menuActive = false;
	menuRow = 0;

	curtainY = 0;
	logoStep = 0;
	logoTimer = 0;
	badgeBlinkTimer = 0;
	titleY = TITLE_CY_START;
	badgeY = BADGE_CY_START;
	bgColorIndex = 0;
	treesVisible = false;
	bottomCurtainVisible = false;

	marioStep = 0;
	luigiStep = 0;
	itemStep = 0;
	marioPoseTimer = 0;
	luigiPoseTimer = 0;
	marioHolding = false;
	luigiHolding = false;
	marioBehindTrees = false;
	leafTaken = false;
	luigiShellKicked = false;
	goombaDead = false;
	koopaTucked = false;

	lateShellTimer = 0;
	lateShellStep = 0;

	ResetActors();

	CCamera::GetInstance()->SetCamPos(0.0f, 0.0f);
	CCamera::GetInstance()->SetCameraBounds(0, 0, 0, 0);

	ifstream f;
	f.open(sceneFilePath);

	int section = INTRO_SECTION_UNKNOWN;

	char str[INTRO_MAX_LINE];
	while (f.getline(str, INTRO_MAX_LINE))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;

		if (line == "[ASSETS]") { section = INTRO_SECTION_ASSETS; continue; }
		if (line[0] == '[') { section = INTRO_SECTION_UNKNOWN; continue; }

		switch (section)
		{
		case INTRO_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
		}
	}

	f.close();

	CSoundManager::GetInstance()->PlayMusic(INTRO_MUSIC_ID);

	DebugOut(L"[INFO] Done loading intro scene\n");
}

void CIntroScene::ApplyPhysics(CIntroActor& a, DWORD dt)
{
	if (!a.visible) return;

	a.x += a.vx * dt;
	a.vy += a.g * dt;
	a.y += a.vy * dt;

	if (a.ny > 0 && a.vy >= 0 && a.y >= GROUND_Y)
	{
		a.y = GROUND_Y;
		a.vy = 0;
		a.onGround = true;
	}
	else
	{
		a.onGround = false;
	}
}

void CIntroScene::UpdateStage(DWORD dt)
{
	if (logoStep >= 2) badgeBlinkTimer = (badgeBlinkTimer + dt) % BADGE_BLINK_PERIOD;

	if (curtainY > CURTAIN_DONE_Y)
	{
		curtainY -= CURTAIN_SPEED * dt;

		if (curtainY < CURTAIN_SPAWN_Y)
		{
			mario.visible = true;
			luigi.visible = true;
		}
	}

	switch (logoStep)
	{
	case 0:
		if (timer > T_LOGO) logoStep = 1;
		break;
	case 1:
		titleY += LOGO_SPEED * dt;
		badgeY += LOGO_SPEED * dt;
		if (titleY >= TITLE_CY_FINAL)
		{
			titleY = TITLE_CY_FINAL;
			badgeY = BADGE_CY_FINAL;
			logoTimer = 0;
			logoStep = 2;
		}
		break;
	case 2:
		logoTimer += dt;
		if (logoTimer > LOGO_SHAKE_MS)
		{
			logoTimer = 0;
			logoStep = 3;
		}
		break;
	case 3:
		logoTimer += dt;
		if (logoTimer > T_COLOR_4)
		{
			bgColorIndex = 4;
			treesVisible = true;
			bottomCurtainVisible = true;
			logoStep = 4;
		}
		else if (logoTimer > T_COLOR_3) bgColorIndex = 3;
		else if (logoTimer > T_COLOR_2) bgColorIndex = 2;
		else if (logoTimer > T_COLOR_1) bgColorIndex = 1;
		break;
	}
}

void CIntroScene::UpdateMario(DWORD dt)
{
	switch (marioStep)
	{
	case 0:
		mario.aniId = ANI_BIG_WALK;
		mario.nx = -1;
		mario.vx = -INTRO_WALK_SPEED;
		marioStep = 1;
		break;
	case 1:
		if (mario.x < MARIO_CROUCH_X)
		{
			mario.vx = 0;
			mario.aniId = ANI_BIG_CROUCH;
			marioStep = 2;
		}
		break;
	case 2:
		if (timer > T_MARIO_STAND)
		{
			mario.aniId = ANI_BIG_IDLE;
			marioStep = 3;
		}
		break;
	case 3:
		if (timer > T_MARIO_CHARGE) marioStep = 4;
		break;
	case 4:
		if (leaf.visible && leaf.y > MARIO_CATCH_TRIGGER_Y)
		{
			mario.vy = -INTRO_JUMP_V;
			mario.onGround = false;
			mario.aniId = ANI_BIG_JUMP;
			CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_JUMP);
			marioStep = 5;
		}
		else if (leafTaken)
		{
			mario.aniId = ANI_RAC_IDLE;
			marioStep = 6;
		}
		break;
	case 5:
		if (leafTaken && mario.aniId != ANI_RAC_FLY)
		{
			mario.aniId = ANI_RAC_FLY;
			mario.g = 0;
			mario.vy = INTRO_FLOAT_V;
			mario.vx = -INTRO_FLOAT_DRIFT;
			mario.nx = -1;
			CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_POWERUP);
		}
		if (mario.onGround)
		{
			mario.g = INTRO_GRAVITY;
			if (leafTaken) marioStep = 6;
			else marioStep = 4;
		}
		break;
	case 6:
		if (mario.onGround)
		{
			mario.g = INTRO_GRAVITY;
			mario.vx = INTRO_WALK_SPEED;
			mario.nx = 1;
			mario.aniId = ANI_RAC_WALK;
			marioStep = 7;
		}
		break;
	case 7:
		if (mario.x > MARIO_STOP_X)
		{
			mario.vx = 0;
			mario.aniId = ANI_RAC_IDLE;
			marioStep = 8;
		}
		break;
	case 8:
		if (luigi.visible && luigi.x < LUIGI_NEAR_X && luigiStep >= 5)
		{
			mario.nx = -1;
			mario.vx = -INTRO_WALK_SPEED;
			mario.aniId = ANI_RAC_WALK;
			marioStep = 9;
		}
		break;
	case 9:
		if (mario.vx < 0 && mario.x < MARIO_WAIT_X)
		{
			mario.vx = 0;
			mario.nx = 1;
			mario.aniId = ANI_RAC_IDLE;
		}
		if (luigiShellKicked && luigiShell.visible && luigiShell.x < mario.x + MARIO_SHELL_GAP)
		{
			mario.vx = 0;
			mario.nx = 1;
			mario.vy = -INTRO_JUMP_V;
			mario.onGround = false;
			mario.aniId = ANI_RAC_JUMP;
			CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_JUMP);
			marioStep = 10;
		}
		break;
	case 10:
		if (mario.onGround)
		{
			mario.vx = INTRO_WALK_SPEED;
			mario.nx = 1;
			mario.aniId = ANI_RAC_WALK;
			marioStep = 11;
		}
		break;
	case 11:
		if (mario.x + CARRY_OFFSET_X >= redShell.x)
		{
			marioHolding = true;
			mario.aniId = ANI_RAC_HOLD;
			marioStep = 12;
		}
		break;
	case 12:
		if (mario.x > MARIO_KICK_X)
		{
			marioHolding = false;
			redShell.x = mario.x + CARRY_OFFSET_X + 4;
			redShell.y = GROUND_Y;
			redShell.vx = INTRO_SHELL_SPEED;
			redShell.aniId = ANI_RED_SHELL_SPIN;
			mario.vx = 0;
			mario.aniId = ANI_RAC_KICK;
			CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_KICK);
			marioPoseTimer = 0;
			marioStep = 13;
		}
		break;
	case 13:
		marioPoseTimer += dt;
		if (marioPoseTimer > T_MARIO_KICK_POSE)
		{
			mario.aniId = ANI_RAC_IDLE;
			marioPoseTimer = 0;
			marioStep = 14;
		}
		break;
	case 14:
		marioPoseTimer += dt;
		if (marioPoseTimer > T_MARIO_BIG_DELAY)
		{
			mario.aniId = ANI_BIG_IDLE;
			marioPoseTimer = 0;
			marioStep = 15;
		}
		break;
	case 15:
		marioPoseTimer += dt;
		if (marioPoseTimer > T_MARIO_SMALL_DELAY)
		{
			mario.aniId = ANI_SMALL_WALK;
			mario.vx = INTRO_WALK_SPEED;
			mario.nx = 1;
			marioStep = 16;
		}
		break;
	case 16:
		if (mario.x > MARIO_TURN_R_X)
		{
			mario.nx = -1;
			mario.vx = -INTRO_WALK_SPEED;
			marioStep = 17;
		}
		break;
	case 17:
		if (mario.x < MARIO_TURN_L_X)
		{
			marioBehindTrees = true;
			mario.nx = 1;
			mario.vx = INTRO_WALK_SPEED;
			marioStep = 18;
		}
		break;
	case 18:
		if (mario.x > MARIO_EXIT_X)
		{
			mario.visible = false;
			mario.vx = 0;
			koopaWalker.visible = false;
			goomba.visible = false;
			menuActive = true;
			marioStep = 19;
		}
		break;
	}
}

void CIntroScene::UpdateLuigi(DWORD dt)
{
	switch (luigiStep)
	{
	case 0:
		luigi.aniId = ANI_LUIGI_WALK;
		luigi.nx = 1;
		luigi.vx = INTRO_WALK_SPEED;
		luigiStep = 1;
		break;
	case 1:
		if (luigi.x > LUIGI_CHARGE_X)
		{
			luigi.vx = 0;
			luigi.aniId = ANI_LUIGI_IDLE;
			luigiStep = 2;
		}
		break;
	case 2:
		if (timer > T_LUIGI_RELEASE)
		{
			luigi.vy = -INTRO_JUMP_V;
			luigi.onGround = false;
			luigi.vx = INTRO_RUN_SPEED;
			luigi.aniId = ANI_LUIGI_JUMP;
			luigiStep = 3;
		}
		break;
	case 3:
		if (luigi.onGround) luigi.aniId = ANI_LUIGI_WALK;
		if (luigi.x > LUIGI_OFF_X)
		{
			luigi.visible = false;
			luigi.vx = 0;
			luigiStep = 4;
		}
		break;
	case 4:
		if (timer > T_LUIGI_RETURN)
		{
			luigi.visible = true;
			luigi.x = LUIGI_OFF_X;
			luigi.y = GROUND_Y;
			luigi.nx = -1;
			luigi.vx = -INTRO_WALK_SPEED;
			luigi.aniId = ANI_LUIGI_HOLD;
			luigiHolding = true;
			luigiShell.visible = true;
			luigiStep = 5;
		}
		break;
	case 5:
		if (luigi.x < LUIGI_HOLD_X)
		{
			luigi.vx = 0;
			luigiStep = 6;
		}
		break;
	case 6:
		if (timer > T_LUIGI_KICK)
		{
			luigiHolding = false;
			luigiShell.x = luigi.x - CARRY_OFFSET_X - 4;
			luigiShell.y = GROUND_Y;
			luigiShell.vx = -INTRO_SHELL_SPEED;
			luigiShell.aniId = ANI_GREEN_SHELL_SPIN;
			luigiShellKicked = true;
			luigi.aniId = ANI_LUIGI_KICK;
			CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_KICK);
			luigiPoseTimer = 0;
			luigiStep = 7;
		}
		break;
	case 7:
		luigiPoseTimer += dt;
		if (luigiPoseTimer > T_LUIGI_KICK_POSE)
		{
			luigi.aniId = ANI_LUIGI_IDLE;
			luigiStep = 8;
		}
		break;
	case 8:
		if (redShell.visible && redShell.vx > 0 && fabs(luigi.x - redShell.x) < LUIGI_FLEE_GAP)
		{
			luigi.nx = 1;
			luigi.vx = INTRO_FLEE_SPEED;
			luigi.aniId = ANI_LUIGI_WALK;
			luigiStep = 9;
		}
		break;
	case 9:
		if (luigi.x > LUIGI_EXIT_X)
		{
			luigi.visible = false;
			luigi.vx = 0;
			luigiStep = 10;
		}
		break;
	}
}

void CIntroScene::UpdateItems(DWORD dt)
{
	if (itemStep == 0)
	{
		if (timer > T_ITEMS)
		{
			goomba.visible = true;
			goomba.vx = 0;
			mushRed.visible = true;
			mushRed.vy = INTRO_FALL_V;
			mushGreen.visible = true;
			mushGreen.vy = INTRO_FALL_V;
			leaf.visible = true;
			koopaWalker.visible = true;
			koopaWalker.x = mario.x;
			koopaWalker.vx = -INTRO_KOOPA_WALK;
			koopaWalker.aniId = ANI_GREEN_SHELL_IDLE;
			redShell.visible = true;
			itemStep = 1;
		}
		return;
	}

	if (mushRed.visible)
	{
		if (mushRed.onGround) mushRed.vx = -INTRO_MUSH_SLIDE;
		if (mushRed.x < OFFSCREEN_L) mushRed.visible = false;
	}

	if (mushGreen.visible)
	{
		if (mushGreen.onGround) mushGreen.vx = INTRO_MUSH_SLIDE;
		if (mushGreen.x > OFFSCREEN_R) mushGreen.visible = false;
	}

	if (leaf.visible)
	{
		leaf.y += INTRO_LEAF_FALL * dt;
		leaf.x = LEAF_SPAWN_X + INTRO_LEAF_SWAY_AMP * (float)sin(timer * INTRO_LEAF_SWAY_FREQ);

		if (!leafTaken && marioStep >= 4 && fabs(leaf.x - mario.x) < CATCH_GAP_X && fabs(leaf.y - (mario.y - 14.0f)) < CATCH_GAP_Y)
		{
			leafTaken = true;
			leaf.visible = false;
		}

		if (leaf.y > GROUND_Y)
		{
			leafTaken = true;
			leaf.visible = false;
		}
	}

	if (koopaWalker.visible && !koopaTucked)
	{
		if (koopaWalker.onGround && fabs(koopaWalker.x - goomba.x) < KOOPA_TUCK_GAP)
		{
			koopaWalker.vx = 0;
			koopaTucked = true;
		}
	}

	if (!goombaDead && goomba.visible && marioStep >= 6)
	{
		goomba.vx = goomba.x > GOOMBA_MIN_X ? -INTRO_GOOMBA_DRIFT : 0;
	}

	if (!goombaDead && goomba.visible && luigiShellKicked && luigiShell.visible && fabs(luigiShell.x - goomba.x) < SHELL_HIT_GAP)
	{
		goombaDead = true;
		goomba.ny = -1;
		goomba.vy = -INTRO_JUMP_V * 0.6f;
		goomba.vx = -0.02f;
		goomba.onGround = false;
		CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_KICK);
	}

	if (goombaDead && goomba.visible && goomba.y > INTRO_SCREEN_H + 20.0f) goomba.visible = false;

	if (luigiShell.visible && luigiShellKicked && luigiShell.x < OFFSCREEN_L) luigiShell.visible = false;
	if (redShell.visible && redShell.vx > 0 && redShell.x > OFFSCREEN_R) redShell.visible = false;
}

void CIntroScene::UpdateLateShells(DWORD dt)
{
	lateShellTimer += dt;

	if (lateShellStep == 0)
	{
		lateShells[0].visible = true;
		lateShells[0].x = OFFSCREEN_L;
		lateShells[0].vx = LATE_SHELL_V1;
		lateShellStep = 1;
	}
	if (lateShellStep == 1 && lateShellTimer > T_LATE_SHELL_2)
	{
		lateShells[1].visible = true;
		lateShells[1].x = OFFSCREEN_L;
		lateShells[1].vx = LATE_SHELL_V2;
		lateShellStep = 2;
	}
	if (lateShellStep == 2 && lateShellTimer > T_LATE_SHELL_3)
	{
		lateShells[2].visible = true;
		lateShells[2].x = OFFSCREEN_L;
		lateShells[2].vx = LATE_SHELL_V3;
		lateShellStep = 3;
	}
	for (int i = 0; i < 3; i++)
	{
		if (lateShells[i].visible && lateShells[i].x > OFFSCREEN_R) lateShells[i].visible = false;
	}

	if (lateShellStep == 3 && !lateShells[0].visible && !lateShells[1].visible && !lateShells[2].visible)
	{
		lateShellTimer = 0;
		lateShellStep = 0;
	}
}

void CIntroScene::Update(DWORD dt)
{
	if (dt > INTRO_DT_CLAMP) dt = INTRO_DT_CLAMP;
	dt = dt * INTRO_TIME_SCALE_NUM / INTRO_TIME_SCALE_DEN;
	timer += dt;

	UpdateStage(dt);

	if (curtainY <= CURTAIN_DONE_Y)
	{
		UpdateMario(dt);
		UpdateLuigi(dt);
		UpdateItems(dt);
	}

	ApplyPhysics(mario, dt);
	ApplyPhysics(luigi, dt);
	ApplyPhysics(goomba, dt);
	ApplyPhysics(koopaWalker, dt);
	ApplyPhysics(mushRed, dt);
	ApplyPhysics(mushGreen, dt);

	if (!marioHolding) ApplyPhysics(redShell, dt);
	if (!luigiHolding) ApplyPhysics(luigiShell, dt);

	if (menuActive)
	{
		UpdateLateShells(dt);
		for (int i = 0; i < 3; i++) ApplyPhysics(lateShells[i], dt);
	}
}

void CIntroScene::DrawActor(CIntroActor& a, float dy)
{
	if (!a.visible) return;

	LPANIMATION ani = CAnimations::GetInstance()->Get(a.aniId);
	if (ani == NULL) return;

	float h = (float)ani->GetSpriteHeight();
	ani->Render(a.x, a.y + dy - h / 2.0f, a.nx, a.ny);
}

void CIntroScene::DrawStatic(int spriteId, float cx, float cy)
{
	LPSPRITE s = CSprites::GetInstance()->Get(spriteId);
	if (s == NULL) return;
	s->Draw(cx, cy);
}

void CIntroScene::Render()
{
	CGame* game = CGame::GetInstance();

	if (bgColorIndex > 0)
	{
		LPTEXTURE texWhite = CTextures::GetInstance()->Get(ID_TEX_WHITE);
		const float* c = BG_COLORS[bgColorIndex - 1];
		game->Draw(INTRO_SCREEN_W / 2.0f, INTRO_SCREEN_H / 2.0f, texWhite, (RECT*)NULL,
			D3DXCOLOR(c[0] / 255.0f, c[1] / 255.0f, c[2] / 255.0f, 1.0f),
			(int)INTRO_SCREEN_W, (int)INTRO_SCREEN_H);
	}

	DrawStatic(SPR_INTRO_GROUND, INTRO_SCREEN_W / 2.0f, GROUND_CY);

	if (marioBehindTrees) DrawActor(mario);

	if (treesVisible)
	{
		DrawStatic(SPR_INTRO_TREE_RIGHT, TREE_RIGHT_CX, TREE_RIGHT_CY);
		DrawStatic(SPR_INTRO_TREE_LEFT, TREE_LEFT_CX, TREE_LEFT_CY);
	}

	DrawActor(goomba, goombaDead ? 0.0f : GOOMBA_FOOT_SINK);
	DrawActor(koopaWalker);
	DrawActor(mushRed);
	DrawActor(mushGreen);
	DrawActor(leaf);

	if (!marioHolding) DrawActor(redShell);
	if (!luigiHolding) DrawActor(luigiShell);

	for (int i = 0; i < 3; i++) DrawActor(lateShells[i]);

	DrawActor(luigi);
	if (luigiHolding && luigi.visible)
	{
		luigiShell.x = luigi.x + CARRY_OFFSET_X * luigi.nx;
		luigiShell.y = luigi.y - CARRY_OFFSET_Y;
		DrawActor(luigiShell);
	}

	if (!marioBehindTrees) DrawActor(mario);
	if (marioHolding && mario.visible)
	{
		redShell.x = mario.x + CARRY_OFFSET_X * mario.nx;
		redShell.y = mario.y - CARRY_OFFSET_Y;
		DrawActor(redShell);
	}

	if (bottomCurtainVisible)
		DrawStatic(SPR_INTRO_VALANCE, INTRO_SCREEN_W / 2.0f, BOTTOM_CURTAIN_CY);

	if (logoStep >= 1)
	{
		float shake = 0;
		if (logoStep == 2) shake = (float)(rand() % LOGO_SHAKE_AMP - rand() % LOGO_SHAKE_AMP);

		DrawStatic(SPR_INTRO_TITLE, TITLE_CX, titleY + shake);
		if (logoStep < 2 || badgeBlinkTimer < BADGE_BLINK_ON)
			DrawStatic(SPR_INTRO_BADGE, BADGE_CX, badgeY + shake);
	}

	if (curtainY > CURTAIN_DONE_Y)
		DrawStatic(SPR_INTRO_CURTAIN, INTRO_SCREEN_W / 2.0f, curtainY + CURTAIN_H / 2.0f);

	if (menuActive)
	{
		DrawStatic(SPR_INTRO_OPTIONS, MENU_PANEL_CX, MENU_PANEL_CY);
		DrawStatic(SPR_INTRO_ARROW, MENU_ARROW_X, menuRow == 0 ? MENU_ARROW_Y1 : MENU_ARROW_Y2);
	}
}

void CIntroScene::Unload()
{
	CSoundManager::GetInstance()->StopMusic();
	DebugOut(L"[INFO] Intro scene unloaded\n");
}

void CIntroScene::MenuMove()
{
	if (!menuActive) return;
	menuRow = 1 - menuRow;
	CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_CURSOR);
}

void CIntroScene::MenuConfirm()
{
	if (!menuActive) return;
	CSoundManager::GetInstance()->PlaySfx(INTRO_SFX_SELECT);
	CGame::GetInstance()->InitiateSwitchScene(INTRO_SCENE_WORLDMAP);
}

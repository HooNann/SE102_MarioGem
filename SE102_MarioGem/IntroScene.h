#pragma once

#include <string>

#include "Scene.h"

struct CIntroActor
{
	float x, y, vx, vy, g;
	int nx;
	int ny;
	bool visible, onGround;
	int aniId;
};

class CIntroScene : public CScene
{
protected:
	DWORD timer;
	bool menuActive;
	int menuRow;

	float curtainY;
	int logoStep;
	DWORD logoTimer;
	DWORD badgeBlinkTimer;
	float titleY, badgeY;
	int bgColorIndex;
	bool treesVisible;
	bool bottomCurtainVisible;

	int marioStep, luigiStep, itemStep;
	DWORD marioPoseTimer, luigiPoseTimer;
	bool marioHolding, luigiHolding;
	bool marioBehindTrees;
	bool leafTaken;
	bool luigiShellKicked;
	bool goombaDead;
	bool koopaTucked;

	CIntroActor mario, luigi, goomba, koopaWalker, redShell, luigiShell, mushRed, mushGreen, leaf;
	CIntroActor lateShells[3];
	DWORD lateShellTimer;
	int lateShellStep;

	void _ParseSection_SPRITES(std::string line);
	void _ParseSection_ANIMATIONS(std::string line);
	void _ParseSection_ASSETS(std::string line);
	void LoadAssets(LPCWSTR assetFile);

	void ResetActors();
	void UpdateStage(DWORD dt);
	void UpdateMario(DWORD dt);
	void UpdateLuigi(DWORD dt);
	void UpdateItems(DWORD dt);
	void UpdateLateShells(DWORD dt);
	void ApplyPhysics(CIntroActor& a, DWORD dt);
	void DrawActor(CIntroActor& a, float dy = 0);
	void DrawStatic(int spriteId, float cx, float cy);

public:
	CIntroScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	bool IsMenuActive() { return menuActive; }
	void SkipToMenu();
	void MenuMove();
	void MenuConfirm();
};

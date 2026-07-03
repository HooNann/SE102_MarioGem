#include "SoundTestScene.h"
#include "SoundTestKeyHandler.h"
#include "SoundManager.h"
#include "debug.h"

CSoundTestScene::CSoundTestScene(int id, LPCWSTR filePath) : CScene(id, filePath)
{
	key_handler = new CSoundTestKeyHandler(this);
}

void CSoundTestScene::Load()
{
	CSoundManager::GetInstance();
	DebugOut(L"[INFO] Sound test scene ready. Keys 1-9,Q,W,E,R = SFX; M,N = music; S = stop; +/- = volume\n");
}

void CSoundTestScene::Update(DWORD dt)
{
}

void CSoundTestScene::Render()
{
}

void CSoundTestScene::Unload()
{
}

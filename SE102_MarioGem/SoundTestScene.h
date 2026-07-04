#pragma once

#include "Scene.h"

class CSoundTestScene : public CScene
{
public:
	CSoundTestScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
};

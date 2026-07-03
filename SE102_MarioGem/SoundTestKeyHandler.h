#pragma once

#include "Scene.h"

class CSoundTestKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CSoundTestKeyHandler(LPSCENE s) : CSceneKeyHandler(s) {}
};

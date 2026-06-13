#pragma once

#include "Scene.h"

class CPlaySceneKeyHandler: public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(LPSCENE s) :CSceneKeyHandler(s) {};
};

#include "IntroKeyHandler.h"
#include "IntroScene.h"
#include "Game.h"

void CIntroKeyHandler::OnKeyDown(int KeyCode)
{
	CIntroScene* intro = (CIntroScene*)scene;
	if (intro == NULL) return;

	switch (KeyCode)
	{
	case DIK_UP:
	case DIK_DOWN:
		intro->MenuMove();
		break;
	case DIK_RETURN:
		if (intro->IsMenuActive())
			intro->MenuConfirm();
		else
			intro->SkipToMenu();
		break;
	}
}

void CIntroKeyHandler::OnKeyUp(int KeyCode)
{
}

void CIntroKeyHandler::KeyState(BYTE* states)
{
}

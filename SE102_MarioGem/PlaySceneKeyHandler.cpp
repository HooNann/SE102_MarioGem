#include "PlaySceneKeyHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"
#include "CMarioIdleState.h"
#include "CMarioWalkState.h"
#include "CMarioRunState.h"
#include "CMarioJumpState.h"
#include "CMarioFallState.h"
#include "CMarioDuckState.h"
#include "CMarioDeadState.h"

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CMario* mario = (CMario *)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer(); 

	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->ChangeState(new CMarioDuckState());
		break;
	case DIK_Z:
		mario->ChangeState(new CMarioJumpState());
		break;
	case DIK_1:
		mario->SetLevel(MarioLevel::Small);
		break;
	case DIK_2:
		mario->SetLevel(MarioLevel::Big);
		break;
	case DIK_0:
		mario->ChangeState(new CMarioDeadState());
		break;
	case DIK_R: // reset
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->ReloadAssets();
		break;
	}
}

void CPlaySceneKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (KeyCode)
	{
	case DIK_Z:
		mario->ChangeState(new CMarioFallState());
		break;
	case DIK_DOWN:
        // Exiting DuckState is handled in Exit(), but we need to transition out to Idle
		mario->ChangeState(new CMarioIdleState());
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE *states)
{
	LPGAME game = CGame::GetInstance();
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    if (mario->currentState && mario->currentState->GetID() == MarioStateID::Duck)
        return;

	if (game->IsKeyDown(DIK_RIGHT))
	{
        mario->SetDirection(1);
		if (game->IsKeyDown(DIK_X))
			mario->ChangeState(new CMarioRunState());
		else
			mario->ChangeState(new CMarioWalkState());
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
        mario->SetDirection(-1);
		if (game->IsKeyDown(DIK_X))
			mario->ChangeState(new CMarioRunState());
		else
			mario->ChangeState(new CMarioWalkState());
	}
	else
		mario->ChangeState(new CMarioIdleState());
}

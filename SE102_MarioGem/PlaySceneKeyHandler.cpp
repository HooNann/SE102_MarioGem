#include "PlaySceneKeyHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CMario* mario = (CMario *)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer(); 

	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MarioState::Sit);
		break;
	case DIK_S:
		mario->SetState(MarioState::Jump);
		break;
	case DIK_1:
		mario->SetLevel(MarioLevel::Small);
		break;
	case DIK_2:
		mario->SetLevel(MarioLevel::Big);
		break;
	case DIK_0:
		mario->SetState(MarioState::Die);
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
	case DIK_S:
		mario->SetState(MarioState::ReleaseJump);
		break;
	case DIK_DOWN:
		mario->SetState(MarioState::SitRelease);
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE *states)
{
	LPGAME game = CGame::GetInstance();
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (game->IsKeyDown(DIK_A))
			mario->SetState(MarioState::RunningRight);
		else
			mario->SetState(MarioState::WalkingRight);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (game->IsKeyDown(DIK_A))
			mario->SetState(MarioState::RunningLeft);
		else
			mario->SetState(MarioState::WalkingLeft);
	}
	else
		mario->SetState(MarioState::Idle);
}

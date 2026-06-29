#include "WorldMapKeyHandler.h"
#include "WorldMapScene.h"
#include "MapMario.h"
#include "Game.h"
#include "debug.h"

void CWorldMapKeyHandler::OnKeyDown(int KeyCode)
{
	CWorldMapScene* mapScene = (CWorldMapScene*)((LPSCENE)scene);
	CMapMario* mario = mapScene->GetPlayer();
	
	if (mario == NULL) return;
	if (mario->isMoving) return; // Không cho phép đổi hướng khi đang di chuyển

	if (mario->currentNode == NULL) return;

	switch (KeyCode)
	{
	case DIK_UP:
		if (mario->currentNode->upNode != NULL)
			mario->MoveToNode(mario->currentNode->upNode);
		break;
	case DIK_DOWN:
		if (mario->currentNode->downNode != NULL)
			mario->MoveToNode(mario->currentNode->downNode);
		break;
	case DIK_LEFT:
		if (mario->currentNode->leftNode != NULL)
			mario->MoveToNode(mario->currentNode->leftNode);
		break;
	case DIK_RIGHT:
		if (mario->currentNode->rightNode != NULL)
			mario->MoveToNode(mario->currentNode->rightNode);
		break;
	case DIK_S:
		if (mario->currentNode->sceneId != -1)
		{
			DebugOut(L"[INFO] Switch to scene %d\n", mario->currentNode->sceneId);
			CGame::GetInstance()->InitiateSwitchScene(mario->currentNode->sceneId);
		}
		break;
	}
}

void CWorldMapKeyHandler::OnKeyUp(int KeyCode) {}

void CWorldMapKeyHandler::KeyState(BYTE* states) {}

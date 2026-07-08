#include "WorldMapKeyHandler.h"
#include "WorldMapScene.h"
#include "MapMario.h"
#include "Game.h"
#include "debug.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

void CWorldMapKeyHandler::OnKeyDown(int KeyCode)
{
	CWorldMapScene* mapScene = (CWorldMapScene*)((LPSCENE)scene);
	CMapMario* mario = mapScene->GetPlayer();
	
	if (mario == NULL) return;
	if (mario->isMoving) return;

	if (mario->currentNode == NULL) return;

	switch (KeyCode)
	{
	case DIK_UP:
		if (mario->currentNode->upNode != NULL)
		{
			CSoundSubject::GetInstance()->Notify(EVENT_MAP_MOVE);
			mario->MoveToNode(mario->currentNode->upNode);
		}
		break;
	case DIK_DOWN:
		if (mario->currentNode->downNode != NULL)
		{
			CSoundSubject::GetInstance()->Notify(EVENT_MAP_MOVE);
			mario->MoveToNode(mario->currentNode->downNode);
		}
		break;
	case DIK_LEFT:
		if (mario->currentNode->leftNode != NULL)
		{
			CSoundSubject::GetInstance()->Notify(EVENT_MAP_MOVE);
			mario->MoveToNode(mario->currentNode->leftNode);
		}
		break;
	case DIK_RIGHT:
		if (mario->currentNode->rightNode != NULL)
		{
			CSoundSubject::GetInstance()->Notify(EVENT_MAP_MOVE);
			mario->MoveToNode(mario->currentNode->rightNode);
		}
		break;
	case DIK_Z:
		if (mario->currentNode->sceneId != -1)
		{
			int sceneId = mario->currentNode->sceneId;
			DebugOut(L"[INFO] Switch to scene %d\n", sceneId);
			CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_STOP);
			CSoundSubject::GetInstance()->Notify(EVENT_LEVEL_START);
			CGame::GetInstance()->StartIrisClose(true, [sceneId]() {
				CGame::GetInstance()->InitiateSwitchScene(sceneId);
			});
		}
		break;
	}
}

void CWorldMapKeyHandler::OnKeyUp(int KeyCode) {}

void CWorldMapKeyHandler::KeyState(BYTE* states) {}

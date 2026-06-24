#include "MapMario.h"
#include <cmath>
#include "debug.h"

CMapMario::CMapMario(float x, float y, int aniId) : CMapObject(x, y, aniId)
{
	currentNode = NULL;
	targetNode = NULL;
	isMoving = false;
	destX = destY = 0;
	vx = vy = 0;
}

void CMapMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	if (isMoving && targetNode != NULL)
	{
		x += vx * dt;
		y += vy * dt;

		// Kiểm tra xem đã tới đích chưa
		bool reached = false;
		if (vx > 0 && x >= destX) { x = destX; reached = true; }
		else if (vx < 0 && x <= destX) { x = destX; reached = true; }
		else if (vy > 0 && y >= destY) { y = destY; reached = true; }
		else if (vy < 0 && y <= destY) { y = destY; reached = true; }

		if (reached)
		{
			isMoving = false;
			vx = vy = 0;
			currentNode = targetNode;
			targetNode = NULL;
			DebugOut(L"[INFO] MapMario reached node %d\n", currentNode->nodeId);
		}
	}
}

void CMapMario::Render()
{
	if (aniId != -1)
	{
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	}
}

void CMapMario::MoveToNode(CMapNode* node)
{
	if (node == NULL || isMoving) return;

	targetNode = node;
	node->GetPosition(destX, destY);

	if (destX > x) vx = MAP_MARIO_SPEED;
	else if (destX < x) vx = -MAP_MARIO_SPEED;
	else vx = 0;

	if (destY > y) vy = MAP_MARIO_SPEED;
	else if (destY < y) vy = -MAP_MARIO_SPEED;
	else vy = 0;

	isMoving = true;
}

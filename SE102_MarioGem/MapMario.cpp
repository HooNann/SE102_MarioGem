#include "MapMario.h"
#include <cmath>
#include "debug.h"
#include "GameData.h"

CMapMario::CMapMario(float x, float y, int aniId, int start_node_id) : CMapObject(x, y, aniId)
{
	this->start_node_id = start_node_id;
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
			CGameData::GetInstance()->SetCurrentNodeId(currentNode->nodeId);
			DebugOut(L"[INFO] MapMario reached node %d\n", currentNode->nodeId);
		}
	}
}

void CMapMario::Render()
{
	int currentAniId = aniId;

	MarioLevel level = CGameData::GetInstance()->GetLevel();
	switch (level)
	{
	case MarioLevel::Small:
		currentAniId = ID_ANI_MAP_MARIO_SMALL;
		break;
	case MarioLevel::Big:
		currentAniId = ID_ANI_MAP_MARIO_BIG;
		break;
	case MarioLevel::Fire:
		currentAniId = ID_ANI_MAP_MARIO_FIRE;
		break;
	case MarioLevel::Raccoon:
		currentAniId = ID_ANI_MAP_MARIO_RACCOON;
		break;
	}

	if (currentAniId != -1)
	{
		CAnimations::GetInstance()->Get(currentAniId)->Render(x, y);
	}
}

void CMapMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	int currentAniId = aniId; 

	MarioLevel level = CGameData::GetInstance()->GetLevel();
	switch (level)
	{
	case MarioLevel::Small:
		currentAniId = ID_ANI_MAP_MARIO_SMALL;
		break;
	case MarioLevel::Big:
		currentAniId = ID_ANI_MAP_MARIO_BIG;
		break;
	case MarioLevel::Fire:
		currentAniId = ID_ANI_MAP_MARIO_FIRE;
		break;
	case MarioLevel::Raccoon:
		currentAniId = ID_ANI_MAP_MARIO_RACCOON;
		break;
	}

	float w = 0, h = 0;
	if (currentAniId != -1)
	{
		LPANIMATION ani = CAnimations::GetInstance()->Get(currentAniId);
		if (ani)
		{
			w = (float)ani->GetSpriteWidth();
			h = (float)ani->GetSpriteHeight();
		}
	}
	left = x - w / 2;
	right = x + w / 2;
	top = y - h / 2;
	bottom = y + h / 2;
}

void CMapMario::MoveToNode(CMapNode* node)
{
	if (node == NULL || isMoving) return;

	targetNode = node;
	node->GetPosition(destX, destY);

	float nl, nt, nr, nb;
	node->GetBoundingBox(nl, nt, nr, nb);

	float ml, mt, mr, mb;
	this->GetBoundingBox(ml, mt, mr, mb);
	float mario_h = mb - mt;

	if (mario_h > 0) 
	{
		destY = nb - mario_h / 2.0f;
	}

	if (destX > x) vx = MAP_MARIO_SPEED;
	else if (destX < x) vx = -MAP_MARIO_SPEED;
	else vx = 0;

	if (destY > y) vy = MAP_MARIO_SPEED;
	else if (destY < y) vy = -MAP_MARIO_SPEED;
	else vy = 0;

	isMoving = true;
}

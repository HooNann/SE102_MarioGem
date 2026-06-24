#pragma once
#include "MapObject.h"
#include "MapNode.h"

#define MAP_MARIO_SPEED 0.15f

class CMapMario : public CMapObject
{
public:
	CMapNode* currentNode;
	CMapNode* targetNode;

	bool isMoving;
	float destX, destY;

	CMapMario(float x, float y, int aniId = -1);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

	void MoveToNode(CMapNode* node);
};

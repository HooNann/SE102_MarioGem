#pragma once
#include "MapObject.h"
#include "MapNode.h"

#define MAP_MARIO_SPEED 0.15f

constexpr int ID_ANI_MAP_MARIO_SMALL = 10001;
constexpr int ID_ANI_MAP_MARIO_BIG = 10002;
constexpr int ID_ANI_MAP_MARIO_RACCOON = 10003;
constexpr int ID_ANI_MAP_MARIO_FIRE = 10004;
constexpr int ID_ANI_MAP_MARIO_HAMMER_SMALL = 10005;
constexpr int ID_ANI_MAP_MARIO_HAMMER = 10006;
constexpr int ID_ANI_MAP_MARIO_FROG_SMALL = 10007;
constexpr int ID_ANI_MAP_MARIO_FROG = 10008;
constexpr int ID_ANI_MAP_MARIO_TANOOKI_SMALL = 10009;
constexpr int ID_ANI_MAP_MARIO_TANOOKI = 10010;

class CMapMario : public CMapObject
{
public:
	CMapNode* currentNode;
	CMapNode* targetNode;

	bool isMoving;
	float destX, destY;
	int start_node_id;

	CMapMario(float x, float y, int aniId = -1, int start_node_id = -1);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	void MoveToNode(CMapNode* node);
};

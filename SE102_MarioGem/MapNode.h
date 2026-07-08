#pragma once
#include "MapObject.h"
#include <string>

using namespace std;

class CMapNode : public CMapObject
{
public:
	int nodeId;
	int sceneId;
	
	CMapNode* upNode;
	CMapNode* downNode;
	CMapNode* leftNode;
	CMapNode* rightNode;

	int up_id;
	int down_id;
	int left_id;
	int right_id;

	int aniActive;
	int aniCleared;

	CMapNode(int id, float x, float y, int scene_id = -1, int ani_active = -1, int ani_cleared = -1);

	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};

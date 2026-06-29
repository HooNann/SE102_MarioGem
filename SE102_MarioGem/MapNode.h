#pragma once
#include "MapObject.h"
#include <string>

using namespace std;

class CMapNode : public CMapObject
{
public:
	int nodeId;
	int sceneId;
	
	// Con trỏ tới các Node xung quanh
	CMapNode* upNode;
	CMapNode* downNode;
	CMapNode* leftNode;
	CMapNode* rightNode;

	// Dùng tạm để parse từ JSON trước khi nối con trỏ
	int up_id;
	int down_id;
	int left_id;
	int right_id;

	CMapNode(int id, float x, float y, int scene_id = -1);
};

#include "MapNode.h"

CMapNode::CMapNode(int id, float x, float y, int scene_id) : CMapObject(x, y, -1)
{
	this->nodeId = id;
	this->sceneId = scene_id;
	
	upNode = NULL;
	downNode = NULL;
	leftNode = NULL;
	rightNode = NULL;

	up_id = -1;
	down_id = -1;
	left_id = -1;
	right_id = -1;
}

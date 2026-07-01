#include "MapNode.h"
#include "GameData.h"
#include "Animations.h"

CMapNode::CMapNode(int id, float x, float y, int scene_id, int ani_active, int ani_cleared) : CMapObject(x, y, -1)
{
	this->nodeId = id;
	this->sceneId = scene_id;
	this->aniActive = ani_active;
	this->aniCleared = ani_cleared;
	
	upNode = NULL;
	downNode = NULL;
	leftNode = NULL;
	rightNode = NULL;

	up_id = -1;
	down_id = -1;
	left_id = -1;
	right_id = -1;
}

void CMapNode::Render()
{
	int currentAniId = -1;

	if (sceneId != -1)
	{
		if (CGameData::GetInstance()->IsSceneCleared(sceneId))
		{
			currentAniId = aniCleared; 
		}
		else
		{
			currentAniId = aniActive; 
		}
	}
	else
	{
		currentAniId = aniActive;
	}

	if (currentAniId != -1)
	{
		CAnimations::GetInstance()->Get(currentAniId)->Render(x, y);
	}
}

void CMapNode::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	float w = 0, h = 0;
	int currentAniId = -1;
	if (sceneId != -1)
	{
		currentAniId = CGameData::GetInstance()->IsSceneCleared(sceneId) ? aniCleared : aniActive;
	}
	else
	{
		currentAniId = aniActive;
	}
	if (currentAniId != -1)
	{
		LPANIMATION ani = CAnimations::GetInstance()->Get(currentAniId);
		if (ani)
		{
			w = (float)ani->GetSpriteWidth();
			h = (float)ani->GetSpriteHeight();
		}
	}

	if (w == 0 || h == 0)
	{
		w = 16.0f;
		h = 16.0f;
	}

	left = x - w / 2;
	right = x + w / 2;
	top = y - h / 2;
	bottom = y + h / 2;
}

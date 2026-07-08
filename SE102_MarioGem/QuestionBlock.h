#pragma once

#include "GameObject.h"
#include <string>

#define QUESTION_BLOCK_STATE_ALIVE	100
#define QUESTION_BLOCK_STATE_HIT	200
#define QUESTION_BLOCK_STATE_EMPTY	300

#define QUESTION_BLOCK_BUMP_SPEED	0.15f
#define QUESTION_BLOCK_BUMP_MAX_DIST	8.0f	

constexpr int ID_ANI_QUESTION_BLOCK_ALIVE = 91002;
constexpr int ID_ANI_QUESTION_BLOCK_EMPTY = 91003;

class CQuestionBlock : public CGameObject
{
protected:
	std::string itemType;
	int isAlive;

	float startY;
	int bumpDirection;

public:
	CQuestionBlock(float x, float y, const std::string& itemType);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void SetState(int state);
	

	virtual int IsBlocking() { return 1; }

	std::string GetItemType() { return itemType; }
	int IsAlive() { return isAlive; }

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void ReleaseItem();
};
typedef CQuestionBlock* LPQUESTIONBLOCK;

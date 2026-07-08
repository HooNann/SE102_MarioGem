#include "QuestionBlock.h"
#include "Animations.h"
#include "PlayScene.h"
#include "CoinBounceEffect.h"
#include "Flower.h"
#include "GameData.h"
#include "Leaf.h"
#include "Mushroom.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

namespace
{
	std::string ResolveItemType(const std::string& itemType)
	{
		if ((itemType == "Flower" || itemType == "Leaf") &&
			CGameData::GetInstance()->GetLevel() == MarioLevel::Small)
		{
			return "Mushroom";
		}

		return itemType;
	}

	bool ShouldReleaseAfterBump(const std::string& itemType)
	{
		std::string resolvedItemType = ResolveItemType(itemType);
		return resolvedItemType == "Flower" || resolvedItemType == "Mushroom";
	}
}

CQuestionBlock::CQuestionBlock(float x, float y, const std::string& itemType) : CGameObject(x, y)
{
	this->itemType = itemType;
	this->isAlive = 1;
	this->startY = y;
	this->bumpDirection = 0;

	this->state = QUESTION_BLOCK_STATE_ALIVE;
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (bumpDirection == -1)
	{
		y -= QUESTION_BLOCK_BUMP_SPEED * dt;
		if (startY - y >= QUESTION_BLOCK_BUMP_MAX_DIST)
		{
			y = startY - QUESTION_BLOCK_BUMP_MAX_DIST;
			bumpDirection = 1;
		}
	}
	else if (bumpDirection == 1)
	{
		y += QUESTION_BLOCK_BUMP_SPEED * dt;
		if (y >= startY)
		{
			y = startY;
			bumpDirection = 0;
			this->SetState(QUESTION_BLOCK_STATE_EMPTY);

			if (ShouldReleaseAfterBump(itemType))
				ReleaseItem();
		}
	}
}

void CQuestionBlock::SetState(int state)
{
	switch (state)
	{
	case QUESTION_BLOCK_STATE_HIT:
		if (isAlive)
		{
			isAlive = 0;
			bumpDirection = -1;
			if (!ShouldReleaseAfterBump(itemType))
				ReleaseItem();
		}
		break;
	}

	CGameObject::SetState(state);
}

void CQuestionBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - 16.0f / 2;
	t = y - 16.0f / 2;
	r = l + 16.0f;
	b = t + 16.0f;
}

void CQuestionBlock::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == QUESTION_BLOCK_STATE_ALIVE)
	{
		aniId = ID_ANI_QUESTION_BLOCK_ALIVE;
	}
	else
	{
		aniId = ID_ANI_QUESTION_BLOCK_EMPTY;
	}

	animations->Get(aniId)->Render(x, y);
}

void CQuestionBlock::ReleaseItem()
{
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();

	float itemX = this->x;
	float itemY = this->y;
	std::string resolvedItemType = ResolveItemType(this->itemType);

	LPGAMEOBJECT newItem = NULL;

	if (resolvedItemType == "Flower")
	{
		newItem = new CFlower(itemX, itemY);
	}
	else if (resolvedItemType == "Leaf")
	{
		newItem = new CLeaf(itemX, itemY);
	}
	else if (resolvedItemType == "Mushroom")
	{
		newItem = new CMushroom(itemX, itemY);
	}
	else if (resolvedItemType == "Coin")
	{
		CGameData::GetInstance()->AddCoin(1);
		CGameData::GetInstance()->AddScore(100);
		CSoundSubject::GetInstance()->Notify(EVENT_COIN);
		currentScene->QueueSpawn(new CCoinBounceEffect(itemX, itemY - 16.0f));
		return;
	}

	if (newItem != NULL)
	{
		currentScene->QueueSpawnBehind(newItem, this);
	}
}

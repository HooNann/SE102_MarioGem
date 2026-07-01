#include "GameData.h"

CGameData* CGameData::__instance = nullptr;

CGameData::CGameData()
{
	Reset();
}

CGameData* CGameData::GetInstance()
{
	if (__instance == nullptr)
	{
		__instance = new CGameData();
	}
	return __instance;
}

void CGameData::AddCoin(int c)
{
	coin += c;
	if (coin >= 100)
	{
		AddLife(1);
		coin -= 100;
	}
}

void CGameData::AddItem(ItemType item)
{
	if (inventory.size() < MAX_INVENTORY)
	{
		inventory.push_back(item);
	}
	else
	{
		inventory.erase(inventory.begin());
		inventory.push_back(item);
	}
}

void CGameData::Reset()
{
	level = MarioLevel::Small;
	coin = 0;
	score = 0;
	lives = 4;
	currentWorld = 1;
	currentNodeId = 0; 
	clearedScenes.clear();
	inventory.clear();
}

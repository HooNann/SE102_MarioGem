#pragma once
#include <vector>
#include <map>

enum class MarioLevel : int
{
	Small = 1,
	Big = 2,
	Fire = 3,
	Raccoon = 4
};

enum class ItemType : int
{
	None = 0,
	Mushroom = 1,
	Flower = 2,
	Star = 3
};

class CGameData
{
private:
	static CGameData* __instance;

	MarioLevel level;
	int coin;
	int score;
	int lives;

	// Progression
	int currentWorld;
	int currentNodeId;
	std::map<int, bool> clearedScenes;

	// Inventory
	std::vector<ItemType> inventory;
	static const int MAX_INVENTORY = 3;

	CGameData();

public:
	static CGameData* GetInstance();

	MarioLevel GetLevel() { return level; }
	void SetLevel(MarioLevel l) { level = l; }

	int GetCoin() { return coin; }
	void SetCoin(int c) { coin = c; }
	void AddCoin(int c);

	int GetScore() { return score; }
	void SetScore(int s) { score = s; }
	void AddScore(int s) { score += s; }

	int GetLives() { return lives; }
	void SetLives(int l) { lives = l; }
	void AddLife(int l) { lives += l; }

	int GetCurrentWorld() { return currentWorld; }
	void SetCurrentWorld(int w) { currentWorld = w; }

	int GetCurrentNodeId() { return currentNodeId; }
	void SetCurrentNodeId(int n) { currentNodeId = n; }

	void MarkSceneCleared(int sceneId) { clearedScenes[sceneId] = true; }
	bool IsSceneCleared(int sceneId) { return clearedScenes[sceneId]; }

	std::vector<ItemType> GetInventory() { return inventory; }
	void AddItem(ItemType item);

	void Reset();
};

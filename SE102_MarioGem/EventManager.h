#pragma once
#include <queue>
#include "GameEvent.h"

class CEventManager
{
	static CEventManager* __instance;
	std::queue<CGameEvent*> events;
	CGameEvent* currentEvent;

	CEventManager();
public:
	static CEventManager* GetInstance();

	void AddEvent(CGameEvent* e);
	void Update(DWORD dt);
	void Clear();
};

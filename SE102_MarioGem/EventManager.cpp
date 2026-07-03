#include "EventManager.h"

CEventManager* CEventManager::__instance = nullptr;

CEventManager* CEventManager::GetInstance()
{
	if (__instance == nullptr)
	{
		__instance = new CEventManager();
	}
	return __instance;
}

CEventManager::CEventManager()
{
	currentEvent = nullptr;
}

void CEventManager::AddEvent(CGameEvent* e)
{
	events.push(e);
}

void CEventManager::Update(DWORD dt)
{
	if (currentEvent == nullptr && !events.empty())
	{
		currentEvent = events.front();
		events.pop();
		currentEvent->Start();
	}

	if (currentEvent != nullptr)
	{
		currentEvent->Update(dt);
		if (currentEvent->IsDone())
		{
			delete currentEvent;
			currentEvent = nullptr;
		}
	}
}

void CEventManager::Clear()
{
	if (currentEvent != nullptr)
	{
		delete currentEvent;
		currentEvent = nullptr;
	}
	while (!events.empty())
	{
		CGameEvent* e = events.front();
		events.pop();
		delete e;
	}
}

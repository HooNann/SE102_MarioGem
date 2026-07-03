#pragma once
#include <Windows.h>
#include <functional>

class CGameEvent
{
public:
	virtual ~CGameEvent() {}
	virtual void Start() = 0;
	virtual void Update(DWORD dt) = 0;
	virtual bool IsDone() = 0;
};

class CEventDelay : public CGameEvent
{
	DWORD delayTime;
	DWORD elapsed;
public:
	CEventDelay(DWORD time);
	virtual void Start();
	virtual void Update(DWORD dt);
	virtual bool IsDone();
};

class CEventWaitForSound : public CGameEvent
{
	size_t trackId;
public:
	CEventWaitForSound(size_t trackId);
	virtual void Start();
	virtual void Update(DWORD dt);
	virtual bool IsDone();
};

class CEventAction : public CGameEvent
{
	std::function<void()> action;
	bool done;
public:
	CEventAction(std::function<void()> action);
	virtual void Start();
	virtual void Update(DWORD dt);
	virtual bool IsDone();
};

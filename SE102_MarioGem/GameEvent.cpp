#include "GameEvent.h"
#include "SoundManager.h"

// CEventDelay
CEventDelay::CEventDelay(DWORD time)
{
	this->delayTime = time;
	this->elapsed = 0;
}
void CEventDelay::Start()
{
	this->elapsed = 0;
}
void CEventDelay::Update(DWORD dt)
{
	elapsed += dt;
}
bool CEventDelay::IsDone()
{
	return elapsed >= delayTime;
}

// CEventWaitForSound
CEventWaitForSound::CEventWaitForSound(size_t trackId)
{
	this->trackId = trackId;
}
void CEventWaitForSound::Start()
{
}
void CEventWaitForSound::Update(DWORD dt)
{
}
bool CEventWaitForSound::IsDone()
{
	return !CSoundManager::GetInstance()->IsPlaying(trackId);
}

// CEventAction
CEventAction::CEventAction(std::function<void()> action)
{
	this->action = action;
	this->done = false;
}
void CEventAction::Start()
{
	if (action != nullptr) {
		action();
	}
	done = true;
}
void CEventAction::Update(DWORD dt)
{
}
bool CEventAction::IsDone()
{
	return done;
}

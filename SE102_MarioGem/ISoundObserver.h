#pragma once

class ISoundObserver
{
public:
	virtual void OnSoundEvent(int eventId) = 0;
	virtual ~ISoundObserver() {}
};

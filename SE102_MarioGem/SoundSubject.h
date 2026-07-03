#pragma once

#include <list>

#include "ISoundObserver.h"

class CSoundSubject
{
	static CSoundSubject* __instance;

	std::list<ISoundObserver*> observers;

	CSoundSubject();

public:
	void Attach(ISoundObserver* observer);
	void Detach(ISoundObserver* observer);
	void Notify(int eventId);

	static CSoundSubject* GetInstance();
};

#include "SoundSubject.h"

CSoundSubject* CSoundSubject::__instance = NULL;

CSoundSubject::CSoundSubject()
{
}

CSoundSubject* CSoundSubject::GetInstance()
{
	if (__instance == NULL) __instance = new CSoundSubject();
	return __instance;
}

void CSoundSubject::Attach(ISoundObserver* observer)
{
	if (observer != NULL) observers.push_back(observer);
}

void CSoundSubject::Detach(ISoundObserver* observer)
{
	observers.remove(observer);
}

void CSoundSubject::Notify(int eventId)
{
	for (ISoundObserver* observer : observers)
		observer->OnSoundEvent(eventId);
}

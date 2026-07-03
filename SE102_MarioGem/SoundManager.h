#pragma once

#include <unordered_map>
#include <string>

#include "ISoundObserver.h"

typedef struct ma_engine ma_engine;
typedef struct ma_sound ma_sound;

class CSoundManager : public ISoundObserver
{
	static CSoundManager* __instance;

	ma_engine* engine;
	ma_sound* currentMusic;

	std::unordered_map<int, std::string> sounds;

	float masterVolume;

	CSoundManager();

public:
	void Add(int id, std::string path);

	void PlaySfx(int soundId);
	void PlayMusic(int soundId);
	void StopMusic();
	void StopAll();
	void SetVolume(float volume);
	float GetVolume() { return masterVolume; }

	virtual void OnSoundEvent(int eventId);

	void Shutdown();

	static CSoundManager* GetInstance();
};

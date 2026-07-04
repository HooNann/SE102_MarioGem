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
	int currentMusicId;

	std::unordered_map<int, std::string> sounds;

	struct TrackedSound {
		ma_sound* sound;
	};
	std::unordered_map<size_t, TrackedSound> trackedSounds;
	size_t nextTrackId;

	float masterVolume;

	CSoundManager();

public:
	void Add(int id, std::string path);

	void PlaySfx(int soundId);
	size_t PlayTrackedSfx(int soundId);
	bool IsPlaying(size_t trackId);
	void Update();

	void PlayMusic(int soundId);
	void StopMusic();
	void StopAll();
	void SetVolume(float volume);
	float GetVolume() { return masterVolume; }

	virtual void OnSoundEvent(int eventId);

	void Shutdown();

	static CSoundManager* GetInstance();
};

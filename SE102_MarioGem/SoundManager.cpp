#include "SoundManager.h"
#include "SoundSubject.h"
#include "SoundEvents.h"
#include "debug.h"

#include "lib/miniaudio/miniaudio.h"

CSoundManager* CSoundManager::__instance = NULL;

CSoundManager* CSoundManager::GetInstance()
{
	if (__instance == NULL) __instance = new CSoundManager();
	return __instance;
}

CSoundManager::CSoundManager()
{
	currentMusic = NULL;
	currentMusicId = -1;
	masterVolume = 1.0f;
	nextTrackId = 1;

	engine = new ma_engine();

	ma_result result = ma_engine_init(NULL, engine);
	if (result != MA_SUCCESS)
	{
		DebugOut(L"[ERROR] ma_engine_init failed: %d\n", result);
		delete engine;
		engine = NULL;
	}
	else
	{
		ma_engine_set_volume(engine, masterVolume);
	}

	CSoundSubject::GetInstance()->Attach(this);
}

void CSoundManager::Add(int id, std::string path)
{
	sounds[id] = path;
}

void CSoundManager::PlaySfx(int soundId)
{
	if (engine == NULL) return;

	auto it = sounds.find(soundId);
	if (it == sounds.end())
	{
		DebugOut(L"[ERROR] Sound Id %d not found\n", soundId);
		return;
	}

	ma_engine_play_sound(engine, it->second.c_str(), NULL);
}

size_t CSoundManager::PlayTrackedSfx(int soundId)
{
	if (engine == NULL) return 0;

	auto it = sounds.find(soundId);
	if (it == sounds.end())
	{
		DebugOut(L"[ERROR] Sound Id %d not found\n", soundId);
		return 0;
	}

	ma_sound* s = new ma_sound();
	ma_result result = ma_sound_init_from_file(engine, it->second.c_str(), 0, NULL, NULL, s);
	if (result != MA_SUCCESS)
	{
		delete s;
		return 0;
	}
	
	ma_sound_start(s);
	
	size_t id = nextTrackId++;
	trackedSounds[id] = { s };
	return id;
}

bool CSoundManager::IsPlaying(size_t trackId)
{
	auto it = trackedSounds.find(trackId);
	if (it == trackedSounds.end()) return false;
	
	return !ma_sound_at_end(it->second.sound);
}

void CSoundManager::Update()
{
	for (auto it = trackedSounds.begin(); it != trackedSounds.end(); )
	{
		if (ma_sound_at_end(it->second.sound))
		{
			ma_sound_uninit(it->second.sound);
			delete it->second.sound;
			it = trackedSounds.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CSoundManager::PlayMusic(int soundId)
{
	if (engine == NULL) return;
	if (currentMusic != NULL && currentMusicId == soundId) return;

	auto it = sounds.find(soundId);
	if (it == sounds.end())
	{
		DebugOut(L"[ERROR] Sound Id %d not found\n", soundId);
		return;
	}

	StopMusic();

	currentMusic = new ma_sound();

	ma_result result = ma_sound_init_from_file(engine, it->second.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, currentMusic);
	if (result != MA_SUCCESS)
	{
		DebugOut(L"[ERROR] ma_sound_init_from_file failed: %d\n", result);
		delete currentMusic;
		currentMusic = NULL;
		return;
	}

	ma_sound_set_looping(currentMusic, MA_TRUE);
	ma_sound_start(currentMusic);
	currentMusicId = soundId;
}

void CSoundManager::StopMusic()
{
	if (currentMusic != NULL)
	{
		ma_sound_stop(currentMusic);
		ma_sound_uninit(currentMusic);
		delete currentMusic;
		currentMusic = NULL;
		currentMusicId = -1;
	}
}

void CSoundManager::StopAll()
{
	if (engine == NULL) return;

	StopMusic();
	ma_engine_stop(engine);
	ma_engine_start(engine);
}

void CSoundManager::SetVolume(float volume)
{
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;

	masterVolume = volume;
	if (engine != NULL) ma_engine_set_volume(engine, masterVolume);
}

void CSoundManager::OnSoundEvent(int eventId)
{
	switch (eventId)
	{
	case EVENT_JUMP:            PlaySfx(SND_JUMP); break;
	case EVENT_COIN:            PlaySfx(SND_COIN); break;
	case EVENT_POWERUP:         PlaySfx(SND_POWERUP); break;
	case EVENT_STOMP:           PlaySfx(SND_STOMP); break;
	case EVENT_KICK:            PlaySfx(SND_KICK); break;
	case EVENT_BLOCK:           PlaySfx(SND_BLOCK); break;
	case EVENT_FIREBALL:        PlaySfx(SND_FIREBALL); break;
	case EVENT_1UP:             PlaySfx(SND_1UP); break;
	case EVENT_POWERDOWN:       PlaySfx(SND_POWERDOWN); break;
	case EVENT_PAUSE:           PlaySfx(SND_PAUSE); break;
	case EVENT_COURSE_CLEAR:    PlaySfx(SND_COURSE_CLEAR); break;
	case EVENT_GAME_OVER:       PlaySfx(SND_GAME_OVER); break;
	case EVENT_LEVEL_START:     PlaySfx(SND_LEVEL_START); break;
	case EVENT_MUSIC_OVERWORLD: PlayMusic(MUS_OVERWORLD); break;
	case EVENT_MUSIC_WORLDMAP:  PlayMusic(MUS_WORLDMAP); break;
	case EVENT_MUSIC_FORTRESS:  PlayMusic(MUS_FORTRESS); break;
	case EVENT_MUSIC_STOP:      StopMusic(); break;
	case EVENT_VOLUME_UP:       SetVolume(masterVolume + 0.1f); break;
	case EVENT_VOLUME_DOWN:     SetVolume(masterVolume - 0.1f); break;
	case EVENT_BREAK:           PlaySfx(SND_BREAK); break;
	case EVENT_TANOOKI:         PlaySfx(SND_TANOOKI); break;
	case EVENT_TWIRL:           PlaySfx(SND_TWIRL); break;
	case EVENT_CANNON:          PlaySfx(SND_CANNON); break;
	case EVENT_PLAYER_DOWN:     PlaySfx(SND_PLAYER_DOWN); break;
	case EVENT_MAP_MOVE:        PlaySfx(SND_MAP_MOVE); break;
	}
}

void CSoundManager::Shutdown()
{
	StopMusic();
	
	for (auto& pair : trackedSounds)
	{
		ma_sound_stop(pair.second.sound);
		ma_sound_uninit(pair.second.sound);
		delete pair.second.sound;
	}
	trackedSounds.clear();

	if (engine != NULL)
	{
		ma_engine_uninit(engine);
		delete engine;
		engine = NULL;
	}
}

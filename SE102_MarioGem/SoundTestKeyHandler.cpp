#include "SoundTestKeyHandler.h"

#include "Game.h"
#include "SoundSubject.h"
#include "SoundEvents.h"

void CSoundTestKeyHandler::OnKeyDown(int KeyCode)
{
	CSoundSubject* subject = CSoundSubject::GetInstance();

	switch (KeyCode)
	{
	case DIK_1: subject->Notify(EVENT_JUMP); break;
	case DIK_2: subject->Notify(EVENT_COIN); break;
	case DIK_3: subject->Notify(EVENT_POWERUP); break;
	case DIK_4: subject->Notify(EVENT_STOMP); break;
	case DIK_5: subject->Notify(EVENT_KICK); break;
	case DIK_6: subject->Notify(EVENT_BLOCK); break;
	case DIK_7: subject->Notify(EVENT_FIREBALL); break;
	case DIK_8: subject->Notify(EVENT_1UP); break;
	case DIK_9: subject->Notify(EVENT_POWERDOWN); break;
	case DIK_Q: subject->Notify(EVENT_PAUSE); break;
	case DIK_W: subject->Notify(EVENT_COURSE_CLEAR); break;
	case DIK_E: subject->Notify(EVENT_GAME_OVER); break;
	case DIK_R: subject->Notify(EVENT_LEVEL_START); break;
	case DIK_M: subject->Notify(EVENT_MUSIC_OVERWORLD); break;
	case DIK_N: subject->Notify(EVENT_MUSIC_WORLDMAP); break;
	case DIK_S: subject->Notify(EVENT_MUSIC_STOP); break;
	case DIK_EQUALS: subject->Notify(EVENT_VOLUME_UP); break;
	case DIK_MINUS: subject->Notify(EVENT_VOLUME_DOWN); break;
	}
}

void CSoundTestKeyHandler::OnKeyUp(int KeyCode)
{
}

void CSoundTestKeyHandler::KeyState(BYTE* states)
{
}

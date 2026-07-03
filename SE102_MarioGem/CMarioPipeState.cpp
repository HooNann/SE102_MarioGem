#include "CMarioPipeState.h"

#include "Game.h"
#include "CMarioIdleState.h"
#include "Mario.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

CMarioPipeState::CMarioPipeState(CPipe* pipe)
{
	this->pipe = pipe;
	phaseStartTime = 0;
	exiting = false;
	waitingForTransition = false;
	phaseStartX = 0.0f;
	phaseStartY = 0.0f;
	phaseEndX = 0.0f;
	phaseEndY = 0.0f;
	targetX = 0.0f;
	targetY = 0.0f;
	entryDirection = PipeDirection::Down;
	exitDirection = PipeDirection::Up;
}

void CMarioPipeState::GetDirectionVector(PipeDirection direction, float& dx, float& dy)
{
	dx = 0.0f;
	dy = 0.0f;

	switch (direction)
	{
	case PipeDirection::Up:
		dy = -1.0f;
		break;
	case PipeDirection::Left:
		dx = -1.0f;
		break;
	case PipeDirection::Right:
		dx = 1.0f;
		break;
	case PipeDirection::Down:
	default:
		dy = 1.0f;
		break;
	}
}

void CMarioPipeState::SetPhase(CMario* mario, float startX, float startY, float endX, float endY)
{
	phaseStartX = startX;
	phaseStartY = startY;
	phaseEndX = endX;
	phaseEndY = endY;
	phaseStartTime = GetTickCount64();
	mario->SetPosition(startX, startY);
}

void CMarioPipeState::Enter(CMario* mario)
{
	mario->SetVelocityX(0.0f);
	mario->SetVelocityY(0.0f);
	mario->SetAccelerationX(0.0f);
	mario->SetAccelerationY(0.0f);
	mario->SetMaxVelocityX(0.0f);

	if (mario->IsSitting())
	{
		mario->SetSitting(false);
		mario->SetY(mario->GetY() - MARIO_SIT_HEIGHT_ADJUST);
	}

	float mx, my;
	mario->GetPosition(mx, my);

	if (pipe != nullptr)
	{
		targetX = pipe->GetTargetX();
		targetY = pipe->GetTargetY();
		entryDirection = pipe->GetEntryDirection();
		exitDirection = pipe->GetExitDirection();
	}
	else
	{
		targetX = mx;
		targetY = my;
	}

	float dx, dy;
	GetDirectionVector(entryDirection, dx, dy);
	SetPhase(mario, mx, my, mx + dx * MARIO_PIPE_TRAVEL_DISTANCE, my + dy * MARIO_PIPE_TRAVEL_DISTANCE);

	CSoundSubject::GetInstance()->Notify(EVENT_POWERDOWN);
}

void CMarioPipeState::StartExitPhase(CMario* mario)
{
	float finalX = targetX;
	float finalY = targetY;

	float dx, dy;
	GetDirectionVector(exitDirection, dx, dy);

	float startX = finalX - dx * MARIO_PIPE_TRAVEL_DISTANCE;
	float startY = finalY - dy * MARIO_PIPE_TRAVEL_DISTANCE;

	exiting = true;
	SetPhase(mario, startX, startY, finalX, finalY);

	CSoundSubject::GetInstance()->Notify(EVENT_POWERDOWN);
}

void CMarioPipeState::Update(CMario* mario, DWORD dt)
{
	mario->SetVelocityX(0.0f);
	mario->SetVelocityY(0.0f);
	mario->SetAccelerationX(0.0f);
	mario->SetAccelerationY(0.0f);

	if (waitingForTransition) return;

	DWORD phaseTime = exiting ? MARIO_PIPE_EXIT_TIME : MARIO_PIPE_ENTER_TIME;
	float progress = (float)(GetTickCount64() - phaseStartTime) / (float)phaseTime;
	if (progress > 1.0f) progress = 1.0f;

	float nx = phaseStartX + (phaseEndX - phaseStartX) * progress;
	float ny = phaseStartY + (phaseEndY - phaseStartY) * progress;
	mario->SetPosition(nx, ny);

	if (progress < 1.0f) return;

	if (!exiting)
	{
		waitingForTransition = true;
		CGame::GetInstance()->StartFadeOut(TRANSITION_FADE_OUT_DURATION_MS, true, [this, mario]() {
			StartExitPhase(mario);
			waitingForTransition = false;
			CGame::GetInstance()->StartFadeIn(TRANSITION_FADE_IN_DURATION_MS, true);
		});
	}
	else
	{
		mario->ChangeState(new CMarioIdleState());
	}
}

void CMarioPipeState::Exit(CMario* mario)
{
	mario->SetVelocityX(0.0f);
	mario->SetVelocityY(0.0f);
	mario->SetAccelerationY(MARIO_GRAVITY);
}

int CMarioPipeState::GetAnimationId(CMario* mario)
{
	switch (mario->GetLevel())
	{
	case MarioLevel::Small: return ID_ANI_MARIO_SMALL_FRONT;
	case MarioLevel::Big: return ID_ANI_MARIO_BIG_FRONT;
	case MarioLevel::Fire: return ID_ANI_MARIO_FIRE_FRONT;
	case MarioLevel::Raccoon: return ID_ANI_MARIO_RACCOON_FRONT;
	}
	return ID_ANI_MARIO_SMALL_FRONT;
}

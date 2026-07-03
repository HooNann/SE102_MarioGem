#include "MarioPipeState.h"

#include "Game.h"
#include "MarioIdleState.h"
#include "Mario.h"
#include "PlayScene.h"
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

float CMarioPipeState::GetMarioBBoxHeight(CMario* mario)
{
	float l, t, r, b;
	mario->GetBoundingBox(l, t, r, b);
	return b - t;
}

float CMarioPipeState::GetMarioBBoxWidth(CMario* mario)
{
	float l, t, r, b;
	mario->GetBoundingBox(l, t, r, b);
	return r - l;
}

float CMarioPipeState::GetTravelDistance(CMario* mario, PipeDirection direction)
{
	if (direction == PipeDirection::Up || direction == PipeDirection::Down)
		return GetMarioBBoxHeight(mario);

	return MARIO_PIPE_TRAVEL_DISTANCE;
}

void CMarioPipeState::ResolveTargetCenter(CMario* mario, float& x, float& y)
{
	x = targetX;
	y = targetY;

	float height = GetMarioBBoxHeight(mario);
	switch (exitDirection)
	{
	case PipeDirection::Up:
		y = targetY - height / 2.0f;
		break;
	case PipeDirection::Down:
		y = targetY + height / 2.0f;
		break;
	case PipeDirection::Left:
	case PipeDirection::Right:
	default:
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
	float travelDistance = GetTravelDistance(mario, entryDirection);
	SetPhase(mario, mx, my, mx + dx * travelDistance, my + dy * travelDistance);

	CSoundSubject::GetInstance()->Notify(EVENT_POWERDOWN);
}

void CMarioPipeState::StartExitPhase(CMario* mario)
{
	float finalX, finalY;
	ResolveTargetCenter(mario, finalX, finalY);

	float dx, dy;
	GetDirectionVector(exitDirection, dx, dy);
	float travelDistance = GetTravelDistance(mario, exitDirection);

	float startX = finalX - dx * travelDistance;
	float startY = finalY - dy * travelDistance;

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
			CPlayScene* scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
			if (scene != nullptr)
				scene->SyncCameraToPlayer();
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

#pragma once

#include "MarioState.h"
#include "Pipe.h"

constexpr DWORD MARIO_PIPE_ENTER_TIME = 1000;
constexpr DWORD MARIO_PIPE_EXIT_TIME = 1000;
constexpr float MARIO_PIPE_TRAVEL_DISTANCE = 32.0f;

class CMarioPipeState : public CMarioState
{
	CPipe* pipe;
	ULONGLONG phaseStartTime;
	bool exiting;
	bool waitingForTransition;
	float phaseStartX;
	float phaseStartY;
	float phaseEndX;
	float phaseEndY;
	float targetX;
	float targetY;
	PipeDirection entryDirection;
	PipeDirection exitDirection;

	void StartExitPhase(CMario* mario);
	void GetDirectionVector(PipeDirection direction, float& dx, float& dy);
	float GetMarioBBoxHeight(CMario* mario);
	float GetMarioBBoxWidth(CMario* mario);
	float GetTravelDistance(CMario* mario, PipeDirection direction);
	void ResolveTargetCenter(CMario* mario, float& x, float& y);
	void SetPhase(CMario* mario, float startX, float startY, float endX, float endY);

public:
	CMarioPipeState(CPipe* pipe);

	MarioStateID GetID() override { return MarioStateID::Pipe; }
	void Enter(CMario* mario) override;
	void Update(CMario* mario, DWORD dt) override;
	void Exit(CMario* mario) override;
	int GetAnimationId(CMario* mario) override;
};

#pragma once
#include "CMarioState.h"

class CMarioFloatState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Float; }
    void Enter(CMario* mario) override;
    void Exit(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
};

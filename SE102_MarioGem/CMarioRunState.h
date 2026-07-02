#pragma once
#include "CMarioState.h"

class CMarioRunState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Run; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
};

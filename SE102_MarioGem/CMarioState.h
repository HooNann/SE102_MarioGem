#pragma once
#include <Windows.h>
#include "AssetIDs.h"
#include "Collision.h"

class CMario;

struct Input {
    // Tạm thời giữ cấu trúc Input đơn giản để tương thích với chữ ký hàm được yêu cầu
    int KeyCode;
};

class CMarioState
{
public:
    virtual MarioStateID GetID() = 0;
    
    virtual void Enter(CMario* mario) {}
    virtual void Update(CMario* mario, DWORD dt) = 0; // Chuyên để nhảy state hoặc nhảy animation
    virtual void Exit(CMario* mario) {}
    virtual void HandleInput(CMario* mario, const Input& input) {}
    
    // Xử lý va chạm mặc định
    virtual void OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e);
    
    // Xử lý animation
    virtual int GetAnimationId(CMario* mario);
    
    virtual ~CMarioState() = default;
};

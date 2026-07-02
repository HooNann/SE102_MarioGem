#include "CMarioFloatState.h"
#include "CMarioFallState.h"
#include "Mario.h"

void CMarioFloatState::Enter(CMario* mario)
{
    // Hãm đà rơi lại (rơi chậm)
    mario->SetVelocityY(MARIO_FLOAT_SPEED_Y);
    mario->SetAccelerationY(MARIO_GRAVITY / 4); // Trọng lực rất nhỏ để lướt một nhịp ngắn
}

void CMarioFloatState::Update(CMario* mario, DWORD dt)
{
    // Sau 1 khoảng vẫy đuôi, tự động trả về rơi tự do
    // Ở đây dùng một cơ chế đơn giản là khi tốc độ rơi tăng quá một mức nhất định
    if (mario->GetVelocityY() > MARIO_FLOAT_SPEED_Y * 2)
    {
        mario->ChangeState(new CMarioFallState());
    }
}

int CMarioFloatState::GetAnimationId(CMario* mario)
{
    if (mario->GetLevel() == MarioLevel::Raccoon) return ID_ANI_MARIO_RACCOON_FLOAT;
    return ID_ANI_MARIO_SMALL_JUMP;
}

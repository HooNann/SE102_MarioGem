#include "Camera.h"
#include <algorithm>
#include "Mario.h"

CCamera *CCamera::__instance = NULL;

CCamera::CCamera() {
  cam_x = 0;
  cam_y = 0;
  width = 320;
  height = 240;
  max_x = 0;
  max_y = 0;
  target = NULL;
}

CCamera *CCamera::GetInstance() {
  if (__instance == NULL)
    __instance = new CCamera();
  return __instance;
}

void CCamera::Update() {
  if (target != NULL && target->GetState() != static_cast<int>(MarioState::Die)) {
    float tx, ty;
    target->GetPosition(tx, ty);

    // Căn giữa camera vào target
    cam_x = tx - width / 2;
    cam_y = ty - height / 2;
  }

  // Clamp biên trái/phải
  if (cam_x < 0)
    cam_x = 0;
  if (max_x > 0 && cam_x > max_x - width)
    cam_x = (max_x > width) ? (max_x - width) : 0;

  // Clamp biên trên/dưới
  if (cam_y < 0)
    cam_y = 0;
  if (max_y > 0 && cam_y > max_y - height)
    cam_y = (max_y > height) ? (max_y - height) : 0;
}

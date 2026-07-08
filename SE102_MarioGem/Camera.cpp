#include "Camera.h"
#include <algorithm>
#include "Mario.h"
#include "AssetIDs.h"
#include "MarioState.h"

CCamera *CCamera::__instance = NULL;

CCamera::CCamera() {
  cam_x = 0;
  cam_y = 0;
  width = 320;
  height = 240;
  min_x = 0;
  min_y = 0;
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
  CMario* mario = dynamic_cast<CMario*>(target);
  if (mario != NULL && mario->currentState && mario->currentState->GetID() != MarioStateID::Dead
      && !mario->IsTransforming()) {
    float tx, ty;
    target->GetPosition(tx, ty);

    cam_x = tx - width / 2;
    cam_y = ty - height / 2;
  }

  if (cam_x < min_x)
    cam_x = min_x;
  if (max_x > min_x && cam_x > max_x - width)
    cam_x = (max_x - min_x > width) ? (max_x - width) : min_x;

  if (cam_y < min_y)
    cam_y = min_y;
  if (max_y > min_y && cam_y > max_y - height)
    cam_y = (max_y - min_y > height) ? (max_y - height) : min_y;
}

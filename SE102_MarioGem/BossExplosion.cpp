#include "BossExplosion.h"
#include "Coin.h"

CBossExplosion::CBossExplosion(float x, float y, float vx, float vy)
    : CGameObject(x, y) {
  this->vx = vx;
  this->vy = vy;
  this->startTime = GetTickCount64();
}

void CBossExplosion::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects) {
  x += vx * dt;
  y += vy * dt;

  if (GetTickCount64() - startTime > 1000) {
    this->Delete();
  }
}

void CBossExplosion::Render() {
  CAnimations::GetInstance()->Get(90000)->Render(x, y);
}

void CBossExplosion::GetBoundingBox(float &l, float &t, float &r, float &b) {
  l = 0;
  t = 0;
  r = 0;
  b = 0;
}

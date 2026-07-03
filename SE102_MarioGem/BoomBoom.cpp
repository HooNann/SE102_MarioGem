#include "BoomBoom.h"
#include "AssetIDs.h"
#include "debug.h"
#include <math.h>
#include "PlayScene.h"
#include "BossExplosion.h"
#include "MarioDeadState.h"

#define ID_ANI_BOOMBOOM_WALKING 36000
#define ID_ANI_BOOMBOOM_HIDING 36004

CBoomBoom::CBoomBoom(float x, float y) : CGameObject(x, y)
{
	this->ax = 0;
	this->ay = BOOMBOOM_GRAVITY;
	this->hp = 3;
	this->untouchable = 0;
	this->untouchable_start = -1;
	this->isOnPlatform = false;
	this->last_jump_time = 0;
	SetState(BOOMBOOM_STATE_HIDING);
}

void CBoomBoom::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (state == BOOMBOOM_STATE_DIE)
	{
		left = x - BOOMBOOM_BBOX_WIDTH / 2;
		top = y - BOOMBOOM_BBOX_HEIGHT_DIE / 2;
		right = left + BOOMBOOM_BBOX_WIDTH;
		bottom = top + BOOMBOOM_BBOX_HEIGHT_DIE;
	}
	else if (state == BOOMBOOM_STATE_HIDING)
	{
		left = x - BOOMBOOM_BBOX_WIDTH / 2;
		top = y - BOOMBOOM_BBOX_HEIGHT_HIDING / 2;
		right = left + BOOMBOOM_BBOX_WIDTH;
		bottom = top + BOOMBOOM_BBOX_HEIGHT_HIDING;
	}
	else // HURT and WALKING use the same normal BBOX
	{
		left = x - BOOMBOOM_BBOX_WIDTH / 2;
		top = y - BOOMBOOM_BBOX_HEIGHT / 2;
		right = left + BOOMBOOM_BBOX_WIDTH;
		bottom = top + BOOMBOOM_BBOX_HEIGHT;
	}
}

void CBoomBoom::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void CBoomBoom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
		if (e->ny < 0) {
			isOnPlatform = true;
		}
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

#include "PlayScene.h"
#include "Mario.h"

void CBoomBoom::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	CMario *mario = (CMario *)((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	//Tracking Mario when walking
	if (state == BOOMBOOM_STATE_WALKING && mario != NULL) {
		if (mario->currentState && mario->currentState->GetID() == MarioStateID::Dead) {
			vx = 0; // Stop moving
		} else {
			float mx, my;
			mario->GetPosition(mx, my);
			if (mx < this->x) {
				vx = -BOOMBOOM_WALKING_SPEED;
			} else {
				vx = BOOMBOOM_WALKING_SPEED;
			}

			// Jumping when Mario is close 
			if (isOnPlatform && abs(mx - this->x) < 48.0f) {
				if (GetTickCount64() - last_jump_time > BOOMBOOM_JUMP_COOLDOWN) {
					vy = -BOOMBOOM_JUMP_SPEED;
					isOnPlatform = false;
					last_jump_time = GetTickCount64();
				}
			}
		}
	}

	if (untouchable) {
		ULONGLONG elapsed = GetTickCount64() - untouchable_start;
		if (state == BOOMBOOM_STATE_HIDING) {
			if (mario != NULL) {
				float mx, my;
				mario->GetPosition(mx, my);
				if (abs(mx - this->x) <= 80.0f) {
					untouchable = 0;
					untouchable_start = 0;
					SetState(BOOMBOOM_STATE_WALKING);
				}
			}
		} else if (state == BOOMBOOM_STATE_HURT && elapsed > BOOMBOOM_HURT_TIME) {
			SetState(BOOMBOOM_STATE_HIDING);
		}
	}

	if (state == BOOMBOOM_STATE_DIE) {
		if (GetTickCount64() - untouchable_start > 3000) {
			CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
			int num_stars = 8;
			for (int i = 0; i < num_stars; i++) {
				float angle = i * (3.14159265f * 2.0f / num_stars);
				float speed = 0.15f;
				float star_vx = speed * cos(angle);
				float star_vy = speed * sin(angle);
				CBossExplosion* star = new CBossExplosion(x, y, star_vx, star_vy);
				scene->QueueSpawn(star); 
			}
			Delete();
			return;
		}
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);

	// Manual collision check with Mario
	if (state != BOOMBOOM_STATE_DIE) {
		CMario *mario = (CMario *)((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		if (mario != NULL && mario->currentState && mario->currentState->GetID() != MarioStateID::Dead) {
			float ml, mt, mr, mb;
			mario->GetBoundingBox(ml, mt, mr, mb);
			
			float bl, bt, br, bb;
			GetBoundingBox(bl, bt, br, bb);

			// AABB check
			if (ml < br && mr > bl && mt < bb && mb > bt) {
				float mx, my, mvx, mvy;
				mario->GetPosition(mx, my);
				mario->GetSpeed(mvx, mvy);

				// If Boom Boom is in Spiked Shell, Mario dies from ANY angle
				if (state == BOOMBOOM_STATE_HIDING) {
					mario->ChangeState(new CMarioDeadState());
				} else {
					// Mario is falling onto Boom Boom (normal state)
					if (mvy > 0 && mb < bb) {
						if (!untouchable) {
							hp--;
							mario->SetSpeed(mvx, -MARIO_JUMP_DEFLECT_SPEED);
							if (hp <= 0) {
								SetState(BOOMBOOM_STATE_DIE);
							} else {
								SetState(BOOMBOOM_STATE_HURT);
							}
						}
					} else {
						// Mario touches from side or bottom
						if (!untouchable) {
							mario->ChangeState(new CMarioDeadState());
						}
					}
				}
			}
		}
	}
}

void CBoomBoom::Render()
{
	int aniId = ID_ANI_BOOMBOOM_WALKING;
	
	if (state == BOOMBOOM_STATE_WALKING) {
		if (!isOnPlatform) aniId = 36001; // Jumping
	}
	else if (state == BOOMBOOM_STATE_DIE) {
		aniId = 36006; // Die/Defeated
	}
	else if (state == BOOMBOOM_STATE_HIDING) {
		aniId = 36004; // Hiding in spiked shell
	}
	else if (state == BOOMBOOM_STATE_HURT) {
		aniId = 36005; // Taking damage
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	// RenderBoundingBox();
}

void CBoomBoom::SetState(int state)
{
	int oldState = this->state;
	CGameObject::SetState(state);
	switch (state)
	{
	case BOOMBOOM_STATE_DIE:
	{
		y += (BOOMBOOM_BBOX_HEIGHT - BOOMBOOM_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		untouchable_start = GetTickCount64();
		break;
	}
	case BOOMBOOM_STATE_HIDING:
		y += (BOOMBOOM_BBOX_HEIGHT - BOOMBOOM_BBOX_HEIGHT_HIDING) / 2;
		vx = 0;
		untouchable = 1;
		untouchable_start = GetTickCount64();
		break;
	case BOOMBOOM_STATE_HURT:
		vx = 0;
		untouchable = 1;
		untouchable_start = GetTickCount64();
		break;
	case BOOMBOOM_STATE_WALKING:
		if (oldState == BOOMBOOM_STATE_HIDING) {
			y -= (BOOMBOOM_BBOX_HEIGHT - BOOMBOOM_BBOX_HEIGHT_HIDING) / 2;
		}
		break;
	}
}

LPGAMEOBJECT CBoomBoom::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());
	return new CBoomBoom(x, y);
}

#include "FireBall.h"
#include "Goomba.h" // Include các class quái vật vào để xử lý va chạm
#include "Koopas.h"

CFireBall::CFireBall(float x, float y, int direction) : CGameObject(x, y)
{
	this->nx = direction;
	this->vx = direction * FIREBALL_SPEED_X; // Bay theo hướng mặt của Mario
	this->vy = 0;
	this->ay = FIREBALL_GRAVITY;             // Bị trọng lực kéo xuống
}

void CFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;

	// SweptAABB không phát hiện vật thể đã chồng lấn sẵn (VD: spawn sát ống nước),
	// nên phải tự kiểm tra chồng lấn với vật chặn và cho quả cầu nổ tại chỗ
	if (coObjects != NULL)
	{
		float l, t, r, b;
		GetBoundingBox(l, t, r, b);

		for (size_t i = 0; i < coObjects->size(); i++)
		{
			LPGAMEOBJECT obj = coObjects->at(i);
			if (obj->IsDeleted() || !obj->IsBlocking()) continue;
			if (obj->IsDirectionColliable((float)-nx, 0) != 1) continue;

			float sl, st, sr, sb;
			obj->GetBoundingBox(sl, st, sr, sb);

			if (l < sr && r > sl && t < sb && b > st)
			{
				this->Delete();
				return;
			}
		}
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFireBall::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CFireBall::OnCollisionWith(LPCOLLISIONEVENT e)
{
	// 1. VA CHẠM VỚI ĐỊA HÌNH CỨNG (Nền đất, Ống nước, Gạch...)
	if (e->obj->IsBlocking())
	{
		if (e->ny < 0) // Chạm mặt trên của sàn đất
		{
			vy = -FIREBALL_BOUNCE_SPEED_Y; // Đảo ngược vận tốc Y để quả cầu nảy tưng lên
		}
		else if (e->nx != 0) // Húc vào tường đứng/cạnh bên bệ đá
		{
			this->Delete(); // Quả cầu lửa nổ tung và tự hủy
		}
	}

	// 2. VA CHẠM VỚI KẺ ĐỊCH NẤM GOOMBA
	if (dynamic_cast<CGoomba*>(e->obj))
	{
		CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

		// Bắt quái chuyển sang trạng thái chết do lửa (lộn ngược rơi khỏi map)
		// Bạn cần định nghĩa trạng thái này bên class Goomba nếu muốn giống game gốc
		goomba->SetState(GoombaState::Die);

		this->Delete(); // Đập trúng quái là quả cầu lửa biến mất ngay
	}

	if (dynamic_cast<CKoopas*>(e->obj))
	{
		CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
		koopas->SetState(KoopasState::Die_KnockOut);
		this->Delete();
	}
}

void CFireBall::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_FIREBALL)->Render(x, y, nx);
}

void CFireBall::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FIREBALL_BBOX_WIDTH / 2;
	top = y - FIREBALL_BBOX_HEIGHT / 2;
	right = left + FIREBALL_BBOX_WIDTH;
	bottom = top + FIREBALL_BBOX_HEIGHT;
}

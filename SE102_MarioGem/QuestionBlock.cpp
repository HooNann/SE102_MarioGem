#include "QuestionBlock.h"
#include "Animations.h"

CQuestionBlock::CQuestionBlock(float x, float y, int itemType) : CGameObject(x, y)
{
	this->itemType = itemType;
	this->isAlive = 1;			// Thuở sơ khai khối gạch luôn còn nguyên
	this->startY = y;			// Ghi nhớ tọa độ sàn ban đầu
	this->bumpDirection = 0;	// Chưa bị ai húc nên đứng yên

	this->state = QUESTION_BLOCK_STATE_ALIVE;
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// LOGIC XỬ LÝ HIỆU ỨNG NẢY (BUMP EFFECT) KHI BỊ HÚC
	if (bumpDirection == -1) // Đang nảy đi lên
	{
		y -= QUESTION_BLOCK_BUMP_SPEED * dt;
		if (startY - y >= QUESTION_BLOCK_BUMP_MAX_DIST)
		{
			y = startY - QUESTION_BLOCK_BUMP_MAX_DIST;
			bumpDirection = 1; // Đạt độ cao cực đại, bắt đầu rơi xuống lại
		}
	}
	else if (bumpDirection == 1) // Đang rơi xuống lại vị trí cũ
	{
		y += QUESTION_BLOCK_BUMP_SPEED * dt;
		if (y >= startY)
		{
			y = startY;			// Trả về chính xác tọa độ sàn ban đầu, chống lún
			bumpDirection = 0;	// Kết thúc chu kỳ nảy
			this->SetState(QUESTION_BLOCK_STATE_EMPTY); // Chuyển hẳn sang khối gạch rỗng
		}
	}
}

void CQuestionBlock::SetState(int state)
{
	switch (state)
	{
	case QUESTION_BLOCK_STATE_HIT:
		if (isAlive)
		{
			isAlive = 0;		// Tước đoạt phần thưởng bên trong
			bumpDirection = -1;	// Ra lệnh cho khối gạch bắt đầu chu kỳ nảy lên
		}
		break;
	}

	CGameObject::SetState(state);
}

void CQuestionBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	// Khối gạch dấu hỏi chuẩn trong Mario 3 thường có kích thước 16x16 pixel
	l = x - 16.0f / 2;
	t = y - 16.0f / 2;
	r = l + 16.0f;
	b = t + 16.0f;
}

void CQuestionBlock::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	// Cấu hình ID Animation tương ứng khớp với file dữ liệu cấu hình .txt bên ngoài
	if (state == QUESTION_BLOCK_STATE_EMPTY)
	{
		aniId = 21011; // ID_ANI của khối gạch sắt nâu rỗng chết cứng
	}
	else
	{
		aniId = 21010; // ID_ANI của khối gạch nhấp nháy dấu hỏi vàng
	}

	animations->Get(aniId)->Render(x, y);
}
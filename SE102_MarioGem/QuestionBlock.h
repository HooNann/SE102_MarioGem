#pragma once

#include "GameObject.h"

// Định nghĩa các trạng thái của khối gạch dấu hỏi
#define QUESTION_BLOCK_STATE_ALIVE	100		// Khối gạch còn nguyên, nhấp nháy màu vàng
#define QUESTION_BLOCK_STATE_HIT	200		// Trạng thái bị húc (đang nảy lên)
#define QUESTION_BLOCK_STATE_EMPTY	300		// Khối gạch trống rỗng (biến thành khối sắt nâu)

// Định nghĩa các loại vật phẩm chứa bên trong khối gạch
#define QUESTION_BLOCK_ITEM_FLOWER		1
#define QUESTION_BLOCK_ITEM_LEAF		2

// Định nghĩa các thông số vật lý cho hiệu ứng nảy (Bump Effect)
#define QUESTION_BLOCK_BUMP_SPEED	0.15f
#define QUESTION_BLOCK_BUMP_MAX_DIST	8.0f	

class CQuestionBlock : public CGameObject
{
protected:
	int itemType;				// Loại vật phẩm chứa bên trong gạch
	int isAlive;				// Khối gạch còn phần thưởng hay không

	float startY;				// Lưu lại tọa độ Y gốc để làm hiệu ứng nảy
	int bumpDirection;			// -1: đang nảy lên, 1: đang rơi về vị trí cũ, 0: đứng yên

public:
	CQuestionBlock(float x, float y, int itemType);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void SetState(int state);

	virtual int IsBlocking() { return 1; }

	int GetItemType() { return itemType; }
	int IsAlive() { return isAlive; }

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void ReleaseItem();
};
typedef CQuestionBlock* LPQUESTIONBLOCK;
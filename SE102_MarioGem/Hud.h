#pragma once
#include "Texture.h"

class CMario;

// Texture id registered in main-config.txt [TEXTURES] -> Assets\Textures\All\ui.png
constexpr int ID_TEX_UI = 30;

/*
	CHud — vẽ thanh thông tin (score, coins, time, lives, world, P-meter) kiểu SMB3,
	dán cố định ở đáy màn hình (screen-space, KHÔNG bị camera dời).

	Vẽ trực tiếp bằng CGame::Draw(...) với RECT nguồn trỏ vào ui.png nên không cần
	đi qua pipeline sprite/animation .txt. Mọi toạ độ đích là TÂM sprite (theo
	quy ước của framework: D3DX10 sprite vẽ quanh tâm).
*/
class CHud
{
	LPTEXTURE tex;

	// Vẽ 1 vùng con của ui.png, tâm tại (centerX, centerY) tính từ góc trên-trái màn hình.
	void DrawRegion(float centerX, float centerY, int srcLeft, int srcTop, int srcW, int srcH);

	// Vẽ 1 glyph font (theo top-left trên màn hình). Trả về true nếu có vẽ (ký tự hợp lệ).
	bool DrawGlyph(char c, float screenX, float screenY);

	// Vẽ chuỗi (0-9, A-Z, space, '-') bắt đầu tại top-left (x, y).
	void DrawString(const char* s, float x, float y);

	// Vẽ số nguyên, canh phải sao cho mép phải chữ số cuối nằm tại rightX (top-left y).
	void DrawNumber(int value, float rightX, float y, int minDigits);

	// Vẽ thanh P-meter: 'P' + số đoạn sáng tỉ lệ theo pmeter (0..MARIO_PMETER_MAX).
	void DrawPMeter(int pmeter, float x, float y);

public:
	CHud();
	void Render(CMario* mario, int timeLeft, const char* world);
};

#include "Sprite.h"

#include "Game.h"
#include "Camera.h"

CSprite::CSprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;

	float texWidth = (float)tex->getWidth();
	float texHeight = (float)tex->getHeight();

	// Set the sprite�s shader resource view
	sprite.pTexture = tex->getShaderResourceView();

	sprite.TexCoord.x = this->left / texWidth;
	sprite.TexCoord.y = this->top / texHeight;

	int spriteWidth = (this->right - this->left + 1);
	int spriteHeight = (this->bottom - this->top + 1);

	sprite.TexSize.x = spriteWidth / texWidth;
	sprite.TexSize.y = spriteHeight / texHeight;

	sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	sprite.TextureIndex = 0;

	D3DXMatrixScaling(&this->matScaling, (FLOAT)spriteWidth, (FLOAT)spriteHeight, 1.0f);
}

void CSprite::Draw(float x, float y, int nx, int ny)
{
	CGame* g = CGame::GetInstance();
	float cx, cy;
	CCamera::GetInstance()->GetCamPos(cx, cy);

	cx = (FLOAT)floor(cx);
	cy = (FLOAT)floor(cy);

	D3DXMATRIX matTranslation;
	
	x = (FLOAT)floor(x);
	y = (FLOAT)floor(y);

	int spriteWidth = (this->right - this->left + 1);
	int spriteHeight = (this->bottom - this->top + 1);

	float scaleX = (FLOAT)spriteWidth;
	float scaleY = (FLOAT)spriteHeight;

	if (nx < 0)
	{
		scaleX = -scaleX;
		x += spriteWidth;
	}
	if (ny < 0)
	{
		scaleY = -scaleY;
		y -= spriteHeight;
	}

	D3DXMATRIX finalScaling;
	D3DXMatrixScaling(&finalScaling, scaleX, scaleY, 1.0f);

	D3DXMatrixTranslation(&matTranslation, x - cx, g->GetBackBufferHeight() - y + cy, 0.1f);

	this->sprite.matWorld = (finalScaling * matTranslation);

	g->GetSpriteHandler()->DrawSpritesImmediate(&sprite, 1, 0, 0);
}



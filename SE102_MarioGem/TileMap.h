#pragma once

#include <vector>
#include <string>
#include "Texture.h"

using namespace std;

// Thông tin của một Tileset (một tấm hình chứa các viên gạch)
struct TilesetInfo
{
	int firstGid;		// ID bắt đầu của tileset này trong Tiled
	int tileWidth;		// Chiều rộng 1 tile (pixel)
	int tileHeight;		// Chiều cao 1 tile (pixel)
	int columns;		// Số cột gạch trên tấm hình
	int tileCount;		// Tổng số tile trong tileset
	int imageWidth;		// Chiều rộng tấm hình (pixel)
	int imageHeight;	// Chiều cao tấm hình (pixel)
	wstring imagePath;	// Đường dẫn tới file hình ảnh
	LPTEXTURE texture;	// Con trỏ texture đã load vào DirectX
};

class CMap
{
	int width;			// Số cột của map (tính bằng tile)
	int height;			// Số hàng của map (tính bằng tile)
	int tileWidth;		// Chiều rộng 1 tile (pixel)
	int tileHeight;		// Chiều cao 1 tile (pixel)

	vector<vector<int>> mapLayers;		// Mảng 2 chiều chứa ID các tile của tất cả các layer
	vector<TilesetInfo> tilesets;		// Danh sách các tileset

public:
	CMap();
	~CMap();

	// Đọc file .json xuất ra từ Tiled Map Editor
	// jsonPath: đường dẫn tới file .json 
	// basePath: thư mục chứa file .json (để tìm file hình ảnh tương đối)
	void LoadJSON(LPCWSTR jsonPath, LPCWSTR basePath);

	// Vẽ toàn bộ tile map lên màn hình (có tính camera)
	void Render();

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetTileWidth() { return tileWidth; }
	int GetTileHeight() { return tileHeight; }

	// Tìm tileset phù hợp cho một tile ID (dựa vào firstGid)
	TilesetInfo* GetTilesetByGid(int gid);
};

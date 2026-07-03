#include "TileMap.h"
#include "Game.h"
#include "Textures.h"
#include "debug.h"
#include "Camera.h"
#include "json.hpp"

#include <fstream>
#include <algorithm>

using json = nlohmann::json;

CTileMap::CTileMap()
{
	width = 0;
	height = 0;
	tileWidth = 0;
	tileHeight = 0;
}

CTileMap::~CTileMap()
{
	mapLayers.clear();
	tilesets.clear();
}

void CTileMap::LoadJSON(LPCWSTR jsonPath, LPCWSTR basePath)
{
	// Chuyển đường dẫn wide-string sang string thường để đọc file
	// (nlohmann/json cần ifstream mở bằng wide-string hoặc string)
	ifstream f(jsonPath);
	if (!f.is_open())
	{
		DebugOut(L"[ERROR] Cannot open map JSON file: %s\n", jsonPath);
		return;
	}

	json j;
	f >> j;
	f.close();

	// Đọc thông số chung của map
	width = j["width"];
	height = j["height"];
	tileWidth = j["tilewidth"];
	tileHeight = j["tileheight"];

	DebugOut(L"[INFO] Map size: %d x %d tiles, tile size: %d x %d\n",
		width, height, tileWidth, tileHeight);


	for (auto& ts : j["tilesets"])
	{
		TilesetInfo info;
		info.firstGid = ts["firstgid"];

		if (ts.contains("image"))
		{
			// Tileset được nhúng trực tiếp (embedded) trong file map
			info.columns = ts["columns"];
			info.tileCount = ts["tilecount"];
			info.tileWidth = ts["tilewidth"];
			info.tileHeight = ts["tileheight"];
			info.imageWidth = ts["imagewidth"];
			info.imageHeight = ts["imageheight"];

			// Chuyển đường dẫn hình ảnh từ string sang wstring
			string imgPath = ts["image"];
			wstring wImgPath(imgPath.begin(), imgPath.end());

			// Ghép basePath + đường dẫn tương đối của hình ảnh
			wstring fullPath = wstring(basePath) + L"\\" + wImgPath;
			info.imagePath = fullPath;

			// Load texture vào DirectX
			info.texture = CGame::GetInstance()->LoadTexture(fullPath.c_str());

			if (info.texture == NULL)
			{
				DebugOut(L"[ERROR] Failed to load tileset texture: %s\n", fullPath.c_str());
			}
			else
			{
				DebugOut(L"[INFO] Loaded tileset: firstGid=%d, columns=%d, image=%s\n",
					info.firstGid, info.columns, fullPath.c_str());
			}
		}
		else if (ts.contains("source"))
		{
			// Tileset được tham chiếu tới file .tsj (JSON) hoặc .tsx (XML) bên ngoài
			string srcPath = ts["source"];

			// Kiểm tra nếu file có đuôi .tsj hoặc .json thì đọc được
			// Nếu là .tsx (XML) thì cần TinyXML - ở đây ta chỉ hỗ trợ JSON
			wstring wSrcPath(srcPath.begin(), srcPath.end());
			wstring fullTsPath = wstring(basePath) + L"\\" + wSrcPath;

			ifstream tsFile(fullTsPath);
			if (!tsFile.is_open())
			{
				DebugOut(L"[WARNING] Cannot open external tileset: %s. "
					L"Please use 'Embed in Map' option in Tiled or export tileset as JSON (.tsj)\n",
					fullTsPath.c_str());
				continue;
			}

			json tsJson;
			tsFile >> tsJson;
			tsFile.close();

			info.columns = tsJson["columns"];
			info.tileCount = tsJson["tilecount"];
			info.tileWidth = tsJson["tilewidth"];
			info.tileHeight = tsJson["tileheight"];
			info.imageWidth = tsJson["imagewidth"];
			info.imageHeight = tsJson["imageheight"];

			string imgPath = tsJson["image"];
			wstring wImgPath(imgPath.begin(), imgPath.end());

			// Đường dẫn hình ảnh trong file .tsj là tương đối so với file .tsj
			// Mà file .tsj nằm cùng thư mục với file .json map
			wstring fullImgPath = wstring(basePath) + L"\\" + wImgPath;
			info.imagePath = fullImgPath;

			info.texture = CGame::GetInstance()->LoadTexture(fullImgPath.c_str());

			if (info.texture == NULL)
			{
				DebugOut(L"[ERROR] Failed to load tileset texture: %s\n", fullImgPath.c_str());
			}
			else
			{
				DebugOut(L"[INFO] Loaded external tileset: firstGid=%d, columns=%d, image=%s\n",
					info.firstGid, info.columns, fullImgPath.c_str());
			}
		}

		tilesets.push_back(info);
	}

	// Sắp xếp tilesets theo firstGid tăng dần (để tìm kiếm nhanh hơn)
	sort(tilesets.begin(), tilesets.end(),
		[](const TilesetInfo& a, const TilesetInfo& b) {
			return a.firstGid < b.firstGid;
		});


	mapLayers.clear();
	for (auto& layer : j["layers"])
	{
		string layerType = layer["type"];

		if (layerType == "tilelayer")
		{
			string layerName = layer["name"];
			DebugOut(L"[INFO] Loading tile layer: %s\n",
				wstring(layerName.begin(), layerName.end()).c_str());

			TileLayerInfo layerInfo;
			layerInfo.name = layerName;
			layerInfo.isForeground = (layerName == "Foreground");

			for (auto& id : layer["data"])
			{
				layerInfo.data.push_back(id.get<unsigned int>());
			}

			mapLayers.push_back(layerInfo);
			DebugOut(L"[INFO] Tile layer loaded: %d tiles\n", (int)layerInfo.data.size());
		}
	}
}

TilesetInfo* CTileMap::GetTilesetByGid(int gid)
{
	// Quét ngược từ cuối danh sách tilesets
	// Tileset có firstGid lớn nhất mà <= gid chính là tileset chứa tile đó
	for (int i = (int)tilesets.size() - 1; i >= 0; i--)
	{
		if (gid >= tilesets[i].firstGid)
			return &tilesets[i];
	}
	return nullptr;
}

void CTileMap::RenderLayers(bool foreground)
{
	if (mapLayers.empty() || tilesets.empty()) return;

	// Lấy vị trí camera để tối ưu: chỉ vẽ tile trong vùng nhìn thấy
	CCamera* camera = CCamera::GetInstance();
	float camX, camY;
	camera->GetCamPos(camX, camY);

	// Làm tròn camera để tránh rác pixel / mờ (Linear filtering issue)
	float cx = (float)floor(camX);
	float cy = (float)floor(camY);

	int screenWidth = (int)camera->GetWidth();
	int screenHeight = (int)camera->GetHeight();

	// Tính toán phạm vi cột/hàng cần vẽ (clip theo camera)
	int colStart = max(0, (int)(cx / tileWidth));
	int colEnd = min(width - 1, (int)((cx + screenWidth) / tileWidth));
	int rowStart = max(0, (int)(cy / tileHeight));
	int rowEnd = min(height - 1, (int)((cy + screenHeight) / tileHeight));

	for (size_t i = 0; i < mapLayers.size(); i++)
	{
		if (mapLayers[i].isForeground != foreground) continue;

		for (int row = rowStart; row <= rowEnd; row++)
		{
			for (int col = colStart; col <= colEnd; col++)
			{
				// Lấy tile ID từ mảng 1 chiều (quy đổi từ hàng, cột)
				int index = row * width + col;
				unsigned int tileData = mapLayers[i].data[index];

				// ID = 0 nghĩa là ô trống, bỏ qua
				if (tileData == 0) continue;

				// Giải mã cờ lật (Flip flags)
				bool flipX = (tileData & 0x80000000);
				bool flipY = (tileData & 0x40000000);
				
				// Tắt các cờ lật để lấy ID gốc của Tile
				int tileId = tileData & ~(0x80000000 | 0x40000000 | 0x20000000);

				// Tìm tileset chứa tile này
				TilesetInfo* ts = GetTilesetByGid(tileId);
				if (ts == nullptr || ts->texture == NULL) continue;

				// Tính LocalID (thứ tự thực sự trong tấm hình tileset)
				int localId = tileId - ts->firstGid;

				// Tính hàng và cột của tile trong tấm hình tileset
				int tsCol = localId % ts->columns;
				int tsRow = localId / ts->columns;

				// Tính RECT cắt từ tấm hình tileset
				// Phải trừ đi 1 vì hàm Draw tính chiều rộng = right - left + 1
				RECT srcRect;
				srcRect.left = tsCol * ts->tileWidth;
				srcRect.top = tsRow * ts->tileHeight;
				srcRect.right = srcRect.left + ts->tileWidth - 1;
				srcRect.bottom = srcRect.top + ts->tileHeight - 1;

				// Tính tọa độ vẽ trên màn hình (trừ đi camera) và LÀM TRÒN (floor)
				// Nếu để số thực (float), DirectX sẽ trộn màu (blend) làm texture bị mờ tịt
				float drawX = (float)floor((col * tileWidth) + tileWidth / 2.0f - cx);
				float drawY = (float)floor((row * tileHeight) + tileHeight / 2.0f - cy);

				int nx = flipX ? -1 : 1;
				int ny = flipY ? -1 : 1;

				CGame::GetInstance()->Draw(drawX, drawY, ts->texture, &srcRect, 1.0f, 0, 0, nx, ny);
			}
		}
	}
}

void CTileMap::Render()
{
	RenderBackground();
	RenderForeground();
}

void CTileMap::RenderBackground()
{
	RenderLayers(false);
}

void CTileMap::RenderForeground()
{
	RenderLayers(true);
}


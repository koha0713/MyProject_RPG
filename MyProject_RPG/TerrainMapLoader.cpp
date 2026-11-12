#include "TerrainMapLoader.h"

#include <fstream>
#include <sstream>
#include <string>

#include "TerrainMapData.h"

namespace
{
	/**
	 * @brief 文字列をTerrainTypeへ変換
	 */
	bool ParseTerrainType(
		const std::string& value,
		TerrainType& outType)
	{
		if (value == "Grass")
		{
			outType =
				TerrainType::Grass;

			return true;
		}

		if (value == "Dirt")
		{
			outType =
				TerrainType::Dirt;

			return true;
		}

		if (value == "Rock")
		{
			outType =
				TerrainType::Rock;

			return true;
		}

		if (value == "Water")
		{
			outType =
				TerrainType::Water;

			return true;
		}

		if (value == "Forest")
		{
			outType = TerrainType::Forest;
			return true;
		}

		return false;
	}
}

//=====================================================
// Load
//=====================================================

bool TerrainMapLoader::Load(
	const std::string& filePath,
	TerrainMapData& outData)
{
	outData.Clear();

	std::ifstream file(
		filePath);

	if (!file.is_open())
	{
		return false;
	}

	std::string line;

	int y =
		0;

	while (std::getline(
		file,
		line))
	{
		// 空行は無視
		if (line.empty())
		{
			continue;
		}

		std::istringstream stream(
			line);

		std::string token;

		int x =
			0;

		while (stream >>
			token)
		{
			TerrainType type;

			if (!ParseTerrainType(
				token,
				type))
			{
				return false;
			}

			TerrainCellData cell;

			cell.Type =
				type;

			cell.Position =
				GridPosition
			{
				x,
				y
			};

			//=====================================================
			// Visual Seed
			//=====================================================
			//
			// Grid座標だけからSeedを作ることで、
			// 起動するたびに草・岩・木の配置が変化するのを防ぐ。
			//=====================================================

			cell.VisualSeed =
				static_cast<uint32_t>(
					(x * 73856093) ^
					(y * 19349663));

			outData.Cells.push_back(
				cell);

			++x;
		}

		++y;
	}

	// 全TerrainCellDataの読み込みが完了したあと
	UpdateRoadConnections(outData);

	return true;
}

/**
 * @brief 指定位置に道路が存在するか確認
 */
bool TerrainMapLoader::IsRoadAt(
	const TerrainMapData& mapData,
	int x,
	int y)
{
	for (const auto& cell : mapData.Cells)
	{
		if (cell.Position.X == x &&
			cell.Position.Y == y)
		{
			return cell.Type == TerrainType::Dirt;
		}
	}

	return false;
}

/**
 * @brief 全Terrainの道路接続情報を更新
 */
void TerrainMapLoader::UpdateRoadConnections(
	TerrainMapData& mapData)
{
	for (auto& cell : mapData.Cells)
	{
		cell.RoadConnections = 0;

		// 道路以外は判定しない
		if (cell.Type != TerrainType::Dirt)
		{
			continue;
		}

		const int x = cell.Position.X;
		const int y = cell.Position.Y;

		// 上
		if (IsRoadAt(mapData, x, y - 1))
		{
			cell.RoadConnections |=
				static_cast<uint8_t>(
					RoadConnection::Up);
		}

		// 右
		if (IsRoadAt(mapData, x + 1, y))
		{
			cell.RoadConnections |=
				static_cast<uint8_t>(
					RoadConnection::Right);
		}

		// 下
		if (IsRoadAt(mapData, x, y + 1))
		{
			cell.RoadConnections |=
				static_cast<uint8_t>(
					RoadConnection::Down);
		}

		// 左
		if (IsRoadAt(mapData, x - 1, y))
		{
			cell.RoadConnections |=
				static_cast<uint8_t>(
					RoadConnection::Left);
		}
	}
}
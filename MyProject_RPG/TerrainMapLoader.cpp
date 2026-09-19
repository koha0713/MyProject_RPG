#include "TerrainMapLoader.h"

#include <fstream>
#include <sstream>
#include <string>

#include "TerrainMapData.h"

namespace
{
	/**
	 * @brief •¶Žš—ñ‚ðTerrainType‚Ö•ÏŠ·
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
		// ‹ós‚Í–³Ž‹
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

			outData.Cells.push_back(
				cell);

			++x;
		}

		++y;
	}

	return true;
}
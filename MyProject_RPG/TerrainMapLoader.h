#pragma once

#include <string>

struct TerrainMapData;

/**
 * @brief TerrainMap.txtを読み込むLoader
 */
class TerrainMapLoader
{
public:

	/**
	 * @brief TerrainMapファイルを読み込む
	 *
	 * @param filePath 読み込むファイルパス
	 * @param outData 読み込み結果
	 *
	 * @return 成功時true
	 */
	static bool Load(
		const std::string& filePath,
		TerrainMapData& outData);
};
#pragma once

/**
 * @file CharaMapLoader.h
 * @brief CharaMap.txtの読み込み
 */

#include <string>

struct CharaMapData;

/**
 * @brief キャラクター配置MapLoader
 */
class CharaMapLoader
{
public:

	/**
	 * @brief CharaMapファイルを読み込む
	 *
	 * @param filePath 読み込むファイルパス
	 * @param outData 読み込み結果
	 *
	 * @return 成功ならtrue
	 */
	static bool Load(
		const std::string& filePath,
		CharaMapData& outData);
};
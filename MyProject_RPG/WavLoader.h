#pragma once

/**
 * @file WavLoader.h
 * @brief WAVファイル読み込み
 */

#include <memory>
#include <string>

#include "SoundData.h"

 /**
  * @brief WAV Loader
  */
class WavLoader
{
public:

	/**
	 * @brief WAVファイルを読み込む
	 *
	 * @param filePath ファイルパス
	 *
	 * @return 成功時 SoundData
	 *         失敗時 nullptr
	 */
	static std::shared_ptr<SoundData>
		Load(
			const std::wstring& filePath);
};
#pragma once

/**
 * @file SoundData.h
 * @brief 音声Resourceデータ
 */

#include <Windows.h>
#include <mmreg.h>

#include <cstdint>
#include <vector>

 /**
  * @brief 読み込まれた音声データ
  *
  * @details
  * WAVファイルから取得したWaveFormatとPCMデータを保持する。
  *
  * SoundData自体は再生状態を持たず、
  * 複数のSourceVoiceから共有して使用する。
  */
struct SoundData
{
	/**
	 * @brief WAVフォーマット情報
	 */
	WAVEFORMATEX Format{};

	/**
	 * @brief PCM Audio Data
	 */
	std::vector<uint8_t>
		AudioData;

	/**
	 * @brief 有効な音声データか
	 */
	bool IsValid() const
	{
		return
			!AudioData.empty() &&
			Format.nChannels > 0 &&
			Format.nSamplesPerSec > 0;
	}
};
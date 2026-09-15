#pragma once

/**
 * @file SoundHandle.h
 * @brief 再生中Soundを識別するHandle
 */

#include <cstdint>

 /**
  * @brief Sound再生Handle
  *
  * @details
  * 外部側がXAudio2のSourceVoiceを
  * 直接触らないために使用する。
  */
struct SoundHandle
{
	uint64_t Id = 0;

	bool IsValid() const
	{
		return
			Id != 0;
	}

	bool operator==(
		const SoundHandle& other) const
	{
		return
			Id == other.Id;
	}
};
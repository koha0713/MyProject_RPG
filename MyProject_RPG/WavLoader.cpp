#include "WavLoader.h"

#include <fstream>
#include <cstring>

namespace
{
	/**
	 * @brief RIFF Chunk Header
	 */
	struct RiffHeader
	{
		char Riff[4];
		uint32_t FileSize;
		char Wave[4];
	};

	/**
	 * @brief Chunk Header
	 */
	struct ChunkHeader
	{
		char Id[4];
		uint32_t Size;
	};

	bool IsChunk(
		const char id[4],
		const char* target)
	{
		return
			std::memcmp(
				id,
				target,
				4) == 0;
	}
}

std::shared_ptr<SoundData>
WavLoader::Load(
	const std::wstring& filePath)
{
	std::ifstream file(
		filePath,
		std::ios::binary);

	if (!file)
	{
		return nullptr;
	}

	//=================================================
	// RIFF Header
	//=================================================

	RiffHeader riffHeader{};

	file.read(
		reinterpret_cast<char*>(
			&riffHeader),
		sizeof(RiffHeader));

	if (!file)
	{
		return nullptr;
	}

	if (!IsChunk(
		riffHeader.Riff,
		"RIFF"))
	{
		return nullptr;
	}

	if (!IsChunk(
		riffHeader.Wave,
		"WAVE"))
	{
		return nullptr;
	}

	auto soundData =
		std::make_shared<SoundData>();

	bool foundFormat =
		false;

	bool foundData =
		false;

	//=================================================
	// Chunk探索
	//=================================================

	while (file &&
		(!foundFormat ||
			!foundData))
	{
		ChunkHeader chunkHeader{};

		file.read(
			reinterpret_cast<char*>(
				&chunkHeader),
			sizeof(ChunkHeader));

		if (!file)
		{
			break;
		}

		//=============================================
		// fmt Chunk
		//=============================================

		if (IsChunk(
			chunkHeader.Id,
			"fmt "))
		{
			// WAVEFORMATEXより大きなfmt Chunkもあるため、
			// 一旦Bufferへ読み込む。
			std::vector<uint8_t>
				formatBuffer(
					chunkHeader.Size);

			file.read(
				reinterpret_cast<char*>(
					formatBuffer.data()),
				chunkHeader.Size);

			if (!file)
			{
				return nullptr;
			}

			if (chunkHeader.Size <
				16)
			{
				return nullptr;
			}

			// PCMの基本部分をコピー
			std::memcpy(
				&soundData->Format,
				formatBuffer.data(),
				(chunkHeader.Size <
					sizeof(WAVEFORMATEX))
				? chunkHeader.Size
				: sizeof(WAVEFORMATEX));

			foundFormat =
				true;
		}

		//=============================================
		// data Chunk
		//=============================================

		else if (IsChunk(
			chunkHeader.Id,
			"data"))
		{
			soundData->AudioData.resize(
				chunkHeader.Size);

			file.read(
				reinterpret_cast<char*>(
					soundData->AudioData.data()),
				chunkHeader.Size);

			if (!file)
			{
				return nullptr;
			}

			foundData =
				true;
		}

		//=============================================
		// Unknown Chunk
		//=============================================

		else
		{
			file.seekg(
				chunkHeader.Size,
				std::ios::cur);
		}

		// RIFF Chunkは偶数Byte境界に配置される
		if ((chunkHeader.Size & 1u) != 0)
		{
			file.seekg(
				1,
				std::ios::cur);
		}
	}

	if (!foundFormat ||
		!foundData)
	{
		return nullptr;
	}

	//=================================================
	// PCM確認
	//=================================================
	//
	// 第1段階ではPCM / IEEE Floatを対象とする。
	// ADPCM等は後から対応可能。
	//

	if (soundData->Format.wFormatTag !=
		WAVE_FORMAT_PCM &&
		soundData->Format.wFormatTag !=
		WAVE_FORMAT_IEEE_FLOAT)
	{
		return nullptr;
	}

	if (!soundData->IsValid())
	{
		return nullptr;
	}

	return soundData;
}
/**
 * @file CharaMapLoader.cpp
 */

#include "CharaMapLoader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <Windows.h>

#include "CharaMapData.h"

namespace
{
	/**
	 * @brief 文字列前後の空白を削除する
	 */
	std::string Trim(
		const std::string& value)
	{
		auto begin =
			std::find_if_not(
				value.begin(),
				value.end(),
				[](unsigned char c)
				{
					return
						std::isspace(c);
				});

		auto end =
			std::find_if_not(
				value.rbegin(),
				value.rend(),
				[](unsigned char c)
				{
					return
						std::isspace(c);
				})
			.base();

		if (begin >= end)
		{
			return {};
		}

		return std::string(
			begin,
			end);
	}

	/**
	 * @brief Section名をCharacterSpawnKindへ変換する
	 */
	bool ParseKind(
		const std::string& section,
		CharacterSpawnKind& outKind)
	{
		if (section == "Player")
		{
			outKind =
				CharacterSpawnKind::Player;

			return true;
		}

		if (section == "Enemy")
		{
			outKind =
				CharacterSpawnKind::Enemy;

			return true;
		}

		return false;
	}
}

//=====================================================
// Load
//=====================================================

bool CharaMapLoader::Load(
	const std::string& filePath,
	CharaMapData& outData)
{
	outData.Clear();

	std::ifstream file(
		filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"[CharaMapLoader] Failed to open file.\n");

		return false;
	}

	CharacterSpawnData
		currentData{};

	bool hasSection =
		false;

	bool hasID =
		false;

	bool hasType =
		false;

	bool hasX =
		false;

	bool hasY =
		false;

	//=================================================
	// 現在読み込み中のSectionを確定する
	//=================================================

	auto flushCurrentData =
		[&]() -> bool
		{
			if (!hasSection)
			{
				return true;
			}

			// 必須データ確認
			if (!hasID ||
				!hasType ||
				!hasX ||
				!hasY)
			{
				OutputDebugStringA(
					"[CharaMapLoader] "
					"Character data is incomplete.\n");

				return false;
			}

			outData.Characters.push_back(
				currentData);

			// 次Section用に初期化
			currentData =
				CharacterSpawnData{};

			hasSection =
				false;

			hasID =
				false;

			hasType =
				false;

			hasX =
				false;

			hasY =
				false;

			return true;
		};

	std::string line;

	int lineNumber =
		0;

	while (std::getline(
		file,
		line))
	{
		++lineNumber;

		line =
			Trim(line);

		//=============================================
		// 空行
		//=============================================

		if (line.empty())
		{
			continue;
		}

		//=============================================
		// Comment
		//=============================================

		if (line[0] == '#' ||
			line[0] == ';')
		{
			continue;
		}

		//=============================================
		// Section
		//=============================================

		if (line.front() == '[' &&
			line.back() == ']')
		{
			// 直前Sectionを保存
			if (!flushCurrentData())
			{
				return false;
			}

			const std::string section =
				Trim(
					line.substr(
						1,
						line.size() - 2));

			CharacterSpawnKind kind{};

			if (!ParseKind(
				section,
				kind))
			{
				OutputDebugStringA(
					"[CharaMapLoader] "
					"Unknown section.\n");

				return false;
			}

			currentData.Kind =
				kind;

			hasSection =
				true;

			continue;
		}

		// Section外にKeyValueがある
		if (!hasSection)
		{
			OutputDebugStringA(
				"[CharaMapLoader] "
				"KeyValue found outside section.\n");

			return false;
		}

		//=============================================
		// Key = Value
		//=============================================

		const size_t separator =
			line.find('=');

		if (separator ==
			std::string::npos)
		{
			OutputDebugStringA(
				"[CharaMapLoader] "
				"Invalid line format.\n");

			return false;
		}

		const std::string key =
			Trim(
				line.substr(
					0,
					separator));

		const std::string value =
			Trim(
				line.substr(
					separator + 1));

		//=============================================
		// ID
		//=============================================

		if (key == "ID")
		{
			currentData.ID =
				value;

			hasID =
				true;

			continue;
		}

		//=============================================
		// Type
		//=============================================

		if (key == "Type")
		{
			currentData.Type =
				value;

			hasType =
				true;

			continue;
		}

		//=============================================
		// X
		//=============================================

		if (key == "X")
		{
			try
			{
				currentData.Position.X =
					std::stoi(value);

				hasX =
					true;
			}
			catch (...)
			{
				OutputDebugStringA(
					"[CharaMapLoader] "
					"Invalid X value.\n");

				return false;
			}

			continue;
		}

		//=============================================
		// Y
		//=============================================

		if (key == "Y")
		{
			try
			{
				currentData.Position.Y =
					std::stoi(value);

				hasY =
					true;
			}
			catch (...)
			{
				OutputDebugStringA(
					"[CharaMapLoader] "
					"Invalid Y value.\n");

				return false;
			}

			continue;
		}

		//=============================================
		// Unknown Key
		//=============================================

		OutputDebugStringA(
			"[CharaMapLoader] "
			"Unknown key found.\n");

		return false;
	}

	//=================================================
	// 最後のSectionを保存
	//=================================================

	if (!flushCurrentData())
	{
		return false;
	}

	OutputDebugStringA(
		"[CharaMapLoader] Load succeeded.\n");

	return true;
}
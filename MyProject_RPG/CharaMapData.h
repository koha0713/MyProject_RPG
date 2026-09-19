#pragma once

/**
 * @file CharaMapData.h
 * @brief Field上のキャラクター初期配置データ
 */

#include <string>
#include <vector>

#include "GridPosition.h"

 /**
  * @brief 配置するキャラクターの種類
  */
enum class CharacterSpawnKind
{
	Player,
	Enemy
};

/**
 * @brief 1キャラクター分の配置データ
 *
 * @details
 * CharaMapには「どのキャラクターをどこに配置するか」
 * だけを保持する。
 *
 * HPやAttackPowerなどのキャラクター固有性能は
 * 将来的にCharacterDefinition側へ分離する。
 */
struct CharacterSpawnData
{
	/**
	 * @brief Player / Enemy
	 */
	CharacterSpawnKind Kind =
		CharacterSpawnKind::Enemy;

	/**
	 * @brief GameObjectを識別する一意のID
	 */
	std::string ID;

	/**
	 * @brief キャラクター種類
	 *
	 * 例:
	 * Warrior
	 * Ranger
	 */
	std::string Type;

	/**
	 * @brief 初期Grid座標
	 */
	GridPosition Position{};
};

/**
 * @brief CharaMap全体のデータ
 */
struct CharaMapData
{
	std::vector<CharacterSpawnData>
		Characters;

	/**
	 * @brief データを破棄
	 */
	void Clear()
	{
		Characters.clear();
	}
};
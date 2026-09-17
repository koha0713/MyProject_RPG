#pragma once

#include <cstdint>
/**
 * @file TurnManager.h
 * @brief Player / Enemy のターン進行を管理する
 */

enum class TurnPhase
{
	Player,
	Enemy
};

class TurnManager
{
public:

	void Initialize();

	void Update();

	/**
	 * @brief Playerの行動完了
	 */
	void EndPlayerTurn();

	/**
	 * @brief Enemy側の全行動完了
	 */
	void EndEnemyTurn();

	bool IsPlayerTurn() const;

	bool IsEnemyTurn() const;

	TurnPhase GetPhase() const
	{
		return m_Phase;
	}

	/**
	 * @brief 現在のPlayerTurn番号
	 *
	 * @details
	 * EnemyTurn終了後に加算される。
	 * Component側で新しいターンを検出するために使用する。
	 */
	uint64_t GetTurnCount() const
	{
		return m_TurnCount;
	}

private:

	TurnPhase m_Phase =
		TurnPhase::Player;

	uint64_t m_TurnCount =
		0;
};
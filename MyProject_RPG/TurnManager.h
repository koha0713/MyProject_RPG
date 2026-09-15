#pragma once

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

private:

	TurnPhase m_Phase =
		TurnPhase::Player;
};
#include "TurnManager.h"

void TurnManager::Initialize()
{
	m_Phase =
		TurnPhase::Player;

	m_TurnCount =
		0;
}

void TurnManager::Update()
{
	// 現段階では状態保持のみ。
	// EnemyManagerやEnemyAIを接続した段階で
	// Enemyターン処理をここから起動する。
}

void TurnManager::EndPlayerTurn()
{
	if (m_Phase !=
		TurnPhase::Player)
	{
		return;
	}

	m_Phase =
		TurnPhase::Enemy;
}

void TurnManager::EndEnemyTurn()
{
	if (m_Phase !=
		TurnPhase::Enemy)
	{
		return;
	}
	m_TurnCount++;

	m_Phase =
		TurnPhase::Player;
}

bool TurnManager::IsPlayerTurn() const
{
	return
		m_Phase ==
		TurnPhase::Player;
}

bool TurnManager::IsEnemyTurn() const
{
	return
		m_Phase ==
		TurnPhase::Enemy;
}
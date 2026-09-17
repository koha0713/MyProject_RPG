#pragma once

/**
 * @file EnemyAIComponent.h
 * @brief Enemyの簡易AIを管理するComponent
 */

#include <vector>

#include "Component.h"
#include "GridPosition.h"

class GameObject;

/**
 * @brief Enemyの現在行動状態
 */
enum class EnemyActionState
{
	// 次の行動を開始できる
	Ready,

	// Attack Animation再生中
	Attacking,

	// このEnemyのTurn行動終了
	Finished
};

/**
 * @brief Enemy AI Component
 */
class EnemyAIComponent :
	public Component
{
public:

	EnemyAIComponent() = default;
	~EnemyAIComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//====================
	// Action
	//====================

	/**
	 * @brief Enemyの1ターン分の行動を開始する
	 *
	 * @param players Player候補一覧
	 *
	 * @return 行動を実行した場合true
	 */
	bool Act(
		const std::vector<GameObject*>& players);

	/**
	 * @brief 次のEnemyTurnに備えて状態を初期化する
	 */
	void ResetAction();

	/**
	 * @brief このEnemyの行動が終了したか
	 */
	bool IsActionFinished() const
	{
		return
			m_ActionState ==
			EnemyActionState::Finished;
	}

	/**
	 * @brief 攻撃Animation待ちか
	 */
	bool IsAttacking() const
	{
		return
			m_ActionState ==
			EnemyActionState::Attacking;
	}

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "EnemyAIComponent";
	}

private:

	GameObject* FindNearestPlayer(
		const std::vector<GameObject*>& players) const;

	bool TryMoveToward(
		const GridPosition& playerPosition);

	bool IsAdjacent(
		GameObject* player) const;

	bool TryAttack(
		GameObject* player);

private:

	/**
	 * @brief 現在の行動状態
	 */
	EnemyActionState m_ActionState =
		EnemyActionState::Ready;
};
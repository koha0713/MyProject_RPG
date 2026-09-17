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
	// 行動開始可能
	Ready,

	// 移動Animation中
	Moving,

	// 攻撃Animation中
	Attacking,

	// 今Turnの行動終了
	Finished
};

/**
 * @brief Enemy AI Component
 *
 * @details
 * 最も近いPlayerをターゲットとして、
 * Agilityの範囲内で接近・攻撃する。
 *
 * 移動はTransformを時間補間し、
 * 見た目上滑らかに1Gridずつ移動する。
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
	 * @brief Enemyの1Turn分の行動を開始する
	 */
	bool Act(
		const std::vector<GameObject*>& players);

	/**
	 * @brief 次のEnemyTurn用に状態を初期化する
	 */
	void ResetAction();

	bool IsActionFinished() const
	{
		return
			m_ActionState ==
			EnemyActionState::Finished;
	}

	bool IsMoving() const
	{
		return
			m_ActionState ==
			EnemyActionState::Moving;
	}

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

	//====================
	// Target
	//====================

	/**
	 * @brief 最も近い生存Playerを検索
	 */
	GameObject* FindNearestPlayer(
		const std::vector<GameObject*>& players) const;

	//====================
	// Movement
	//====================

	/**
	 * @brief Target方向への次の1Gridを決定する
	 *
	 * @param playerPosition TargetのGrid座標
	 * @param outNextPosition 次に進むGrid座標
	 *
	 * @return 移動可能なGridが見つかった場合true
	 */
	bool FindNextMovePosition(
		const GridPosition& playerPosition,
		GridPosition& outNextPosition);

	/**
	 * @brief 1マス分の移動Animationを開始する
	 */
	bool BeginMove(
		const GridPosition& nextPosition);

	/**
	 * @brief 移動Animation更新
	 */
	void UpdateMovement(
		uint64_t delta);

	/**
	 * @brief 次の移動または攻撃を判断する
	 */
	void ContinueAction();

	//====================
	// Attack
	//====================

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

	/**
	 * @brief 現在狙っているPlayer
	 *
	 * GameObjectManagerが所有するため非所有。
	 * Enemyの1Turn中のみ使用する。
	 */
	GameObject* m_Target =
		nullptr;

	/**
	 * @brief 今Turnで残っている移動回数
	 */
	int m_RemainingMovePoints =
		0;

	/**
	 * @brief 現在向かっている次のGrid
	 */
	GridPosition m_NextGridPosition{};

	/**
	 * @brief 1秒あたりのWorld移動速度
	 */
	float m_MoveSpeed =
		0.01f;
};
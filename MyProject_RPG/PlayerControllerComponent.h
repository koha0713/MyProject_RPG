#pragma once

/**
 * @file PlayerControllerComponent.h
 * @brief Playerの入力と行動要求を管理するComponent
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "Component.h"
#include "GridPosition.h"
#include "TurnManager.h"

class GameObject;

/**
 * @brief Playerの現在行動状態
 */
enum class PlayerActionState
{
	Idle,

	Moving,

	Attacking
};

/**
 * @brief Player Controller
 *
 * @details
 * Playerの共通行動を管理する。
 *
 * 担当：
 * ・移動要求
 * ・攻撃要求
 * ・Turn中の行動状態
 * ・移動力
 * ・攻撃Damage適用
 *
 * 職業によって異なる、
 * ・攻撃Animation
 * ・攻撃Sequence
 *
 * はPlayerClassComponentへ委譲する。
 */
class PlayerControllerComponent :
	public Component
{
public:

	PlayerControllerComponent() = default;

	~PlayerControllerComponent() override =
		default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//=================================================
	// Turn
	//=================================================

	/**
	 * @brief TurnManagerを設定
	 *
	 * @note
	 * 所有権は持たない。
	 */
	void SetTurnManager(
		TurnManager* turnManager)
	{
		m_TurnManager =
			turnManager;
	}

	/**
	 * @brief 新しいPlayerTurnの開始準備
	 *
	 * @details
	 * FieldSceneからPlayerTurn開始時に呼び出す。
	 * 移動力・行動状態を初期化する。
	 */
	void StartPlayerTurn(
		uint64_t turnCount);

	//=================================================
	// Movement
	//=================================================

	/**
	 * @brief 指定方向へ1マス移動を要求する
	 *
	 * @param direction
	 * Grid上の移動方向
	 *
	 * @return
	 * 移動成功時true
	 */
	bool RequestMove(
		const GridPosition& direction);

	/**
	 * @brief 指定Gridへの移動を要求する
	 *
	 * @param destination
	 * 移動先Grid座標
	 *
	 * @return
	 * 移動開始成功時true
	 *
	 * @details
	 * A*で経路を取得して、
	 * 残り移動力以内なら移動を開始する。
	 */
	bool RequestMoveTo(
		const GridPosition& destination);

	/**
	 * @brief 現在の残り移動力を取得
	 */
	int GetRemainingMovePoints() const
	{
		return m_RemainingMovePoints;
	}

	//=================================================
	// Attack
	//=================================================

	/**
	 * @brief 隣接Enemyを検索して攻撃する
	 *
	 * @details
	 * Keyboard操作等で利用する簡易入口。
	 */
	bool RequestAttack();

	/**
	 * @brief 指定Enemyへの攻撃を要求する
	 *
	 * @param target
	 * 攻撃対象Enemy
	 *
	 * @return
	 * 攻撃開始成功時true
	 *
	 * @details
	 * 実際のAnimation Sequenceは
	 * PlayerClassComponentへ委譲する。
	 */
	bool RequestAttack(
		GameObject* target);

	/**
	 * @brief 職業固有攻撃Sequenceを更新する
	 *
	 * PlayerClassComponentから
	 * Hit通知を受け取ってDamageを適用する。
	 */
	void UpdateClassAttack();

	/**
	 * @brief 現在職業攻撃中か
	 */
	bool IsClassAttacking() const
	{
		return m_ClassAttackActive;
	}

	//=================================================
	// Turn Action
	//=================================================

	bool HasActed() const
	{
		return m_HasActed;
	}

	/**
	 * @brief 現在の行動が終了しているか
	 *
	 * @return
	 * Idleならtrue
	 */
	bool IsActionFinished() const
	{
		return
			m_ActionState ==
			PlayerActionState::Idle;
	}

	void SetActed(
		bool acted)
	{
		m_HasActed =
			acted;
	}

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "PlayerControllerComponent";
	}

private:

	//=================================================
	// Turn
	//=================================================

	/**
	 * @brief PlayerTurn開始処理
	 *
	 * CharacterStatusComponentの
	 * Agilityから移動力を設定する。
	 */
	void BeginPlayerTurn();

	//=================================================
	// Movement
	//=================================================

	/**
	 * @brief 移動中更新
	 */
	void UpdateMovement(
		uint64_t delta);

	/**
	 * @brief Path移動開始
	 */
	bool BeginMove(
		const std::vector<GridPosition>& path);

private:

	//=================================================
	// Turn
	//=================================================

	/**
	 * @brief Turn管理
	 *
	 * 所有権は持たない。
	 */
	TurnManager*
		m_TurnManager =
		nullptr;

	/**
	 * @brief 現在のPlayerTurnで残っている移動力
	 */
	int
		m_RemainingMovePoints =
		0;

	/**
	 * @brief 最後に開始処理したTurn番号
	 */
	uint64_t
		m_LastTurnCount =
		(std::numeric_limits<uint64_t>::max)();

	//=================================================
	// Action
	//=================================================

	PlayerActionState
		m_ActionState =
		PlayerActionState::Idle;

	/**
	 * @brief このTurnですでに行動したか
	 */
	bool
		m_HasActed =
		false;

	//=================================================
	// Movement
	//=================================================

	/**
	 * @brief 現在移動中Path
	 *
	 * index 0 = 開始地点
	 */
	std::vector<GridPosition>
		m_MovePath;

	/**
	 * @brief 現在向かっているPath Index
	 */
	size_t
		m_MovePathIndex =
		0;

	/**
	 * @brief 1秒あたりのWorld移動速度
	 */
	float
		m_MoveSpeed =
		0.01f;

	//=================================================
	// Class Attack
	//=================================================

	/**
	 * @brief 現在の攻撃対象
	 *
	 * GameObjectManager所有。
	 * PlayerControllerは非所有。
	 */
	GameObject*
		m_AttackTarget =
		nullptr;

	/**
	 * @brief PlayerClassComponentによる
	 * 攻撃Sequence実行中か
	 */
	bool
		m_ClassAttackActive =
		false;
};
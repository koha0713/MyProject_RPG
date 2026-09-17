#pragma once

/**
 * @file PlayerControllerComponent.h
 * @brief Playerの入力と行動要求を管理するComponent
 */

#include <cstdint>
#include <limits>

#include "Component.h"
#include "GridPosition.h"
#include "TurnManager.h"

enum class PlayerActionState
{
	Idle,
	Attacking
};

/**
 * @brief Player Controller
 *
 * @details
 * Keyboard / Mouseなどの入力を、
 * Game上の行動要求へ変換する。
 *
 * 実際のGrid座標管理は
 * GridPositionComponentへ委譲する。
 *
 * 現段階ではKeyboard操作だが、
 * 移動・攻撃処理を関数として分離することで、
 * 将来的なMouse操作にも対応できる構造にしている。
 */
class PlayerControllerComponent :
	public Component
{
public:

	PlayerControllerComponent() = default;
	~PlayerControllerComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// Turn
	//====================

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

	//====================
	// Movement
	//====================

	/**
	 * @brief 指定方向へ1マス移動を要求する
	 *
	 * @param direction Grid上の移動方向
	 *
	 * @return 移動成功時true
	 *
	 * @details
	 * Keyboard / Mouseなど入力方法に依存しない
	 * 共通の移動入口として使用する。
	 */
	bool RequestMove(
		const GridPosition& direction);

	/**
	 * @brief 指定Gridへの移動を要求する
	 *
	 * @param destination 移動先Grid座標
	 *
	 * @return 移動成功時true
	 *
	 * @details
	 * 現段階では隣接する1マスのみ移動可能。
	 *
	 * 将来的にはPathFindingを使用し、
	 * 離れたGridへの移動要求にも対応する。
	 */
	bool RequestMoveTo(
		const GridPosition& destination);

	//====================
	// Attack
	//====================

	/**
	 * @brief 隣接するEnemyへの攻撃を要求する
	 *
	 * @return 攻撃成功時true
	 *
	 * @details
	 * Player周囲4マスを検索し、
	 * 隣接しているEnemyを攻撃する。
	 *
	 * 現段階では複数隣接している場合、
	 * 検索順で最初に見つかったEnemyを攻撃する。
	 *
	 * 将来的にはMouse選択による
	 * 対象指定攻撃へ拡張する。
	 */
	bool RequestAttack();

	/**
	 * @brief 指定したEnemyへの攻撃を要求する
	 *
	 * @param target 攻撃対象
	 *
	 * @return 攻撃成功時true
	 *
	 * @details
	 * Mouse操作などで明示的に選択された対象を攻撃する。
	 * 隣接していない場合やEnemy以外は攻撃しない。
	 */
	bool RequestAttack(
		GameObject* target);

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "PlayerControllerComponent";
	}

private:

	/**
	 * @brief 新しいPlayerTurn開始処理
	 *
	 * CharacterStatusComponentの敏捷値から
	 * そのターンの移動可能回数を設定する。
	 */
	void BeginPlayerTurn();

private:

	/**
	 * @brief Turn管理
	 *
	 * 所有権は持たない。
	 */
	TurnManager* m_TurnManager =
		nullptr;

	/**
	 * @brief 現在のPlayerTurnで残っている移動可能回数
	 */
	int m_RemainingMovePoints =
		0;

	/**
	 * @brief 最後に開始処理を行ったTurn番号
	 *
	 * 初回Updateで必ずBeginPlayerTurn()が
	 * 呼ばれるよう最大値で初期化する。
	 */
	uint64_t m_LastTurnCount =
		(std::numeric_limits<uint64_t>::max)();

	/**
	 * @brief Playerの現在行動状態
	 */
	PlayerActionState m_ActionState =
		PlayerActionState::Idle;
};
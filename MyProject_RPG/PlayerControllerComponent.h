#pragma once

/**
 * @file PlayerControllerComponent.h
 * @brief Playerの行動要求と実行状態を管理するComponent
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "Component.h"
#include "GridPosition.h"
#include "TurnManager.h"
#include "PlayerActionRequest.h"

#include "EffectDefinition.h"
#include "PlayerActionDefinition.h"
#include "GridPattern.h"

class GameObject;
class CharacterStatusComponent;
class PlayerActionComponent;

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
 * Playerへの操作要求をGame上の行動へ変換する。
 *
 * このComponent自身は、
 * AP / MP等のResourceを保持しない。
 *
 * AP / MP
 *     CharacterStatusComponent
 *
 * Action Cost
 *     PlayerActionComponent
 *
 * 職業固有Action
 *     PlayerClassComponent
 *
 * をそれぞれ利用する。
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
	 * @brief TurnManager設定
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
	 * @brief 新しいPlayerTurn開始
	 *
	 * @details
	 * PlayerActionComponent::BeginTurn()を呼び、
	 * APを最大値まで回復する。
	 */
	void StartPlayerTurn(
		uint64_t turnCount);

	//=================================================
	// Movement
	//=================================================

	/**
	 * @brief 指定方向へ1マス移動
	 *
	 * @details
	 * デバッグ操作などの簡易移動用。
	 *
	 * Move ActionとしてAPを消費する。
	 */
	bool RequestMove(
		const GridPosition& direction);

	/**
	 * @brief 指定Gridへの移動要求
	 *
	 * @details
	 * A*でPathを検索し、
	 * CharacterStatusComponent::CalculateMoveRange()
	 * 以内であれば移動を開始する。
	 *
	 * 距離に関係なく、
	 * 1回のMove ActionとしてAPを消費する。
	 */
	bool RequestMoveTo(
		const GridPosition& destination);

	//=================================================
	// Attack
	//=================================================

	/**
	 * @brief 攻撃可能範囲内からEnemyを検索して攻撃
	 *
	 * @note
	 * 現段階では簡易操作用。
	 */
	bool RequestAttack();

	/**
	 * @brief 指定Enemyへの通常攻撃要求
	 *
	 * @param target 攻撃対象
	 *
	 * @return
	 * 攻撃開始成功時true
	 */
	bool RequestAttack(
		GameObject* target);

	/**
	 * @brief 職業固有通常攻撃Sequence更新
	 */
	void UpdateClassAttack();

	/**
	 * @brief 現在攻撃中か
	 */
	bool IsClassAttacking() const
	{
		return m_ClassAttackActive;
	}

	//=================================================
	// Action State
	//=================================================

	/**
	 * @brief 現在実行中Actionが終了しているか
	 */
	bool IsActionFinished() const
	{
		return
			m_ActionState ==
			PlayerActionState::Idle;
	}

	/**
	 * @brief Action実行要求
	 *
	 * @details
	 * ActionDefinitionのTargetingModeに応じて、
	 * 適切なAction処理へ振り分ける。
	 */
	bool RequestAction(
		const PlayerActionRequest& request);

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
	 * @brief PlayerTurn開始内部処理
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

	//=================================================
	// Component Access
	//=================================================

	/**
	 * @brief CharacterStatus取得
	 */
	CharacterStatusComponent*
		GetStatus() const;

	/**
	 * @brief PlayerActionComponent取得
	 */
	PlayerActionComponent*
		GetPlayerAction() const;

	/**
	 * @brief PlayerがAction要求を受け付けられるか
	 *
	 * @details
	 * ・PlayerTurn
	 * ・Idle
	 * ・生存
	 *
	 * を確認する。
	 */
	bool CanAcceptActionRequest() const;


	/**
	 * @brief Unit Target Action
	 */
	bool RequestUnitAction(
		PlayerActionType type,
		GameObject* target);

	/**
	 * @brief Grid Target Action
	 */
	bool RequestGridAction(
		PlayerActionType type,
		const GridPosition& target);

	/**
	 * @brief Self Target Action
	 */
	bool RequestSelfAction(
		PlayerActionType type);

	/**
	 * @brief Direction Target Action
	 */
	bool RequestDirectionAction(
		PlayerActionType type,
		GridDirection direction);

	//=====================================================
	// Skill Action
	//=====================================================
	/**
	 * @brief Unit Target Skill実行
	 */
	bool ExecuteUnitSkill(
		PlayerActionType type,
		GameObject* target);

	/**
	 * @brief Direction Skill実行
	 */
	bool ExecuteDirectionSkill(
		PlayerActionType type,
		GridDirection direction);

	/**
	 * @brief Self Skill実行
	 */
	bool ExecuteSelfSkill(
		PlayerActionType type);

	/**
	 * @brief EffectDefinitionをRuntime値へ変換
	 *
	 * AttackPower倍率などをここで解決する。
	 */
	EffectDefinition BuildRuntimeEffect(
		const EffectDefinition& definition) const;

	/**
	 * @brief 単一TargetへAction Effectsを適用
	 */
	bool ApplyActionEffectsToTarget(
		const PlayerActionDefinition& definition,
		GameObject* target);

	/**
	 * @brief EffectPatternにAction Effectsを適用
	 */
	bool ApplyActionEffectsToPattern(
		const PlayerActionDefinition& definition,
		const GridPosition& origin,
		GridDirection direction);

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

	//=================================================
	// Movement
	//=================================================

	/**
	 * @brief 現在移動中のPath
	 *
	 * index 0は開始地点。
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
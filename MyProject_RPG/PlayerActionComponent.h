#pragma once

/**
 * @file PlayerActionComponent.h
 * @brief Player Actionの共通実行管理
 */

#include <cstdint>

#include "Component.h"
#include "PlayerActionDefinition.h"

class CharacterStatusComponent;

/**
 * @brief Player Action Component
 *
 * @details
 * PlayerのAction実行可否と、
 * AP / MP消費を共通管理する。
 *
 * PlayerControllerは入力・選択を担当し、
 * 実行条件判定はこのComponentへ委譲する。
 */
class PlayerActionComponent :
	public Component
{
public:

	PlayerActionComponent() = default;

	~PlayerActionComponent() override =
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
	// Action Definition
	//=================================================

	/**
	 * @brief Action定義を取得
	 */
	PlayerActionDefinition GetActionDefinition(
		PlayerActionType type) const;

	//=================================================
	// Validation
	//=================================================

	/**
	 * @brief 指定Actionを実行可能か
	 */
	bool CanUseAction(
		PlayerActionType type) const;

	/**
	 * @brief 指定Costを支払えるか
	 */
	bool CanPayCost(
		const PlayerActionCost& cost) const;

	//=================================================
	// Cost
	//=================================================

	/**
	 * @brief Action Costを消費
	 *
	 * @return
	 * 消費成功時true
	 */
	bool ConsumeActionCost(
		PlayerActionType type);

	/**
	 * @brief 指定Costを消費
	 *
	 * @note
	 * AP / MP両方を確認後にまとめて消費する。
	 */
	bool ConsumeCost(
		const PlayerActionCost& cost);

	//=================================================
	// Turn
	//=================================================

	/**
	 * @brief PlayerTurn開始
	 *
	 * APを最大値まで回復する。
	 */
	void BeginTurn();

	/**
	 * @brief 現在何かActionを実行可能か
	 *
	 * @details
	 * 自動Turn終了判定等に使用する。
	 */
	bool HasUsableAction() const;

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "PlayerActionComponent";
	}

private:

	/**
	 * @brief CharacterStatus取得
	 */
	CharacterStatusComponent*
		GetStatus() const;
};
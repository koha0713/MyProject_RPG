#pragma once

/**
 * @file PlayerControllerComponent.h
 * @brief Playerの入力と行動要求を管理するComponent
 */

#include "Component.h"
#include "GridPosition.h"
#include "TurnManager.h"

class GameObject;

 /**
  * @brief Player Controller
  *
  * @details
  * Keyboard / Mouseなどの入力を、
  * Game上の行動要求へ変換する。
  *
  * Gridの実際の座標管理は
  * GridPositionComponentへ委譲する。
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

	void SetTurnManager(
		TurnManager* turnManager)
	{
		m_TurnManager =
			turnManager;
	}

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "PlayerControllerComponent";
	}

	void SetAttackTarget(
		GameObject* target)
	{
		m_AttackTarget =
			target;
	}

	bool RequestAttack();

private:

	TurnManager* m_TurnManager =
		nullptr;
	GameObject* m_AttackTarget =
		nullptr;
};
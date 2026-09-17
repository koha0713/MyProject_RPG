#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "GridPositionComponent.h"
#include "InputManager.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void PlayerControllerComponent::Initialize()
{
}

void PlayerControllerComponent::Finalize()
{
}

void PlayerControllerComponent::Update(
	uint64_t delta)
{
	(void)delta;

	if (!m_TurnManager ||
		!m_TurnManager->IsPlayerTurn())
	{
		return;
	}
	//=================================================
	// Keyboard Input
	//=================================================
	//
	// 現段階では動作確認のためWASDを使用する。
	//
	// 実際のGrid移動処理はRequestMove()へ集約し、
	// 将来Mouse操作へ変更しても移動処理そのものは
	// 変更しなくて済むようにする。
	//

	//====================
	// Attack
	//====================

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::Space))
	{
		RequestAttack();

		return;
	}

	//====================
	// Movement
	//====================

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::S))
	{
		RequestMove(
			GridPosition
			{
				0,
				1
			});

		return;
	}

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::W))
	{
		RequestMove(
			GridPosition
			{
				0,
				-1
			});

		return;
	}

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::A))
	{
		RequestMove(
			GridPosition
			{
				-1,
				0
			});

		return;
	}

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::D))
	{
		RequestMove(
			GridPosition
			{
				1,
				0
			});

		return;
	}


}

void PlayerControllerComponent::Draw()
{
}

//=====================================================
// Movement
//=====================================================

bool PlayerControllerComponent::RequestMove(
	const GridPosition& direction)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	//=================================================
	// GridPositionComponent取得
	//=================================================

	GridPositionComponent* gridPosition =
		owner->GetComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"GridPositionComponent not found.\n");

		return false;
	}

	//=================================================
	// Grid移動
	//=================================================

	const bool moved =
		gridPosition->TryMove(
			direction);

	if (!moved)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move blocked.\n");

		return false;
	}

	// 移動に成功した場合のみターン終了
	if (m_TurnManager)
	{
		m_TurnManager->
			EndPlayerTurn();
	}

	return true;
}

//=====================================================
// Debug UI
//=====================================================

void PlayerControllerComponent::DrawDebugUI()
{
	ImGui::Text(
		"Control: Keyboard");

	ImGui::Text(
		"W / A / S / D");

	ImGui::Text(
		"Move: 1 Grid Cell");
}

bool PlayerControllerComponent::RequestAttack()
{
	GameObject* owner =
		GetOwner();

	if (!owner ||
		!m_AttackTarget)
	{
		return false;
	}

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	auto* enemyGrid =
		m_AttackTarget->GetComponent<
		GridPositionComponent>();

	auto* enemyStatus =
		m_AttackTarget->GetComponent<
		CharacterStatusComponent>();

	if (!playerGrid ||
		!playerStatus ||
		!enemyGrid ||
		!enemyStatus)
	{
		return false;
	}

	if (enemyStatus->IsDead())
	{
		return false;
	}

	const GridPosition playerPosition =
		playerGrid->GetGridPosition();

	const GridPosition enemyPosition =
		enemyGrid->GetGridPosition();

	const int dx =
		enemyPosition.X -
		playerPosition.X;

	const int dy =
		enemyPosition.Y -
		playerPosition.Y;

	const int distance =
		std::abs(dx) +
		std::abs(dy);

	// 隣接していない場合は攻撃不可
	if (distance != 1)
	{
		OutputDebugStringA(
			"[PlayerController] Attack failed: Target is not adjacent.\n");

		return false;
	}

	const int damage =
		playerStatus->GetAttackPower();

	enemyStatus->TakeDamage(
		damage);

	OutputDebugStringA(
		"[PlayerController] Attack success.\n");

	// 攻撃成功時のみターン終了
	if (m_TurnManager)
	{
		m_TurnManager->EndPlayerTurn();
	}

	return true;
}
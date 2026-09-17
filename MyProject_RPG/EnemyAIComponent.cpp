#include "EnemyAIComponent.h"

#include <cstdlib>

#include "GameObject.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void EnemyAIComponent::Initialize()
{
}

void EnemyAIComponent::Finalize()
{
}

void EnemyAIComponent::Update(
	uint64_t delta)
{
	(void)delta;

	// Enemyの行動はTurnManager側から
	// 明示的にAct()を呼ぶため、
	// 通常Updateでは処理しない。
}

void EnemyAIComponent::Draw()
{
}

//=====================================================
// Action
//=====================================================
bool EnemyAIComponent::Act(
	GameObject* player)
{
	if (!player)
	{
		return false;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	//=================================================
	// 必要Component取得
	//=================================================

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* enemyStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	auto* playerGrid =
		player->GetComponent<
		GridPositionComponent>();

	auto* playerStatus =
		player->GetComponent<
		CharacterStatusComponent>();

	if (!enemyGrid ||
		!enemyStatus ||
		!playerGrid ||
		!playerStatus)
	{
		OutputDebugStringA(
			"[EnemyAI] Required Component not found.\n");

		return false;
	}

	// 死亡済みEnemyは行動しない。
	if (enemyStatus->IsDead())
	{
		return false;
	}

	// 死亡済みPlayerにも行動しない。
	if (playerStatus->IsDead())
	{
		return false;
	}

	const GridPosition enemyPosition =
		enemyGrid->GetGridPosition();

	const GridPosition playerPosition =
		playerGrid->GetGridPosition();

	const int dx =
		playerPosition.X -
		enemyPosition.X;

	const int dy =
		playerPosition.Y -
		enemyPosition.Y;

	//=================================================
	// Manhattan Distance
	//=================================================

	const int distance =
		std::abs(dx) +
		std::abs(dy);

	//=================================================
	// 隣接していれば攻撃
	//=================================================

	if (distance == 1)
	{
		const int damage =
			enemyStatus->
			GetAttackPower();

		playerStatus->
			TakeDamage(
				damage);

		char buffer[128]{};

		sprintf_s(
			buffer,
			"[EnemyAI] Attack Player Damage=%d\n",
			damage);

		OutputDebugStringA(
			buffer);

		return true;
	}

	//=================================================
	// 離れていれば接近
	//=================================================

	return TryMoveToward(
		playerPosition);
}

//=====================================================
// Movement
//=====================================================

bool EnemyAIComponent::TryMoveToward(
	const GridPosition& playerPosition)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	GridPositionComponent* gridPosition =
		owner->GetComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		return false;
	}

	const GridPosition enemyPosition =
		gridPosition->GetGridPosition();

	const int dx =
		playerPosition.X -
		enemyPosition.X;

	const int dy =
		playerPosition.Y -
		enemyPosition.Y;

	//=================================================
	// 優先方向決定
	//=================================================
	//
	// Playerとの差が大きい軸を優先する。
	//

	if (std::abs(dx) >
		std::abs(dy))
	{
		const GridPosition xDirection
		{
			dx > 0 ? 1 : -1,
			0
		};

		if (gridPosition->TryMove(
			xDirection))
		{
			return true;
		}

		// X方向が塞がれていればY方向を試す
		if (dy != 0)
		{
			const GridPosition yDirection
			{
				0,
				dy > 0 ? 1 : -1
			};

			if (gridPosition->TryMove(
				yDirection))
			{
				return true;
			}
		}
	}
	else
	{
		const GridPosition yDirection
		{
			0,
			dy > 0 ? 1 : -1
		};

		if (dy != 0 &&
			gridPosition->TryMove(
				yDirection))
		{
			return true;
		}

		// Y方向が塞がれていればX方向を試す
		if (dx != 0)
		{
			const GridPosition xDirection
			{
				dx > 0 ? 1 : -1,
				0
			};

			if (gridPosition->TryMove(
				xDirection))
			{
				return true;
			}
		}
	}

	OutputDebugStringA(
		"[EnemyAI] Move failed.\n");

	return false;
}

//=====================================================
// Debug UI
//=====================================================

void EnemyAIComponent::DrawDebugUI()
{
	ImGui::Text(
		"AI Type: Chase Player");

	ImGui::Text(
		"Movement: 1 Cell / Turn");
}
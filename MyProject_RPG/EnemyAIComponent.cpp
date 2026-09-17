#include "EnemyAIComponent.h"

#include <cstdlib>
#include <limits>

#include "GameObject.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "CharacterAnimationComponent.h"
#include "TransformComponent.h"
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

	//=================================================
	// Attack Animation終了待ち
	//=================================================

	if (m_ActionState !=
		EnemyActionState::Attacking)
	{
		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	auto* characterAnimation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	// AnimationComponentが無い場合でも
	// EnemyTurnを停止させない。
	if (!characterAnimation)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	// CharacterAnimationComponent側で
	// Attack終了後にIdleへ戻るため、
	// Attack状態でなくなったら行動終了。
	if (!characterAnimation->
		IsAttacking())
	{
		m_ActionState =
			EnemyActionState::Finished;
	}
}

void EnemyAIComponent::Draw()
{
}

//=====================================================
// Action
//=====================================================

bool EnemyAIComponent::Act(
	const std::vector<GameObject*>& players)
{
	//=================================================
	// 既に行動開始済み
	//=================================================

	if (m_ActionState !=
		EnemyActionState::Ready)
	{
		return false;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	auto* enemyStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	if (!enemyStatus ||
		!enemyGrid)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	if (enemyStatus->IsDead())
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	//=================================================
	// Target検索
	//=================================================

	GameObject* target =
		FindNearestPlayer(
			players);

	if (!target)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	const int agility =
		enemyStatus->GetAgility();

	bool acted =
		false;

	//=================================================
	// Agility分行動
	//=================================================

	for (int moveCount = 0;
		moveCount < agility;
		++moveCount)
	{
		//=============================================
		// 既に隣接していれば攻撃
		//=============================================

		if (IsAdjacent(
			target))
		{
			return TryAttack(
				target);
		}

		auto* playerGrid =
			target->GetComponent<
			GridPositionComponent>();

		if (!playerGrid)
		{
			break;
		}

		const GridPosition playerPosition =
			playerGrid->GetGridPosition();

		//=============================================
		// 1マス移動
		//=============================================

		if (!TryMoveToward(
			playerPosition))
		{
			break;
		}

		acted =
			true;

		//=============================================
		// 移動後に隣接したら攻撃
		//=============================================

		if (IsAdjacent(
			target))
		{
			return TryAttack(
				target);
		}
	}

	// 攻撃しなかった場合は
	// このEnemyの行動終了。
	m_ActionState =
		EnemyActionState::Finished;

	return acted;
}

void EnemyAIComponent::ResetAction()
{
	m_ActionState =
		EnemyActionState::Ready;
}

//=====================================================
// Target Search
//=====================================================

GameObject* EnemyAIComponent::FindNearestPlayer(
	const std::vector<GameObject*>& players) const
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return nullptr;
	}

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	if (!enemyGrid)
	{
		return nullptr;
	}

	const GridPosition enemyPosition =
		enemyGrid->GetGridPosition();

	GameObject* nearestPlayer =
		nullptr;

	int nearestDistance =
		(std::numeric_limits<int>::max)();

	//=================================================
	// 全Playerから最短距離を検索
	//=================================================

	for (GameObject* player :
		players)
	{
		if (!player)
		{
			continue;
		}

		auto* playerGrid =
			player->GetComponent<
			GridPositionComponent>();

		auto* playerStatus =
			player->GetComponent<
			CharacterStatusComponent>();

		if (!playerGrid ||
			!playerStatus)
		{
			continue;
		}

		// 死亡済みPlayerはターゲットにしない。
		if (playerStatus->IsDead())
		{
			continue;
		}

		const GridPosition playerPosition =
			playerGrid->GetGridPosition();

		const int dx =
			playerPosition.X -
			enemyPosition.X;

		const int dy =
			playerPosition.Y -
			enemyPosition.Y;

		const int distance =
			std::abs(dx) +
			std::abs(dy);

		if (distance <
			nearestDistance)
		{
			nearestDistance =
				distance;

			nearestPlayer =
				player;
		}
	}

	return nearestPlayer;
}

//=====================================================
// Attack
//=====================================================

bool EnemyAIComponent::IsAdjacent(
	GameObject* player) const
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

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerGrid =
		player->GetComponent<
		GridPositionComponent>();

	if (!enemyGrid ||
		!playerGrid)
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

	const int distance =
		std::abs(dx) +
		std::abs(dy);

	return
		distance == 1;
}

bool EnemyAIComponent::TryAttack(
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

	auto* enemyStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	auto* playerStatus =
		player->GetComponent<
		CharacterStatusComponent>();

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerGrid =
		player->GetComponent<
		GridPositionComponent>();

	if (!enemyStatus ||
		!playerStatus ||
		!enemyGrid ||
		!playerGrid)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	if (enemyStatus->IsDead() ||
		playerStatus->IsDead())
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	if (!IsAdjacent(
		player))
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	const GridPosition enemyPosition =
		enemyGrid->GetGridPosition();

	const GridPosition playerPosition =
		playerGrid->GetGridPosition();

	//=================================================
	// Player方向を向く
	//=================================================

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (transform)
	{
		transform->
			FaceGridPosition(
				enemyPosition,
				playerPosition);
	}

	//=================================================
	// Attack Animation
	//=================================================

	auto* characterAnimation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (characterAnimation)
	{
		characterAnimation->
			ChangeState(
				CharacterAnimationState::Attack);

		// Animation終了待ちへ。
		m_ActionState =
			EnemyActionState::Attacking;
	}
	else
	{
		// Animationが無い場合は
		// 即座に行動終了扱い。
		m_ActionState =
			EnemyActionState::Finished;
	}

	//=================================================
	// Damage
	//=================================================
	//
	// 現段階ではAttack開始時にDamageを発生させる。
	// 将来Animation Eventで命中タイミングへ移動可能。
	//

	const int damage =
		enemyStatus->GetAttackPower();

	playerStatus->TakeDamage(
		damage);

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[EnemyAI] "
		"Attack Player Damage=%d\n",
		damage);

	OutputDebugStringA(
		buffer);

	return true;
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

	auto* gridPosition =
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
	// 現在はA*導入前の簡易追跡AIなので、
	// 第一候補が塞がれていた場合は
	// もう一方の軸を試す。
	//

	if (std::abs(dx) >
		std::abs(dy))
	{
		//=============================================
		// X方向を優先
		//=============================================

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

		//=============================================
		// Xが塞がれていればY
		//=============================================

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
		//=============================================
		// Y方向を優先
		//=============================================

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

		//=============================================
		// Yが塞がれていればX
		//=============================================

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
		"AI Type: Nearest Player Chase");

	ImGui::Text(
		"Movement: Agility Based");

	ImGui::Text(
		"Attack Range: 1 Grid Cell");
}
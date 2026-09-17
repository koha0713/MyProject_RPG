#include "EnemyAIComponent.h"

#include <cstdlib>
#include <limits>

#include "GameObject.h"
#include "GridMap.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "CharacterAnimationComponent.h"
#include "TransformComponent.h"
#include "DebugUI.h"

#include "AnimatorComponent.h"

//=====================================================
// Lifecycle
//=====================================================

void EnemyAIComponent::Initialize()
{
	m_ActionState =
		EnemyActionState::Ready;

	m_Target =
		nullptr;

	m_RemainingMovePoints =
		0;
}

void EnemyAIComponent::Finalize()
{
	m_Target =
		nullptr;
}

void EnemyAIComponent::Update(
	uint64_t delta)
{
	//=================================================
	// Moving
	//=================================================

	if (m_ActionState ==
		EnemyActionState::Moving)
	{
		UpdateMovement(
			delta);

		return;
	}

	//=================================================
	// Attacking
	//=================================================

	if (m_ActionState ==
		EnemyActionState::Attacking)
	{
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

		if (!characterAnimation)
		{
			m_ActionState =
				EnemyActionState::Finished;

			return;
		}

		// Attack Animation終了後、
		// CharacterAnimationComponentがIdleへ戻る。
		if (!characterAnimation->
			IsAttacking())
		{
			m_ActionState =
				EnemyActionState::Finished;

			m_Target =
				nullptr;
		}

		return;
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

	if (!enemyStatus ||
		enemyStatus->IsDead())
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	//=================================================
	// Target選択
	//=================================================

	m_Target =
		FindNearestPlayer(
			players);

	if (!m_Target)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return false;
	}

	//=================================================
	// 移動可能回数
	//=================================================

	m_RemainingMovePoints =
		enemyStatus->GetAgility();

	//=================================================
	// 既に隣接していれば攻撃
	//=================================================

	if (IsAdjacent(
		m_Target))
	{
		return TryAttack(
			m_Target);
	}

	//=================================================
	// 最初の移動開始
	//=================================================

	ContinueAction();

	return
		m_ActionState !=
		EnemyActionState::Finished;
}
void EnemyAIComponent::ResetAction()
{
	m_ActionState =
		EnemyActionState::Ready;

	m_Target =
		nullptr;

	m_RemainingMovePoints =
		0;
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
bool EnemyAIComponent::FindNextMovePosition(
	const GridPosition& playerPosition,
	GridPosition& outNextPosition)
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

	GridMap* gridMap =
		gridPosition->GetGridMap();

	if (!gridMap)
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
	// X方向優先
	//=================================================

	if (std::abs(dx) >
		std::abs(dy))
	{
		if (dx != 0)
		{
			const GridPosition candidate
			{
				enemyPosition.X +
					(dx > 0 ? 1 : -1),

				enemyPosition.Y
			};

			if (gridMap->CanMoveTo(
				candidate))
			{
				outNextPosition =
					candidate;

				return true;
			}
		}

		if (dy != 0)
		{
			const GridPosition candidate
			{
				enemyPosition.X,

				enemyPosition.Y +
					(dy > 0 ? 1 : -1)
			};

			if (gridMap->CanMoveTo(
				candidate))
			{
				outNextPosition =
					candidate;

				return true;
			}
		}
	}
	else
	{
		//=================================================
		// Y方向優先
		//=================================================

		if (dy != 0)
		{
			const GridPosition candidate
			{
				enemyPosition.X,

				enemyPosition.Y +
					(dy > 0 ? 1 : -1)
			};

			if (gridMap->CanMoveTo(
				candidate))
			{
				outNextPosition =
					candidate;

				return true;
			}
		}

		if (dx != 0)
		{
			const GridPosition candidate
			{
				enemyPosition.X +
					(dx > 0 ? 1 : -1),

				enemyPosition.Y
			};

			if (gridMap->CanMoveTo(
				candidate))
			{
				outNextPosition =
					candidate;

				return true;
			}
		}
	}

	return false;
}

bool EnemyAIComponent::BeginMove(
	const GridPosition& nextPosition)
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

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (!gridPosition ||
		!transform)
	{
		return false;
	}

	const GridPosition currentPosition =
		gridPosition->GetGridPosition();

	//=================================================
	// 移動方向を向く
	//=================================================

	transform->
		FaceGridPosition(
			currentPosition,
			nextPosition);

	//=================================================
	// Move Animation開始
	//=================================================

	auto* animation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (animation)
	{
		animation->
			ChangeState(
				CharacterAnimationState::Move);
	}

	m_NextGridPosition =
		nextPosition;

	m_ActionState =
		EnemyActionState::Moving;

	return true;
}

void EnemyAIComponent::UpdateMovement(
	uint64_t delta)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	auto* gridPosition =
		owner->GetComponent<
		GridPositionComponent>();

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (!gridPosition ||
		!transform)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	GridMap* gridMap =
		gridPosition->GetGridMap();

	if (!gridMap)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	//=================================================
	// 移動先World座標
	//=================================================

	const Vector3 targetPosition =
		gridMap->GridToWorld(
			m_NextGridPosition);

	Vector3 currentPosition =
		transform->GetPosition();

	Vector3 direction =
		targetPosition -
		currentPosition;

	const float distance =
		direction.Length();

	constexpr float ARRIVE_EPSILON =
		0.01f;

	//=================================================
	// 到着
	//=================================================

	if (distance <=
		ARRIVE_EPSILON)
	{
		transform->SetPosition(
			targetPosition);

		// 論理Grid位置を更新。
		if (!gridPosition->
			SetGridPosition(
				m_NextGridPosition))
		{
			m_ActionState =
				EnemyActionState::Finished;

			return;
		}

		--m_RemainingMovePoints;

		// 次の行動を判断。
		ContinueAction();

		return;
	}

	//=================================================
	// Transform補間
	//=================================================

	direction.Normalize();

	// deltaがミリ秒の場合。
	const float deltaSeconds =
		static_cast<float>(
			delta) /
		1000.0f;

	const float moveDistance =
		m_MoveSpeed *
		deltaSeconds;

	if (moveDistance >=
		distance)
	{
		transform->SetPosition(
			targetPosition);
	}
	else
	{
		currentPosition +=
			direction *
			moveDistance;

		transform->SetPosition(
			currentPosition);
	}
}

void EnemyAIComponent::ContinueAction()
{
	GameObject* owner =
		GetOwner();

	if (!owner ||
		!m_Target)
	{
		m_ActionState =
			EnemyActionState::Finished;

		return;
	}

	auto* targetStatus =
		m_Target->GetComponent<
		CharacterStatusComponent>();

	auto* targetGrid =
		m_Target->GetComponent<
		GridPositionComponent>();

	if (!targetStatus ||
		!targetGrid ||
		targetStatus->IsDead())
	{
		m_ActionState =
			EnemyActionState::Finished;

		m_Target =
			nullptr;

		return;
	}

	//=================================================
	// Playerに隣接
	//=================================================

	if (IsAdjacent(
		m_Target))
	{
		TryAttack(
			m_Target);

		return;
	}

	//=================================================
	// MovePoint終了
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		auto* animation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (animation)
		{
			animation->
				ChangeState(
					CharacterAnimationState::Idle);
		}

		m_ActionState =
			EnemyActionState::Finished;

		m_Target =
			nullptr;

		return;
	}

	//=================================================
	// 次のGrid決定
	//=================================================

	const GridPosition playerPosition =
		targetGrid->GetGridPosition();

	GridPosition nextPosition{};

	if (!FindNextMovePosition(
		playerPosition,
		nextPosition))
	{
		// 移動不可。
		auto* animation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (animation)
		{
			animation->
				ChangeState(
					CharacterAnimationState::Idle);
		}

		m_ActionState =
			EnemyActionState::Finished;

		m_Target =
			nullptr;

		return;
	}

	BeginMove(
		nextPosition);
}

//=====================================================
// Debug UI
//=====================================================
void EnemyAIComponent::DrawDebugUI()
{
	GameObject* owner =
		GetOwner();

	//=================================================
	// AI基本情報
	//=================================================

	ImGui::Text(
		"AI Type: Nearest Player Chase");

	ImGui::Text(
		"Movement: Agility Based");

	ImGui::Text(
		"Attack Range: 1 Grid Cell");

	ImGui::Separator();

	//=================================================
	// Action State
	//=================================================

	const char* actionStateName =
		"Unknown";

	switch (m_ActionState)
	{
	case EnemyActionState::Ready:
		actionStateName =
			"Ready";
		break;

	case EnemyActionState::Moving:
		actionStateName =
			"Moving";
		break;

	case EnemyActionState::Attacking:
		actionStateName =
			"Attacking";
		break;

	case EnemyActionState::Finished:
		actionStateName =
			"Finished";
		break;

	default:
		break;
	}

	ImGui::Text(
		"Action State: %s",
		actionStateName);

	ImGui::Text(
		"Remaining Move Points: %d",
		m_RemainingMovePoints);

	ImGui::Text(
		"Move Speed: %.2f",
		m_MoveSpeed);

	ImGui::Separator();

	//=================================================
	// Owner確認
	//=================================================

	if (!owner)
	{
		ImGui::Text(
			"Owner: nullptr");

		return;
	}

	//=================================================
	// Enemy Grid Position
	//=================================================

	auto* enemyGrid =
		owner->GetComponent<
		GridPositionComponent>();

	if (enemyGrid)
	{
		const GridPosition currentGrid =
			enemyGrid->GetGridPosition();

		ImGui::Text(
			"Current Grid: (%d, %d)",
			currentGrid.X,
			currentGrid.Y);

		ImGui::Text(
			"Next Grid: (%d, %d)",
			m_NextGridPosition.X,
			m_NextGridPosition.Y);
	}
	else
	{
		ImGui::Text(
			"GridPositionComponent: None");
	}

	//=================================================
	// Transform
	//=================================================

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (transform)
	{
		const Vector3 worldPosition =
			transform->GetPosition();

		ImGui::Text(
			"World Position: "
			"(%.2f, %.2f, %.2f)",
			worldPosition.x,
			worldPosition.y,
			worldPosition.z);

		//=============================================
		// 移動中なら目的World座標までの距離も表示
		//=============================================

		if (enemyGrid &&
			m_ActionState ==
			EnemyActionState::Moving)
		{
			GridMap* gridMap =
				enemyGrid->GetGridMap();

			if (gridMap)
			{
				const Vector3 targetWorld =
					gridMap->GridToWorld(
						m_NextGridPosition);

				const Vector3 difference =
					targetWorld -
					worldPosition;

				const float distance =
					difference.Length();

				ImGui::Text(
					"Target World: "
					"(%.2f, %.2f, %.2f)",
					targetWorld.x,
					targetWorld.y,
					targetWorld.z);

				ImGui::Text(
					"Distance To Next: %.4f",
					distance);

				ImGui::Text(
					"Next Cell Movable: %s",
					gridMap->CanMoveTo(
						m_NextGridPosition)
					? "true"
					: "false");
			}
		}
	}

	ImGui::Separator();

	//=================================================
	// Target
	//=================================================

	if (m_Target)
	{
		ImGui::Text(
			"Target: Valid");

		auto* targetGrid =
			m_Target->GetComponent<
			GridPositionComponent>();

		auto* targetStatus =
			m_Target->GetComponent<
			CharacterStatusComponent>();

		if (targetGrid)
		{
			const GridPosition targetPosition =
				targetGrid->GetGridPosition();

			ImGui::Text(
				"Target Grid: (%d, %d)",
				targetPosition.X,
				targetPosition.Y);
		}

		if (targetStatus)
		{
			ImGui::Text(
				"Target Alive: %s",
				targetStatus->IsDead()
				? "false"
				: "true");
		}

		ImGui::Text(
			"Adjacent: %s",
			IsAdjacent(
				m_Target)
			? "true"
			: "false");
	}
	else
	{
		ImGui::Text(
			"Target: nullptr");
	}

	ImGui::Separator();

	//=================================================
	// Animation
	//=================================================

	auto* characterAnimation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (characterAnimation)
	{
		const char* animationState =
			"Unknown";

		switch (
			characterAnimation->GetState())
		{
		case CharacterAnimationState::Idle:
			animationState =
				"Idle";
			break;

		case CharacterAnimationState::Move:
			animationState =
				"Move";
			break;

		case CharacterAnimationState::Attack:
			animationState =
				"Attack";
			break;

		case CharacterAnimationState::Dead:
			animationState =
				"Dead";
			break;

		default:
			break;
		}

		ImGui::Text(
			"Animation State: %s",
			animationState);
	}
	else
	{
		ImGui::Text(
			"CharacterAnimation: None");
	}

	//=================================================
// Animator
//=================================================

	auto* animator =
		owner->GetComponent<
		AnimatorComponent>();

	if (animator)
	{
		ImGui::Separator();

		ImGui::Text(
			"Animator");

		const AnimationID currentAnimation =
			animator->GetCurrentAnimation();

		ImGui::Text(
			"Animation ID: %d",
			static_cast<int>(
				currentAnimation));

		ImGui::Text(
			"Playing: %s",
			animator->IsPlaying()
			? "true"
			: "false");

		ImGui::Text(
			"Current Time: %.3f",
			animator->GetCurrentTime());

		ImGui::Text(
			"Finished: %s",
			animator->IsAnimationFinished()
			? "true"
			: "false");

		const AnimationClip* clip =
			animator->GetCurrentClip();

		if (clip)
		{
			ImGui::Text(
				"Clip Name: %s",
				clip->Name.c_str());

			ImGui::Text(
				"Clip Duration: %.3f",
				clip->Duration);

			ImGui::Text(
				"Ticks Per Second: %.3f",
				clip->TicksPerSecond);
		}
		else
		{
			ImGui::Text(
				"Clip: nullptr");
		}
	}
	else
	{
		ImGui::Text(
			"AnimatorComponent: None");
	}
}
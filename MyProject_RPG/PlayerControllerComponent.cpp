#include "PlayerControllerComponent.h"

#include <cstdlib>

#include "GameObject.h"

#include "GridMap.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "CharacterAnimationComponent.h"
#include "TransformComponent.h"
#include "PlayerClassComponent.h"

#include "GridPathFinder.h"

#include "InputManager.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void PlayerControllerComponent::Initialize()
{
	m_RemainingMovePoints =
		0;

	m_ActionState =
		PlayerActionState::Idle;

	m_MovePath.clear();

	m_MovePathIndex =
		0;

	m_HasActed =
		false;

	m_AttackTarget =
		nullptr;

	m_ClassAttackActive =
		false;
}

//=====================================================
// Finalize
//=====================================================

void PlayerControllerComponent::Finalize()
{
	m_TurnManager =
		nullptr;

	m_MovePath.clear();

	m_MovePathIndex =
		0;

	m_AttackTarget =
		nullptr;

	m_ClassAttackActive =
		false;

	m_ActionState =
		PlayerActionState::Idle;
}

//=====================================================
// Update
//=====================================================

void PlayerControllerComponent::Update(
	uint64_t delta)
{
	//=================================================
	// TurnManager確認
	//=================================================

	if (!m_TurnManager)
	{
		return;
	}

	// PlayerTurn以外では操作処理しない。
	if (!m_TurnManager->
		IsPlayerTurn())
	{
		return;
	}

	//=================================================
	// PlayerTurn開始検出
	//=================================================

	const uint64_t currentTurn =
		m_TurnManager->
		GetTurnCount();

	if (m_LastTurnCount !=
		currentTurn)
	{
		m_LastTurnCount =
			currentTurn;

		BeginPlayerTurn();
	}

	//=================================================
	// Moving
	//=================================================

	if (m_ActionState ==
		PlayerActionState::Moving)
	{
		UpdateMovement(
			delta);

		return;
	}

	//=================================================
	// Attacking
	//=================================================

	if (m_ActionState ==
		PlayerActionState::Attacking)
	{
		UpdateClassAttack();

		// 攻撃中は追加入力を受け付けない。
		return;
	}

	//=================================================
	// 移動力を使い切った場合
	//=================================================
	//
	// Controller単体ではTurnを終了しない。
	//
	// 複数Player全体のTurn終了判定は
	// FieldScene::CheckPlayerTurnEnd()へ委譲する。
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		m_RemainingMovePoints =
			0;

		return;
	}
}

//=====================================================
// Draw
//=====================================================

void PlayerControllerComponent::Draw()
{
}

//=====================================================
// Player Turn
//=====================================================

void PlayerControllerComponent::BeginPlayerTurn()
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	//=================================================
	// Action Reset
	//=================================================

	m_ActionState =
		PlayerActionState::Idle;

	m_MovePath.clear();

	m_MovePathIndex =
		0;

	m_AttackTarget =
		nullptr;

	m_ClassAttackActive =
		false;

	//=================================================
	// Character Status
	//=================================================

	auto* status =
		owner->GetComponent<
		CharacterStatusComponent>();

	if (!status)
	{
		// Statusが無い場合でも
		// 最低1マス動けるようにする。
		m_RemainingMovePoints =
			1;

		return;
	}

	//=================================================
	// Agility -> Move Points
	//=================================================

	m_RemainingMovePoints =
		status->GetAgility();

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[PlayerTurn] Begin MovePoints=%d\n",
		m_RemainingMovePoints);

	OutputDebugStringA(
		buffer);
}

//=====================================================
// Start Player Turn
//=====================================================

void PlayerControllerComponent::StartPlayerTurn(
	uint64_t turnCount)
{
	//=================================================
	// Turn番号記録
	//=================================================

	m_LastTurnCount =
		turnCount;

	//=================================================
	// 行動状態Reset
	//=================================================

	m_HasActed =
		false;

	//=================================================
	// Turn開始
	//=================================================

	BeginPlayerTurn();
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
	// PlayerTurn確認
	//=================================================

	if (!m_TurnManager ||
		!m_TurnManager->
		IsPlayerTurn())
	{
		return false;
	}

	// 行動中は新しい要求を受け付けない。
	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	//=================================================
	// 移動力確認
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		return false;
	}

	//=================================================
	// GridPosition
	//=================================================

	auto* gridPosition =
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

	// 壁・Actor等で移動できなければ
	// MovePointを消費しない。
	if (!moved)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move blocked.\n");

		return false;
	}

	//=================================================
	// MovePoint
	//=================================================

	--m_RemainingMovePoints;

	if (m_RemainingMovePoints < 0)
	{
		m_RemainingMovePoints =
			0;
	}

	m_HasActed =
		true;

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[PlayerController] "
		"Move success. Remaining=%d\n",
		m_RemainingMovePoints);

	OutputDebugStringA(
		buffer);

	//=================================================
	// Turn終了はFieldSceneへ委譲
	//=================================================

	return true;
}

//=====================================================
// Request Move To
//=====================================================

bool PlayerControllerComponent::RequestMoveTo(
	const GridPosition& destination)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	//=================================================
	// PlayerTurn確認
	//=================================================

	if (!m_TurnManager ||
		!m_TurnManager->
		IsPlayerTurn())
	{
		return false;
	}

	//=================================================
	// Action確認
	//=================================================

	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	//=================================================
	// MovePoint確認
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		return false;
	}

	//=================================================
	// GridPosition
	//=================================================

	auto* gridPosition =
		owner->GetComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		return false;
	}

	GridMap* gridMap =
		gridPosition->
		GetGridMap();

	if (!gridMap)
	{
		return false;
	}

	const GridPosition start =
		gridPosition->
		GetGridPosition();

	if (start ==
		destination)
	{
		return false;
	}

	//=================================================
	// A* Path
	//=================================================

	const std::vector<GridPosition> path =
		GridPathFinder::FindPath(
			*gridMap,
			start,
			destination);

	if (path.empty())
	{
		return false;
	}

	//=================================================
	// 必要MovePoint
	//=================================================

	const int requiredMovePoints =
		static_cast<int>(
			path.size()) -
		1;

	if (requiredMovePoints <= 0)
	{
		return false;
	}

	// 残り移動力で目的地まで届かない場合は
	// 一切移動しない。
	if (requiredMovePoints >
		m_RemainingMovePoints)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move failed: "
			"Destination out of range.\n");

		return false;
	}

	//=================================================
	// Begin Move
	//=================================================

	if (!BeginMove(
		path))
	{
		return false;
	}

	// 実際に移動開始できた場合のみ
	// 行動済みにする。
	m_HasActed =
		true;

	return true;
}

//=====================================================
// Attack - Auto Target
//=====================================================

bool PlayerControllerComponent::RequestAttack()
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	//=================================================
	// PlayerTurn
	//=================================================

	if (!m_TurnManager ||
		!m_TurnManager->
		IsPlayerTurn())
	{
		return false;
	}

	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	//=================================================
	// Component
	//=================================================

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerClass =
		owner->GetComponent<
		PlayerClassComponent>();

	if (!playerGrid ||
		!playerClass)
	{
		return false;
	}

	GridMap* gridMap =
		playerGrid->
		GetGridMap();

	if (!gridMap)
	{
		return false;
	}

	const GridPosition playerPosition =
		playerGrid->
		GetGridPosition();

	const int attackRange =
		playerClass->
		GetNormalAttackRange();

	//=================================================
	// Attack Range内Enemy検索
	//=================================================
	//
	// Manhattan距離1から順番に検索する。
	// そのため最も近いEnemyを優先する。
	//=================================================

	for (int distance = 1;
		distance <= attackRange;
		++distance)
	{
		for (int dx = -distance;
			dx <= distance;
			++dx)
		{
			const int remaining =
				distance -
				std::abs(dx);

			//=========================================
			// 同じdistanceに存在する
			// 上下2地点を確認する。
			//=========================================

			const int dyValues[] =
			{
				-remaining,
				remaining
			};

			for (int i = 0;
				i < 2;
				++i)
			{
				const int dy =
					dyValues[i];

				// remaining == 0の場合、
				// 同じGridを2回確認しない。
				if (i == 1 &&
					remaining == 0)
				{
					continue;
				}

				const GridPosition targetPosition
				{
					playerPosition.X +
						dx,

					playerPosition.Y +
						dy
				};

				GameObject* occupant =
					gridMap->
					GetOccupant(
						targetPosition);

				if (!occupant)
				{
					continue;
				}

				if (occupant->
					GetTag() !=
					Tag::Enemy)
				{
					continue;
				}

				// 実際の攻撃条件チェックは
				// Target指定版へ委譲。
				return RequestAttack(
					occupant);
			}
		}
	}

	OutputDebugStringA(
		"[PlayerController] "
		"Attack failed: "
		"No enemy in attack range.\n");

	return false;
}

//=====================================================
// Attack - Target
//=====================================================

bool PlayerControllerComponent::RequestAttack(
	GameObject* target)
{
	GameObject* owner =
		GetOwner();

	if (!owner ||
		!target)
	{
		return false;
	}

	//=================================================
	// PlayerTurn
	//=================================================

	if (!m_TurnManager ||
		!m_TurnManager->
		IsPlayerTurn())
	{
		return false;
	}

	//=================================================
	// Action State
	//=================================================

	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	if (m_ClassAttackActive)
	{
		return false;
	}

	//=================================================
	// Enemy確認
	//=================================================

	if (target->IsDestroy() ||
		target->GetTag() !=
		Tag::Enemy)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Target is not Enemy.\n");

		return false;
	}

	//=================================================
	// Components
	//=================================================

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* enemyGrid =
		target->GetComponent<
		GridPositionComponent>();

	auto* enemyStatus =
		target->GetComponent<
		CharacterStatusComponent>();

	auto* playerClass =
		owner->GetComponent<
		PlayerClassComponent>();

	if (!playerGrid ||
		!enemyGrid ||
		!enemyStatus ||
		!playerClass)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Required Component not found.\n");

		return false;
	}

	if (enemyStatus->IsDead())
	{
		return false;
	}

	//=================================================
	// Distance
	//=================================================

	const GridPosition playerPosition =
		playerGrid->
		GetGridPosition();

	const GridPosition enemyPosition =
		enemyGrid->
		GetGridPosition();

	const int dx =
		enemyPosition.X -
		playerPosition.X;

	const int dy =
		enemyPosition.Y -
		playerPosition.Y;

	const int distance =
		std::abs(dx) +
		std::abs(dy);

	//=================================================
	// Attack Range
	//=================================================

	const int attackRange =
		playerClass->
		GetNormalAttackRange();

	if (distance <= 0 ||
		distance >
		attackRange)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Target out of attack range.\n");

		return false;
	}

	//=================================================
	// Face Target
	//=================================================

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (transform)
	{
		transform->
			FaceGridPosition(
				playerPosition,
				enemyPosition);
	}

	//=================================================
	// PlayerClass Attack
	//=================================================
	//
	// Warrior:
	// Attack
	//
	// Ranger:
	// Draw -> Release
	//
	// 等の差分はPlayerClassComponent側が担当。
	//=================================================

	if (!playerClass->
		BeginNormalAttack(
			target))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"BeginNormalAttack failed.\n");

		return false;
	}

	//=================================================
	// Attack State
	//=================================================

	m_AttackTarget =
		target;

	m_ClassAttackActive =
		true;

	m_ActionState =
		PlayerActionState::Attacking;

	// 攻撃はそのPlayerの行動を終了させる。
	m_RemainingMovePoints =
		0;

	m_HasActed =
		true;

	OutputDebugStringA(
		"[PlayerController] "
		"Class Attack Started.\n");

	return true;
}

//=====================================================
// Update Class Attack
//=====================================================

void PlayerControllerComponent::
UpdateClassAttack()
{
	if (!m_ClassAttackActive)
	{
		m_ActionState =
			PlayerActionState::Idle;

		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		m_AttackTarget =
			nullptr;

		m_ClassAttackActive =
			false;

		m_ActionState =
			PlayerActionState::Idle;

		return;
	}

	auto* playerClass =
		owner->GetComponent<
		PlayerClassComponent>();

	if (!playerClass)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"PlayerClassComponent lost.\n");

		m_AttackTarget =
			nullptr;

		m_ClassAttackActive =
			false;

		m_ActionState =
			PlayerActionState::Idle;

		return;
	}

	//=================================================
	// Hit Request
	//=================================================
	//
	// PlayerClassComponent側が
	// Animation Sequenceに応じて
	// Hitタイミングを通知する。
	//=================================================

	if (playerClass->
		ConsumeAttackHitRequest())
	{
		if (m_AttackTarget &&
			!m_AttackTarget->
			IsDestroy())
		{
			auto* attackerStatus =
				owner->GetComponent<
				CharacterStatusComponent>();

			auto* targetStatus =
				m_AttackTarget->
				GetComponent<
				CharacterStatusComponent>();

			if (attackerStatus &&
				targetStatus &&
				!targetStatus->
				IsDead())
			{
				//=====================================
				// Damage
				//=====================================
				//
				// 現段階では
				// AttackPower = Damage。
				//
				// 将来的にはBattleCalculator等へ
				// 分離可能。
				//=====================================

				const int damage =
					attackerStatus->
					GetAttackPower();

				targetStatus->
					TakeDamage(
						damage);

				char buffer[128]{};

				sprintf_s(
					buffer,
					"[PlayerController] "
					"Attack Hit. Damage=%d\n",
					damage);

				OutputDebugStringA(
					buffer);
			}
		}
	}

	//=================================================
	// Attack Sequence終了待ち
	//=================================================

	if (!playerClass->
		IsAttackFinished())
	{
		return;
	}

	//=================================================
	// Attack Complete
	//=================================================

	m_AttackTarget =
		nullptr;

	m_ClassAttackActive =
		false;

	m_ActionState =
		PlayerActionState::Idle;

	OutputDebugStringA(
		"[PlayerController] "
		"Class Attack Finished.\n");
}

//=====================================================
// Movement Update
//=====================================================

void PlayerControllerComponent::UpdateMovement(
	uint64_t delta)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
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
			PlayerActionState::Idle;

		m_MovePath.clear();

		return;
	}

	GridMap* gridMap =
		gridPosition->
		GetGridMap();

	if (!gridMap)
	{
		m_ActionState =
			PlayerActionState::Idle;

		m_MovePath.clear();

		return;
	}

	//=================================================
	// Path終了
	//=================================================

	if (m_MovePathIndex >=
		m_MovePath.size())
	{
		m_ActionState =
			PlayerActionState::Idle;

		m_MovePath.clear();

		auto* animation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (animation)
		{
			animation->
				ChangeState(
					CharacterAnimationState::
					Idle);
		}

		return;
	}

	const GridPosition currentGrid =
		gridPosition->
		GetGridPosition();

	const GridPosition nextGrid =
		m_MovePath[
			m_MovePathIndex];

	//=================================================
	// Face Move Direction
	//=================================================

	transform->
		FaceGridPosition(
			currentGrid,
			nextGrid);

	//=================================================
	// Target World Position
	//=================================================

	const Vector3 targetPosition =
		gridMap->GridToWorld(
			nextGrid);

	Vector3 currentPosition =
		transform->
		GetPosition();

	Vector3 direction =
		targetPosition -
		currentPosition;

	const float distance =
		direction.Length();

	//=================================================
	// Arrive
	//=================================================

	constexpr float ARRIVE_EPSILON =
		0.01f;

	if (distance <=
		ARRIVE_EPSILON)
	{
		// 座標誤差を除去するため
		// 完全に目的位置へ合わせる。
		transform->SetPosition(
			targetPosition);

		//=============================================
		// Grid論理位置更新
		//=============================================

		if (!gridPosition->
			SetGridPosition(
				nextGrid))
		{
			OutputDebugStringA(
				"[PlayerController] "
				"SetGridPosition failed.\n");

			m_ActionState =
				PlayerActionState::Idle;

			m_MovePath.clear();

			return;
		}

		//=============================================
		// MovePoint消費
		//=============================================

		--m_RemainingMovePoints;

		if (m_RemainingMovePoints < 0)
		{
			m_RemainingMovePoints =
				0;
		}

		++m_MovePathIndex;

		//=============================================
		// Path終了
		//=============================================

		if (m_MovePathIndex >=
			m_MovePath.size())
		{
			m_ActionState =
				PlayerActionState::Idle;

			m_MovePath.clear();

			auto* animation =
				owner->GetComponent<
				CharacterAnimationComponent>();

			if (animation)
			{
				animation->
					ChangeState(
						CharacterAnimationState::
						Idle);
			}

			//=========================================
			// Turn終了はFieldSceneへ委譲
			//=========================================
			//
			// 複数Player対応のため、
			// 1人の移動力が0になっただけでは
			// EndPlayerTurn()しない。
			//=========================================

			return;
		}

		return;
	}

	//=================================================
	// World Position補間
	//=================================================

	direction.Normalize();

	// 現在のプロジェクトでは
	// deltaをミリ秒として扱う。
	const float deltaSeconds =
		static_cast<float>(
			delta) /
		1000.0f;

	const float moveDistance =
		m_MoveSpeed *
		deltaSeconds;

	//=================================================
	// Overshoot防止
	//=================================================

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

//=====================================================
// Begin Move
//=====================================================

bool PlayerControllerComponent::BeginMove(
	const std::vector<GridPosition>& path)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	// StartしかないPathなら移動不要。
	if (path.size() <= 1)
	{
		return false;
	}

	m_MovePath =
		path;

	// index 0は現在位置。
	m_MovePathIndex =
		1;

	m_ActionState =
		PlayerActionState::Moving;

	//=================================================
	// Move Animation
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

	return true;
}

//=====================================================
// Debug UI
//=====================================================

void PlayerControllerComponent::DrawDebugUI()
{
	ImGui::Text(
		"Control: Mouse / Controller");

	ImGui::Separator();

	//=================================================
	// Action State
	//=================================================

	const char* actionState =
		"Unknown";

	switch (m_ActionState)
	{
	case PlayerActionState::Idle:

		actionState =
			"Idle";

		break;

	case PlayerActionState::Moving:

		actionState =
			"Moving";

		break;

	case PlayerActionState::Attacking:

		actionState =
			"Attacking";

		break;

	default:
		break;
	}

	ImGui::Text(
		"Action State: %s",
		actionState);

	ImGui::Text(
		"Has Acted: %s",
		m_HasActed
		? "Yes"
		: "No");

	ImGui::Text(
		"Remaining Move Points: %d",
		m_RemainingMovePoints);

	ImGui::Text(
		"Class Attack Active: %s",
		m_ClassAttackActive
		? "Yes"
		: "No");

	//=================================================
	// Attack Target
	//=================================================

	if (m_AttackTarget)
	{
		ImGui::Text(
			"Attack Target: %s",
			m_AttackTarget->
			GetName().
			c_str());
	}
	else
	{
		ImGui::Text(
			"Attack Target: None");
	}

	//=================================================
	// Turn
	//=================================================

	if (m_TurnManager)
	{
		ImGui::Separator();

		ImGui::Text(
			"Turn: %llu",
			static_cast<unsigned long long>(
				m_TurnManager->
				GetTurnCount()));

		ImGui::Text(
			"Phase: %s",
			m_TurnManager->
			IsPlayerTurn()
			? "Player"
			: "Enemy");
	}
	else
	{
		ImGui::Text(
			"TurnManager: None");
	}
}
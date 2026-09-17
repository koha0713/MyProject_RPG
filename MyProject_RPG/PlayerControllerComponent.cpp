#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "GridMap.h"
#include "GridPositionComponent.h"
#include "CharacterStatusComponent.h"
#include "CharacterAnimationComponent.h"
#include "TransformComponent.h"
#include "GridPathFinder.h"
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
	m_TurnManager =
		nullptr;
}

void PlayerControllerComponent::Update(
	uint64_t delta)
{
	(void)delta;

	//=================================================
	// TurnManager確認
	//=================================================

	if (!m_TurnManager)
	{
		return;
	}

	// PlayerTurn以外では入力を受け付けない。
	if (!m_TurnManager->IsPlayerTurn())
	{
		return;
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
	// Attack中
	//=================================================

	if (m_ActionState ==
		PlayerActionState::Attacking)
	{
		GameObject* owner =
			GetOwner();

		if (!owner)
		{
			return;
		}

		auto* characterAnimation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (!characterAnimation)
		{
			// AnimationComponentが無い場合は
			// 行動を停止させないためターン終了。
			m_ActionState =
				PlayerActionState::Idle;

			m_TurnManager->
				EndPlayerTurn();

			return;
		}

		// CharacterAnimationComponentは
		// Attack Animation終了時にIdleへ戻る。
		if (!characterAnimation->
			IsAttacking())
		{
			m_ActionState =
				PlayerActionState::Idle;

			m_TurnManager->
				EndPlayerTurn();
		}

		// Attack中は追加入力を受け付けない。
		return;
	}

	//=================================================
	// PlayerTurn開始検出
	//=================================================

	const uint64_t currentTurn =
		m_TurnManager->GetTurnCount();

	if (m_LastTurnCount !=
		currentTurn)
	{
		m_LastTurnCount =
			currentTurn;

		BeginPlayerTurn();
	}

	//=================================================
	// Turn End
	//=================================================
	//
	// 移動力を使い切る前でも、
	// EnterでlayerTurnを終了できる。
	//

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::Enter))
	{
		m_RemainingMovePoints =
			0;

		m_TurnManager->
			EndPlayerTurn();

		return;
	}

	//=================================================
	// 移動力を使い切った
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		m_TurnManager->
			EndPlayerTurn();

		return;
	}
}

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
	// Character Status
	//=================================================

	auto* status =
		owner->GetComponent<
		CharacterStatusComponent>();

	if (!status)
	{
		// Statusが無い場合でも最低1マスは動けるようにする。
		m_RemainingMovePoints =
			1;

		return;
	}

	//=================================================
	// 敏捷値から移動可能回数を決定
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
	// 移動可能回数確認
	//=================================================

	if (m_RemainingMovePoints <= 0)
	{
		return false;
	}

	//=================================================
	// GridPositionComponent取得
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

	// 壁・他Actorなどによって移動できなかった場合は
	// MovePointを消費しない。
	if (!moved)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move blocked.\n");

		return false;
	}

	//=================================================
	// MovePoint消費
	//=================================================

	--m_RemainingMovePoints;

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[PlayerController] Move success. Remaining=%d\n",
		m_RemainingMovePoints);

	OutputDebugStringA(
		buffer);

	//=================================================
	// 移動力を使い切ったらEnemyTurnへ
	//=================================================

	if (m_RemainingMovePoints <= 0 &&
		m_TurnManager)
	{
		m_TurnManager->
			EndPlayerTurn();
	}

	return true;
}

bool PlayerControllerComponent::RequestMoveTo(
	const GridPosition& destination)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	if (!m_TurnManager ||
		!m_TurnManager->IsPlayerTurn())
	{
		return false;
	}

	// 行動中は新しい移動要求を受け付けない。
	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	if (m_RemainingMovePoints <= 0)
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

	const GridPosition start =
		gridPosition->GetGridPosition();

	if (start == destination)
	{
		return false;
	}

	//=================================================
	// A* Path探索
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
	// 必要移動力確認
	//=================================================

	const int requiredMovePoints =
		static_cast<int>(
			path.size()) - 1;

	// 残り移動力で目的地まで届かない場合は
	// 一切移動しない。
	if (requiredMovePoints >
		m_RemainingMovePoints)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move failed: Destination out of range.\n");

		return false;
	}

	//=================================================
	// 移動開始
	//=================================================

	return BeginMove(
		path);
}

//=====================================================
// Attack
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
	// Player Component取得
	//=================================================

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	if (!playerGrid ||
		!playerStatus)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Required Player Component not found.\n");

		return false;
	}

	//=================================================
	// GridMap取得
	//=================================================

	GridMap* gridMap =
		playerGrid->GetGridMap();

	if (!gridMap)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"GridMap not found.\n");

		return false;
	}

	const GridPosition playerPosition =
		playerGrid->GetGridPosition();

	//=================================================
	// 隣接4方向
	//=================================================
	//
	// 現段階では順番に検索して、
	// 最初に見つかったEnemyを攻撃する。
	//
	// 将来的にはMouse選択によって
	// GameObjectを直接指定する方式へ変更予定。
	//

	const GridPosition directions[] =
	{
		{  0, -1 }, // Up
		{  0,  1 }, // Down
		{ -1,  0 }, // Left
		{  1,  0 }  // Right
	};

	GameObject* attackTarget =
		nullptr;

	//=================================================
	// 隣接Enemy検索
	//=================================================

	for (const GridPosition& direction :
		directions)
	{
		const GridPosition targetPosition
		{
			playerPosition.X +
				direction.X,

			playerPosition.Y +
				direction.Y
		};

		GameObject* occupant =
			gridMap->GetOccupant(
				targetPosition);

		if (!occupant)
		{
			continue;
		}

		// Enemy以外は攻撃対象にしない。
		if (occupant->GetTag() !=
			Tag::Enemy)
		{
			continue;
		}

		attackTarget =
			occupant;

		break;
	}

	//=================================================
	// Enemyが隣接していない
	//=================================================

	if (!attackTarget)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: No adjacent enemy.\n");

		return false;
	}

	//=================================================
	// Enemy Status取得
	//=================================================

	auto* enemyStatus =
		attackTarget->GetComponent<
		CharacterStatusComponent>();

	if (!enemyStatus)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Enemy CharacterStatusComponent not found.\n");

		return false;
	}

	if (enemyStatus->IsDead())
	{
		return false;
	}

	//=================================================
	// Damage
	//=================================================

	const int damage =
		playerStatus->
		GetAttackPower();

	enemyStatus->
		TakeDamage(
			damage);

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[PlayerController] Attack success. Damage=%d\n",
		damage);

	OutputDebugStringA(
		buffer);

	//=================================================
	// 攻撃成功時はPlayerTurn終了
	//=================================================

	if (m_TurnManager)
	{
		m_RemainingMovePoints =
			0;

		m_TurnManager->
			EndPlayerTurn();
	}

	return true;
}

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
	// PlayerTurn確認
	//=================================================

	if (!m_TurnManager ||
		!m_TurnManager->IsPlayerTurn())
	{
		return false;
	}

	//=================================================
	// Enemy確認
	//=================================================

	if (target->GetTag() !=
		Tag::Enemy)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: Target is not Enemy.\n");

		return false;
	}

	//=================================================
	// Component取得
	//=================================================

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerStatus =
		owner->GetComponent<
		CharacterStatusComponent>();

	auto* enemyGrid =
		target->GetComponent<
		GridPositionComponent>();

	auto* enemyStatus =
		target->GetComponent<
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

	//=================================================
	// 距離判定
	//=================================================

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

	// 隣接4方向のみ攻撃可能。
	if (distance != 1)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: Target is not adjacent.\n");

		return false;
	}

	//=================================================
	// 攻撃対象の方向を向く
	//=================================================
	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (transform)
	{
		transform->FaceGridPosition(
			playerPosition,
			enemyPosition);
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
	}

	//=================================================
	// Damage
	//=================================================

	const int damage =
		playerStatus->GetAttackPower();

	enemyStatus->TakeDamage(
		damage);

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[PlayerController] "
		"Attack success. Damage=%d\n",
		damage);

	OutputDebugStringA(
		buffer);

	//=================================================
	// 攻撃中状態へ移行
	//=================================================
	//
	// EnemyTurnへの移行はここでは行わない。
	// Attack Animation終了後にUpdate()側で行う。
	//

	m_RemainingMovePoints =
		0;

	m_ActionState =
		PlayerActionState::Attacking;

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
		"W / A / S / D : Move");

	ImGui::Text(
		"Space : Attack");

	ImGui::Text(
		"Enter : End Turn");

	ImGui::Separator();

	ImGui::Text(
		"Remaining Move Points: %d",
		m_RemainingMovePoints);

	if (m_TurnManager)
	{
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

		return;
	}

	GridMap* gridMap =
		gridPosition->GetGridMap();

	if (!gridMap)
	{
		m_ActionState =
			PlayerActionState::Idle;

		return;
	}

	//=================================================
	// Path終了確認
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
					CharacterAnimationState::Idle);
		}

		return;
	}

	const GridPosition currentGrid =
		gridPosition->GetGridPosition();

	const GridPosition nextGrid =
		m_MovePath[
			m_MovePathIndex];

	//=================================================
	// 移動方向を向く
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
		transform->GetPosition();

	Vector3 direction =
		targetPosition -
		currentPosition;

	const float distance =
		direction.Length();

	//=================================================
	// 到着判定
	//=================================================

	constexpr float ARRIVE_EPSILON =
		0.01f;

	if (distance <=
		ARRIVE_EPSILON)
	{
		// 座標誤差を消すため完全に合わせる。
		transform->SetPosition(
			targetPosition);

		//=============================================
		// Grid論理位置更新
		//=============================================

		if (!gridPosition->
			SetGridPosition(
				nextGrid))
		{
			// 何らかの理由でGrid更新失敗。
			m_ActionState =
				PlayerActionState::Idle;

			return;
		}

		// 1マス分の移動力消費。
		--m_RemainingMovePoints;

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
						CharacterAnimationState::Idle);
			}

			// 移動力を使い切った場合のみ
			// EnemyTurnへ進む。
			if (m_RemainingMovePoints <= 0 &&
				m_TurnManager)
			{
				m_TurnManager->
					EndPlayerTurn();
			}

			return;
		}

		return;
	}

	//=================================================
	// World座標補間
	//=================================================

	direction.Normalize();

	// deltaは現在uint64_tなので、
	// プロジェクト内で「何単位か」に合わせてください。
	//
	// もしdeltaがミリ秒なら /1000.0f。
	// 既に秒換算済みならそのまま。
	const float deltaSeconds =
		static_cast<float>(
			delta) /
		1000.0f;

	const float moveDistance =
		m_MoveSpeed *
		deltaSeconds;

	// 次フレームでTargetを通り越す場合は
	// TargetにClampする。
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

bool PlayerControllerComponent::BeginMove(
	const std::vector<GridPosition>& path)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	// StartだけしかないPathは移動不要。
	if (path.size() <= 1)
	{
		return false;
	}

	m_MovePath =
		path;

	// index 0は現在地なので、
	// index 1を最初の目的地とする。
	m_MovePathIndex =
		1;

	m_ActionState =
		PlayerActionState::Moving;

	//=================================================
	// 移動Animation開始
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
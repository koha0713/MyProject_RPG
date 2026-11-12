#include "PlayerControllerComponent.h"

#include <cstdlib>

#include "GameObject.h"

#include "GridMap.h"
#include "GridPositionComponent.h"

#include "CharacterStatusComponent.h"
#include "CharacterAnimationComponent.h"
#include "TransformComponent.h"

#include "TargetingSystem.h"
#include "EffectSystem.h"
#include "EffectDefinition.h"
#include "EffectType.h"

#include "PlayerActionComponent.h"
#include "PlayerActionType.h"
#include "PlayerClassComponent.h"

#include "GridPathFinder.h"

#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void PlayerControllerComponent::Initialize()
{
	m_ActionState =
		PlayerActionState::Idle;

	m_MovePath.clear();

	m_MovePathIndex =
		0;

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
	// TurnManager
	//=================================================

	if (!m_TurnManager)
	{
		return;
	}

	// Player Phase以外ではPlayer Actionを進めない。
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
	// Skill Animation
	//=================================================
	if (m_IsSkillExecuting)
	{
		UpdateSkillAnimation();

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
	// Attacking
	//=================================================

	if (m_ActionState ==
		PlayerActionState::Attacking)
	{
		UpdateClassAttack();

		return;
	}

	//=================================================
	// Idle
	//=================================================
	//
	// APが0でもController自身は
	// Turnを終了させない。
	//
	// Player Phase終了判断はFieldSceneが行う。
	//=================================================
}

//=====================================================
// Draw
//=====================================================

void PlayerControllerComponent::Draw()
{
}

//=====================================================
// Component Access
//=====================================================

CharacterStatusComponent*
PlayerControllerComponent::GetStatus() const
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return nullptr;
	}

	return owner->GetComponent<
		CharacterStatusComponent>();
}

PlayerActionComponent*
PlayerControllerComponent::GetPlayerAction() const
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return nullptr;
	}

	return owner->GetComponent<
		PlayerActionComponent>();
}

//=====================================================
// Can Accept Action
//=====================================================

bool PlayerControllerComponent::
CanAcceptActionRequest() const
{
	if (!m_TurnManager)
	{
		return false;
	}

	if (!m_TurnManager->
		IsPlayerTurn())
	{
		return false;
	}

	if (m_ActionState !=
		PlayerActionState::Idle)
	{
		return false;
	}

	auto* status =
		GetStatus();

	if (!status)
	{
		return false;
	}

	if (status->IsDead())
	{
		return false;
	}

	return true;
}

//=====================================================
// Unit Action
//=====================================================

bool PlayerControllerComponent::RequestUnitAction(
	PlayerActionType type,
	GameObject* target)
{
	if (!target)
	{
		return false;
	}

	switch (type)
	{
	case PlayerActionType::NormalAttack:
	{
		// 既存の通常攻撃処理を再利用する。
		return RequestAttack(
			target);
	}

	case PlayerActionType::Skill1:
	case PlayerActionType::Skill2:
	case PlayerActionType::Ultimate:
	{
		return ExecuteUnitSkill(
			type,
			target);
	}

	default:
		break;
	}

	return false;
}

//=====================================================
// Grid Action
//=====================================================

bool PlayerControllerComponent::RequestGridAction(
	PlayerActionType type,
	const GridPosition& target)
{
	switch (type)
	{
	case PlayerActionType::Move:
	{
		// 既存Move処理を利用。
		return RequestMoveTo(
			target);
	}

	case PlayerActionType::Skill1:
	case PlayerActionType::Skill2:
	case PlayerActionType::Ultimate:
	{
		// Fireball / IceWall /
		// ShadowStep等を後で実装する。
		OutputDebugStringA(
			"[PlayerController] "
			"Grid Skill is not implemented yet.\n");

		return false;
	}

	default:
		break;
	}

	return false;
}

//=====================================================
// Self Action
//=====================================================
bool PlayerControllerComponent::
RequestSelfAction(
	PlayerActionType type)
{
	switch (type)
	{
	case PlayerActionType::Skill1:
	case PlayerActionType::Skill2:
	case PlayerActionType::Ultimate:
	{
		return ExecuteSelfSkill(
			type);
	}

	default:
		break;
	}

	return false;
}

//=====================================================
// Direction Action
//=====================================================
bool PlayerControllerComponent::
RequestDirectionAction(
	PlayerActionType type,
	GridDirection direction)
{
	switch (type)
	{
	case PlayerActionType::Skill1:
	case PlayerActionType::Skill2:
	case PlayerActionType::Ultimate:
	{
		return ExecuteDirectionSkill(
			type,
			direction);
	}

	default:
		break;
	}

	return false;
}

//=====================================================
// Execute Unit Skill
//=====================================================

bool PlayerControllerComponent::
ExecuteUnitSkill(
	PlayerActionType type,
	GameObject* target)
{
	if (!target ||
		target->IsDestroy())
	{
		return false;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	auto* playerAction =
		GetPlayerAction();

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* targetGrid =
		target->GetComponent<
		GridPositionComponent>();

	if (!playerAction ||
		!playerGrid ||
		!targetGrid)
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

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			type);

	if (!definition.Enabled)
	{
		return false;
	}

	//=================================================
	// Target Validation
	//=================================================

	const GridPosition origin =
		playerGrid->
		GetGridPosition();

	const GridPosition targetPosition =
		targetGrid->
		GetGridPosition();

	if (!TargetingSystem::
		IsTargetableCell(
			*gridMap,
			owner,
			origin,
			targetPosition,
			definition.TargetPattern,
			definition.Targeting))
	{
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
				origin,
				targetPosition);
	}

	//=================================================
	// Effect
	//=================================================

	if (!ApplyActionEffectsToTarget(
		definition,
		target))
	{
		return false;
	}

	//=================================================
	// Cost
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			type))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Skill cost consume failed.\n");

		return false;
	}

	//=================================================
	// Animation
	//=================================================

	BeginSkillAnimation(
		definition,
		type);

	OutputDebugStringA(
		"[PlayerController] "
		"Unit Skill executed.\n");

	return true;
}

//=====================================================
// Execute Direction Skill
//=====================================================
bool PlayerControllerComponent::
ExecuteDirectionSkill(
	PlayerActionType type,
	GridDirection direction)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	auto* playerAction =
		GetPlayerAction();

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	if (!playerAction ||
		!playerGrid)
	{
		return false;
	}

	//=================================================
	// Action使用可能確認
	//=================================================

	if (!playerAction->
		CanUseAction(
			type))
	{
		return false;
	}

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			type);

	if (!definition.Enabled ||
		!definition.HasEffectPattern ||
		!definition.EffectPattern.IsValid())
	{
		return false;
	}

	const GridPosition origin =
		playerGrid->
		GetGridPosition();

	//=================================================
	// Facing Target Position
	//=================================================

	GridPosition facingTarget =
		origin;

	switch (direction)
	{
	case GridDirection::North:

		facingTarget.Y -= 1;
		break;

	case GridDirection::East:

		facingTarget.X += 1;
		break;

	case GridDirection::South:

		facingTarget.Y += 1;
		break;

	case GridDirection::West:

		facingTarget.X -= 1;
		break;

	default:

		return false;
	}

	//=================================================
	// Face Direction
	//=================================================

	auto* transform =
		owner->GetComponent<
		TransformComponent>();

	if (transform)
	{
		transform->
			FaceGridPosition(
				origin,
				facingTarget);
	}

	//=================================================
	// Effect
	//=================================================
	//
	// 対象が存在しなくてもSkill自体は成立させる。
	//
	// これによりDirection Skillは
	// 「方向を選択して空振りする」ことも可能。
	//=================================================

	ApplyActionEffectsToPattern(
		definition,
		origin,
		direction);

	//=================================================
	// Cost
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			type))
	{
		return false;
	}

	BeginSkillAnimation(
		definition,
		type);

	OutputDebugStringA(
		"[PlayerController] "
		"Direction Skill executed.\n");

	return true;
}

//=====================================================
// Execute Self Skill
//=====================================================

bool PlayerControllerComponent::
ExecuteSelfSkill(
	PlayerActionType type)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	auto* playerAction =
		GetPlayerAction();

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	if (!playerAction ||
		!playerGrid)
	{
		return false;
	}

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			type);

	if (!definition.Enabled)
	{
		return false;
	}

	const GridPosition origin =
		playerGrid->
		GetGridPosition();

	//=================================================
	// Pattern Effect
	//=================================================
	//
	// Self Skillは方向回転不要なので
	// Northを基準方向として渡す。
	//
	// Square等のRotateWithFacing=false Patternなら
	// 結果は方向に依存しない。
	//=================================================

	const bool applied =
		ApplyActionEffectsToPattern(
			definition,
			origin,
			GridDirection::North);

	if (!applied)
	{
		// 周囲にEnemyがいなければ
		// Ultimateを消費しない。
		return false;
	}

	//=================================================
	// Cost
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			type))
	{
		return false;
	}

	//=================================================
	// Animation
	//=================================================

	BeginSkillAnimation(
		definition,
		type);

	OutputDebugStringA(
		"[PlayerController] "
		"Self Skill executed.\n");

	return true;
}

//=====================================================
// Build Runtime Effect
//=====================================================
EffectDefinition
PlayerControllerComponent::BuildRuntimeEffect(
	const EffectDefinition& definition) const
{
	EffectDefinition runtimeEffect =
		definition;

	//=================================================
	// Source AttackPower
	//=================================================

	if (!definition.UseSourceAttackPower)
	{
		return runtimeEffect;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		runtimeEffect.Power =
			0;

		return runtimeEffect;
	}

	auto* status =
		owner->GetComponent<
		CharacterStatusComponent>();

	if (!status)
	{
		runtimeEffect.Power =
			0;

		return runtimeEffect;
	}

	const int attackPower =
		status->
		GetAttackPower();

	//=================================================
	// Damage Calculation
	//=================================================
	//
	// 現段階では
	// AttackPower × Skill倍率。
	//
	// 将来はBattleCalculatorへ移行する。
	//=================================================

	const float calculatedPower =
		static_cast<float>(
			attackPower) *
		definition.PowerMultiplier;

	runtimeEffect.Power =
		static_cast<int>(
			calculatedPower);

	// 最低1Damageを保証。
	if (runtimeEffect.Power <= 0)
	{
		runtimeEffect.Power =
			1;
	}

	return runtimeEffect;
}

//=====================================================
// Apply Action Effects To Target
//=====================================================
bool PlayerControllerComponent::
ApplyActionEffectsToTarget(
	const PlayerActionDefinition& definition,
	GameObject* target)
{
	if (!target ||
		target->IsDestroy())
	{
		return false;
	}

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
		gridPosition->
		GetGridMap();

	if (!gridMap)
	{
		return false;
	}

	bool appliedAny =
		false;

	//=================================================
	// Effect順に適用
	//=================================================
	//
	// Shield Bashなら
	//
	// Damage
	// ↓
	// Knockback
	//
	// の順番になる。
	//=================================================

	for (const EffectDefinition& effectDefinition :
		definition.Effects)
	{
		const EffectDefinition runtimeEffect =
			BuildRuntimeEffect(
				effectDefinition);

		if (EffectSystem::ApplyEffect(
			*gridMap,
			owner,
			target,
			runtimeEffect))
		{
			appliedAny =
				true;
		}
	}

	return appliedAny;
}

//=====================================================
// Apply Action Effects To Pattern
//=====================================================
bool PlayerControllerComponent::
ApplyActionEffectsToPattern(
	const PlayerActionDefinition& definition,
	const GridPosition& origin,
	GridDirection direction)
{
	if (!definition.HasEffectPattern ||
		!definition.EffectPattern.IsValid())
	{
		return false;
	}

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
		gridPosition->
		GetGridMap();

	if (!gridMap)
	{
		return false;
	}

	//=================================================
	// Pattern展開
	//=================================================

	const auto offsets =
		definition.EffectPattern.
		GetOffsets(
			direction);

	bool appliedAny =
		false;

	for (const GridOffset& offset :
		offsets)
	{
		const GridPosition targetPosition
		{
			origin.X +
				offset.X,

			origin.Y +
				offset.Y
		};

		if (!gridMap->
			IsInside(
				targetPosition))
		{
			continue;
		}

		GameObject* target =
			gridMap->
			GetOccupant(
				targetPosition);

		// 空マスはEffect適用対象ではない。
		if (!target ||
			target->IsDestroy())
		{
			continue;
		}

		//=============================================
		// 全Effect適用
		//=============================================

		for (const EffectDefinition& effectDefinition :
			definition.Effects)
		{
			const EffectDefinition runtimeEffect =
				BuildRuntimeEffect(
					effectDefinition);

			if (EffectSystem::
				ApplyEffect(
					*gridMap,
					owner,
					target,
					runtimeEffect))
			{
				appliedAny =
					true;
			}
		}
	}

	return appliedAny;
}

//=====================================================
// Begin Skill Animation
//=====================================================

void PlayerControllerComponent::
BeginSkillAnimation(
	const PlayerActionDefinition& definition,
	PlayerActionType type)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	//=================================================
	// Animation未設定
	//=================================================
	//
	// AnimationID::Noneなら
	// Skill Animation待ちは行わない。
	//=================================================

	if (definition.Animation ==
		AnimationID::None)
	{
		m_IsSkillExecuting =
			false;

		return;
	}

	auto* characterAnimation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (!characterAnimation)
	{
		m_IsSkillExecuting =
			false;

		return;
	}

	//=================================================
	// Custom Animation再生
	//=================================================

	characterAnimation->
		PlayCustomAnimation(
			definition.Animation,
			definition.AnimationLoop,
			definition.AnimationBlendTime);

	m_IsSkillExecuting =
		true;

	m_ExecutingSkillType =
		type;

	OutputDebugStringA(
		"[PlayerController] "
		"Skill Animation Started.\n");
}

//=====================================================
// Update Skill Animation
//=====================================================

void PlayerControllerComponent::
UpdateSkillAnimation()
{
	if (!m_IsSkillExecuting)
	{
		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		m_IsSkillExecuting =
			false;

		return;
	}

	auto* characterAnimation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (!characterAnimation)
	{
		m_IsSkillExecuting =
			false;

		return;
	}

	//=================================================
	// Animation終了待ち
	//=================================================

	if (!characterAnimation->
		IsCurrentAnimationFinished())
	{
		return;
	}

	//=================================================
	// Idle復帰
	//=================================================

	characterAnimation->
		ReturnToIdle();

	m_IsSkillExecuting =
		false;

	OutputDebugStringA(
		"[PlayerController] "
		"Skill Animation Finished.\n");
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
	// Action State Reset
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
	// AP Reset
	//=================================================

	auto* playerAction =
		GetPlayerAction();

	if (playerAction)
	{
		playerAction->
			BeginTurn();
	}
	else
	{
		OutputDebugStringA(
			"[PlayerController] "
			"PlayerActionComponent not found.\n");
	}

	//=================================================
	// Debug
	//=================================================

	auto* status =
		GetStatus();

	if (status)
	{
		char buffer[128]{};

		sprintf_s(
			buffer,
			"[PlayerTurn] Begin AP=%d/%d\n",
			status->GetAP(),
			status->GetMaxAP());

		OutputDebugStringA(
			buffer);
	}
}

//=====================================================
// Start Player Turn
//=====================================================

void PlayerControllerComponent::StartPlayerTurn(
	uint64_t turnCount)
{
	// FieldSceneから明示的に呼ばれた場合、
	// Update()内で同じTurnを二重初期化しないよう記録。
	m_LastTurnCount =
		turnCount;

	BeginPlayerTurn();
}

//=====================================================
// Request Move
//=====================================================

bool PlayerControllerComponent::RequestMove(
	const GridPosition& direction)
{
	if (!CanAcceptActionRequest())
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
	// Action
	//=================================================

	auto* playerAction =
		GetPlayerAction();

	if (!playerAction)
	{
		return false;
	}

	if (!playerAction->
		CanUseAction(
			PlayerActionType::Move))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move failed: "
			"Not enough AP/MP.\n");

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

	// 移動できなかった場合は
	// APを消費しない。
	if (!moved)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move blocked.\n");

		return false;
	}

	//=================================================
	// Cost
	//=================================================
	//
	// 実際に移動成功した後にCostを支払う。
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			PlayerActionType::Move))
	{
		// CanUseAction確認済みなので、
		// 通常ここには到達しない。
		OutputDebugStringA(
			"[PlayerController] "
			"Move Cost consumption failed.\n");

		return false;
	}

	return true;
}

//=====================================================
// Request Move To
//=====================================================

bool PlayerControllerComponent::RequestMoveTo(
	const GridPosition& destination)
{
	if (!CanAcceptActionRequest())
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
	// Components
	//=================================================

	auto* gridPosition =
		owner->GetComponent<
		GridPositionComponent>();

	auto* status =
		GetStatus();

	auto* playerAction =
		GetPlayerAction();

	if (!gridPosition ||
		!status ||
		!playerAction)
	{
		return false;
	}

	//=================================================
	// Action Cost確認
	//=================================================

	if (!playerAction->
		CanUseAction(
			PlayerActionType::Move))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move failed: "
			"Not enough AP/MP.\n");

		return false;
	}

	//=================================================
	// GridMap
	//=================================================

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

	const int moveDistance =
		static_cast<int>(
			path.size()) -
		1;

	if (moveDistance <= 0)
	{
		return false;
	}

	//=================================================
	// Move Range
	//=================================================
	//
	// AGIを直接移動マス数には使用しない。
	//
	// CalculateMoveRange()が、
	// 現在のCharacterの最終移動距離を返す。
	//=================================================

	const int maxMoveRange =
		status->
		CalculateMoveRange();

	if (moveDistance >
		maxMoveRange)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Move failed: "
			"Destination out of move range.\n");

		return false;
	}

	//=================================================
	// Move開始
	//=================================================

	if (!BeginMove(
		path))
	{
		return false;
	}

	//=================================================
	// Cost Consume
	//=================================================
	//
	// Path・距離・移動開始の全確認後に消費する。
	//
	// 1マスでも最大Rangeまで移動しても、
	// 「1回のMove Action」として同じCost。
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			PlayerActionType::Move))
	{
		// 通常発生しないが、
		// Cost消費に失敗した場合は移動を取り消す。
		m_MovePath.clear();

		m_MovePathIndex =
			0;

		m_ActionState =
			PlayerActionState::Idle;

		auto* animation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (animation)
		{
			animation->
				ChangeState(
					CharacterAnimationState::Idle);
		}

		return false;
	}

	return true;
}

//=====================================================
// Attack - Auto Target
//=====================================================

bool PlayerControllerComponent::RequestAttack()
{
	if (!CanAcceptActionRequest())
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
	// Components
	//=================================================

	auto* playerGrid =
		owner->GetComponent<
		GridPositionComponent>();

	auto* playerAction =
		GetPlayerAction();

	if (!playerGrid ||
		!playerAction)
	{
		return false;
	}

	//=================================================
	// Cost
	//=================================================

	if (!playerAction->
		CanUseAction(
			PlayerActionType::NormalAttack))
	{
		return false;
	}

	//=================================================
	// GridMap
	//=================================================

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

	//=================================================
	// Action Definition
	//=================================================

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			PlayerActionType::NormalAttack);

	if (!definition.Enabled ||
		!definition.TargetPattern.IsValid())
	{
		return false;
	}

	//=================================================
	// Targetable Cells
	//=================================================
	//
	// Pattern / LOS / Wall / Unit遮蔽を
	// TargetingSystemへ一元化する。
	//=================================================

	const auto targetableCells =
		TargetingSystem::
		GetTargetableCells(
			*gridMap,
			owner,
			playerPosition,
			definition.TargetPattern,
			definition.Targeting);

	//=================================================
	// Enemy検索
	//=================================================
	//
	// 現段階ではTargetPattern内の順序で
	// 最初に見つかったEnemyを自動Targetにする。
	//
	// 将来的にAutoTargetが必要なら
	// 距離・HP・脅威度などでScore化可能。
	//=================================================

	for (const GridPosition& targetPosition :
		targetableCells)
	{
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

		return RequestAttack(
			occupant);
	}

	OutputDebugStringA(
		"[PlayerController] "
		"Attack failed: "
		"No targetable enemy.\n");

	return false;
}

//=====================================================
// Attack - Target
//=====================================================

bool PlayerControllerComponent::RequestAttack(
	GameObject* target)
{
	if (!CanAcceptActionRequest())
	{
		return false;
	}

	GameObject* owner =
		GetOwner();

	if (!owner ||
		!target)
	{
		return false;
	}

	//=================================================
	// Enemy
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

	auto* playerAction =
		GetPlayerAction();

	if (!playerGrid ||
		!enemyGrid ||
		!enemyStatus ||
		!playerClass ||
		!playerAction)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Required Component not found.\n");

		return false;
	}

	if (enemyStatus->
		IsDead())
	{
		return false;
	}

	//=================================================
	// Cost確認
	//=================================================

	if (!playerAction->
		CanUseAction(
			PlayerActionType::NormalAttack))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Not enough AP/MP.\n");

		return false;
	}

	//=================================================
	// Grid Position
	//=================================================

	const GridPosition playerPosition =
		playerGrid->
		GetGridPosition();

	const GridPosition enemyPosition =
		enemyGrid->
		GetGridPosition();

	//=================================================
	// GridMap
	//=================================================

	GridMap* gridMap =
		playerGrid->
		GetGridMap();

	if (!gridMap)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"GridMap not found.\n");

		return false;
	}

	//=================================================
	// Action Definition
	//=================================================

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			PlayerActionType::NormalAttack);

	if (!definition.Enabled ||
		!definition.TargetPattern.IsValid())
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Invalid Action Definition.\n");

		return false;
	}

	//=================================================
	// Targeting
	//=================================================
	//
	// Range / Pattern / LOS / Wall / Unit遮蔽を
	// TargetingSystemへ一元化する。
	//=================================================

	const bool targetable =
		TargetingSystem::
		IsTargetableCell(
			*gridMap,
			owner,
			playerPosition,
			enemyPosition,
			definition.TargetPattern,
			definition.Targeting);

	if (!targetable)
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack failed: "
			"Target is not targetable.\n");

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
	// Begin Class Attack
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
	// Cost Consume
	//=================================================
	//
	// Target / Range / Attack開始が全て成功してから
	// AP / MPを消費する。
	//=================================================

	if (!playerAction->
		ConsumeActionCost(
			PlayerActionType::NormalAttack))
	{
		OutputDebugStringA(
			"[PlayerController] "
			"Attack Cost consumption failed.\n");

		// BeginNormalAttack後なので本来は
		// CancelAttack APIが欲しい。
		//
		// ただし同一FrameでCanUseAction確認済みのため、
		// 現構造では通常ここには到達しない。
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

			auto* attackerGrid =
				owner->GetComponent<
				GridPositionComponent>();

			if (attackerStatus &&
				attackerGrid)
			{
				GridMap* gridMap =
					attackerGrid->
					GetGridMap();

				if (gridMap)
				{
					//=============================
					// Temporary Damage Effect
					//=============================

					EffectDefinition damageEffect;

					damageEffect.Type =
						EffectType::Damage;

					damageEffect.Power =
						attackerStatus->
						GetAttackPower();

					damageEffect.AffectSelf =
						false;

					damageEffect.AffectAllies =
						false;

					damageEffect.AffectEnemies =
						true;

					EffectSystem::
						ApplyEffect(
							*gridMap,
							owner,
							m_AttackTarget,
							damageEffect);
				}
			}
		}
	}

	//=================================================
	// Attack終了待ち
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
// Request Action
//=====================================================

bool PlayerControllerComponent::RequestAction(
	const PlayerActionRequest& request)
{
	if (!CanAcceptActionRequest())
	{
		return false;
	}

	PlayerActionComponent* playerAction =
		GetPlayerAction();

	if (!playerAction)
	{
		return false;
	}

	//=================================================
	// Action Definition
	//=================================================

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			request.Type);

	if (!definition.Enabled)
	{
		return false;
	}

	//=================================================
	// Cost確認
	//=================================================
	//
	// この段階では消費しない。
	//
	// Target検証・Action開始に成功してから
	// 各Action側でCostを消費する。
	//=================================================

	if (!playerAction->
		CanUseAction(
			request.Type))
	{
		return false;
	}

	//=================================================
	// Target Mode
	//=================================================

	switch (definition.TargetMode)
	{
	case TargetingMode::Unit:
	{
		return RequestUnitAction(
			request.Type,
			request.TargetObject);
	}

	case TargetingMode::Grid:
	{
		return RequestGridAction(
			request.Type,
			request.TargetGrid);
	}

	case TargetingMode::Self:
	{
		return RequestSelfAction(
			request.Type);
	}

	case TargetingMode::Direction:
	{
		return RequestDirectionAction(
			request.Type,
			request.Direction);
	}

	case TargetingMode::None:
	default:
		break;
	}

	return false;
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
		// 座標誤差を除去する。
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
			// 旧MovePoint消費は削除
			//=========================================
			//
			// Move Action開始時に既に
			// APを消費済み。
			//=========================================

			return;
		}

		return;
	}

	//=================================================
	// World Position補間
	//=================================================

	direction.Normalize();

	// 現在の既存仕様を維持して、
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

	// 開始地点しかないPathは移動不要。
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
		"Action Finished: %s",
		IsActionFinished()
		? "Yes"
		: "No");

	ImGui::Text(
		"Class Attack Active: %s",
		m_ClassAttackActive
		? "Yes"
		: "No");

	//=================================================
	// Character Status
	//=================================================

	auto* status =
		GetStatus();

	if (status)
	{
		ImGui::Separator();

		ImGui::Text(
			"AP: %d / %d",
			status->GetAP(),
			status->GetMaxAP());

		ImGui::Text(
			"MP: %d / %d",
			status->GetMP(),
			status->GetMaxMP());

		ImGui::Text(
			"Move Range: %d",
			status->
			CalculateMoveRange());
	}

	//=================================================
	// Player Action
	//=================================================

	auto* playerAction =
		GetPlayerAction();

	if (playerAction)
	{
		ImGui::Separator();

		ImGui::Text(
			"Can Move: %s",
			playerAction->
			CanUseAction(
				PlayerActionType::Move)
			? "Yes"
			: "No");

		ImGui::Text(
			"Can Normal Attack: %s",
			playerAction->
			CanUseAction(
				PlayerActionType::
				NormalAttack)
			? "Yes"
			: "No");
	}

	//=================================================
	// Attack Target
	//=================================================

	ImGui::Separator();

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
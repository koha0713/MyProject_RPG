#include "PlayerClassComponent.h"

#include "GameObject.h"

#include "CharacterAnimationComponent.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void PlayerClassComponent::Initialize()
{
	m_AttackTarget =
		nullptr;

	m_AttackSequence.clear();

	m_CurrentAnimationIndex =
		0;

	m_IsAttacking =
		false;

	m_AttackHitRequested =
		false;

	m_HitRequestedThisAttack =
		false;
}

void PlayerClassComponent::Finalize()
{
	m_AttackTarget =
		nullptr;

	m_AttackSequence.clear();

	m_IsAttacking =
		false;

	m_AttackHitRequested =
		false;
}

//=====================================================
// Update
//=====================================================

void PlayerClassComponent::Update(
	uint64_t delta)
{
	if (!m_IsAttacking)
	{
		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		FinishAttack();

		return;
	}

	auto* animation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (!animation)
	{
		FinishAttack();

		return;
	}

	//=================================================
	// 職業固有更新
	//=================================================

	UpdateClassAttack(
		delta);

	//=================================================
	// Animation終了待ち
	//=================================================

	if (!animation->
		IsCurrentAnimationFinished())
	{
		return;
	}

	//=================================================
	// Hit Request
	//=================================================

	const size_t hitIndex =
		GetHitAnimationIndex(
			m_AttackSequence.size());

	if (!m_HitRequestedThisAttack &&
		m_CurrentAnimationIndex ==
		hitIndex)
	{
		m_AttackHitRequested =
			true;

		m_HitRequestedThisAttack =
			true;
	}

	//=================================================
	// 次Animation
	//=================================================

	++m_CurrentAnimationIndex;

	if (m_CurrentAnimationIndex <
		m_AttackSequence.size())
	{
		StartCurrentAnimation();

		return;
	}

	//=================================================
	// 完了
	//=================================================

	FinishAttack();
}

//=====================================================
// Begin Attack
//=====================================================

bool PlayerClassComponent::
BeginNormalAttack(
	GameObject* target)
{
	if (!target ||
		m_IsAttacking)
	{
		return false;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	auto* animation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (!animation)
	{
		return false;
	}

	m_AttackSequence =
		BuildNormalAttackSequence();

	if (m_AttackSequence.empty())
	{
		return false;
	}

	m_AttackTarget =
		target;

	m_CurrentAnimationIndex =
		0;

	m_IsAttacking =
		true;

	m_AttackHitRequested =
		false;

	m_HitRequestedThisAttack =
		false;

	StartCurrentAnimation();

	return true;
}

//=====================================================
// Start Current Animation
//=====================================================

void PlayerClassComponent::
StartCurrentAnimation()
{
	if (m_CurrentAnimationIndex >=
		m_AttackSequence.size())
	{
		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	auto* animation =
		owner->GetComponent<
		CharacterAnimationComponent>();

	if (!animation)
	{
		return;
	}

	animation->
		PlayCustomAnimation(
			m_AttackSequence[
				m_CurrentAnimationIndex],
				false,
				0.08f);
}

//=====================================================
// Finish Attack
//=====================================================

void PlayerClassComponent::FinishAttack()
{
	GameObject* owner =
		GetOwner();

	if (owner)
	{
		auto* animation =
			owner->GetComponent<
			CharacterAnimationComponent>();

		if (animation)
		{
			animation->
				ReturnToIdle();
		}
	}

	m_IsAttacking =
		false;

	m_AttackTarget =
		nullptr;

	m_AttackSequence.clear();

	m_CurrentAnimationIndex =
		0;
}

//=====================================================
// Hit Request
//=====================================================

bool PlayerClassComponent::
ConsumeAttackHitRequest()
{
	if (!m_AttackHitRequested)
	{
		return false;
	}

	m_AttackHitRequested =
		false;

	return true;
}

//=====================================================
// Hit Animation Index
//=====================================================

size_t PlayerClassComponent::
GetHitAnimationIndex(
	size_t sequenceSize) const
{
	if (sequenceSize == 0)
	{
		return 0;
	}

	// デフォルトでは最後のAnimation終了時にHit。
	return sequenceSize - 1;
}

//=====================================================
// Class Attack Update
//=====================================================

void PlayerClassComponent::
UpdateClassAttack(
	uint64_t delta)
{
	(void)delta;
}

//=====================================================
// Action Definition
//=====================================================
PlayerActionDefinition
PlayerClassComponent::GetActionDefinition(
	PlayerActionType type) const
{
	PlayerActionDefinition definition;

	definition.Type =
		type;

	definition.Enabled =
		true;

	switch (type)
	{
	case PlayerActionType::Move:
	{
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			0;

		//=================================================
		// MoveはTargetingSystemとは別扱い
		//=================================================
		//
		// 移動範囲はCharacterStatusComponentの
		// CalculateMoveRange()を使用するため、
		// TargetPatternはここでは設定しない。
		//=================================================

		break;
	}

	case PlayerActionType::NormalAttack:
	{
		// 派生Class側で設定する。
		definition.Enabled =
			false;

		break;
	}

	case PlayerActionType::Skill1:
	case PlayerActionType::Skill2:
	case PlayerActionType::Ultimate:
	{
		// 派生Class側で設定する。
		definition.Enabled =
			false;

		break;
	}

	case PlayerActionType::Item:
	{
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			0;

		// ItemのTargetingは
		// Inventory実装時に設定する。
		break;
	}

	default:
	{
		definition.Enabled =
			false;

		break;
	}
	}

	return definition;
}

//=====================================================
// Debug
//=====================================================

void PlayerClassComponent::DrawDebugUI()
{
	ImGui::Text(
		"Class: %s",
		GetClassName());

	ImGui::Text(
		"Attacking: %s",
		m_IsAttacking
		? "Yes"
		: "No");

	ImGui::Text(
		"Animation Index: %zu",
		m_CurrentAnimationIndex);

	ImGui::Text(
		"Sequence Size: %zu",
		m_AttackSequence.size());
}
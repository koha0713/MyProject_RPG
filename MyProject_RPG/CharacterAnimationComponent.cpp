#include "CharacterAnimationComponent.h"

#include "GameObject.h"
#include "AnimatorComponent.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void CharacterAnimationComponent::Initialize()
{
	// 初期状態はIdle。
	m_State =
		CharacterAnimationState::Idle;

	PlayStateAnimation();
}

void CharacterAnimationComponent::Finalize()
{
}

void CharacterAnimationComponent::Update(
	uint64_t delta)
{
	(void)delta;

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	auto* animator =
		owner->GetComponent<
		AnimatorComponent>();

	if (!animator)
	{
		return;
	}

	//=================================================
	// Attack終了判定
	//=================================================
	//
	// Attackは非Loop再生。
	//
	// 再生終了後は自動的にIdleへ戻す。
	//

	if (m_State ==
		CharacterAnimationState::Attack)
	{
		if (animator->
			IsAnimationFinished())
		{
			ChangeState(
				CharacterAnimationState::Idle);
		}

		return;
	}

	//=================================================
	// Dead
	//=================================================
	//
	// Death Animation終了後もIdleには戻さない。
	//

	if (m_State ==
		CharacterAnimationState::Dead)
	{
		return;
	}
}

void CharacterAnimationComponent::Draw()
{
}

//=====================================================
// State
//=====================================================

void CharacterAnimationComponent::ChangeState(
	CharacterAnimationState state)
{
	// 死亡後は他Stateへ戻さない。
	if (m_State ==
		CharacterAnimationState::Dead &&
		state !=
		CharacterAnimationState::Dead)
	{
		return;
	}

	// 同じStateなら再生し直さない。
	if (m_State ==
		state)
	{
		return;
	}

	m_State =
		state;

	PlayStateAnimation();
}

//=====================================================
// Animation
//=====================================================

void CharacterAnimationComponent::
PlayStateAnimation()
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	auto* animator =
		owner->GetComponent<
		AnimatorComponent>();

	if (!animator)
	{
		return;
	}

	switch (m_State)
	{
	case CharacterAnimationState::Idle:

		animator->PlayAnimation(
			m_IdleAnimation,
			true,
			0.15f);

		break;

	case CharacterAnimationState::Move:

		animator->PlayAnimation(
			m_MoveAnimation,
			true,
			0.15f);

		break;

	case CharacterAnimationState::Attack:

		animator->PlayAnimation(
			m_AttackAnimation,
			false,
			0.10f);

		break;

	case CharacterAnimationState::Dead:

		animator->PlayAnimation(
			m_DeathAnimation,
			false,
			0.15f);

		break;

	default:
		break;
	}
}

//=====================================================
// Debug
//=====================================================

void CharacterAnimationComponent::DrawDebugUI()
{
	ImGui::Text(
		"State: %s",
		GetStateName(
			m_State));

	ImGui::Text(
		"Idle ID: %d",
		static_cast<int>(
			m_IdleAnimation));

	ImGui::Text(
		"Move ID: %d",
		static_cast<int>(
			m_MoveAnimation));

	ImGui::Text(
		"Attack ID: %d",
		static_cast<int>(
			m_AttackAnimation));

	ImGui::Text(
		"Death ID: %d",
		static_cast<int>(
			m_DeathAnimation));
}

//=====================================================
// State Name
//=====================================================

const char*
CharacterAnimationComponent::GetStateName(
	CharacterAnimationState state)
{
	switch (state)
	{
	case CharacterAnimationState::Idle:
		return "Idle";

	case CharacterAnimationState::Move:
		return "Move";

	case CharacterAnimationState::Attack:
		return "Attack";

	case CharacterAnimationState::Dead:
		return "Dead";

	default:
		break;
	}

	return "Unknown";
}
#pragma once

/**
 * @file CharacterAnimationComponent.h
 * @brief Character共通のAnimation状態を管理するComponent
 */

#include "Component.h"
#include "AnimationData.h"

 /**
  * @brief CharacterのAnimation状態
  *
  * @details
  * Gameplay側ではAnimationIDそのものではなく、
  * Characterとしての意味を持つStateを使用する。
  */
enum class CharacterAnimationState
{
	Idle,

	Move,

	/**
	 * @brief 通常の単発Attack
	 */
	Attack,

	/**
	 * @brief 職業固有Animation
	 *
	 * Rangerの
	 * Draw -> Release
	 * などに使用する。
	 */
	CustomAction,

	Dead
};

/**
 * @brief Character Animation Component
 *
 * @details
 * Player / Enemy共通で、
 *
 * Idle
 * Move
 * Attack
 * Dead
 *
 * のAnimation状態を管理する。
 *
 * 実際のSkeletal Animation再生処理は
 * AnimatorComponentへ委譲する。
 */
class CharacterAnimationComponent :
	public Component
{
public:

	CharacterAnimationComponent() = default;
	~CharacterAnimationComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//====================
	// State
	//====================

	/**
	 * @brief Animation状態を変更
	 *
	 * @param state 新しいAnimation状態
	 */
	void ChangeState(
		CharacterAnimationState state);

	/**
	 * @brief 現在のAnimation状態を取得
	 */
	CharacterAnimationState GetState() const
	{
		return m_State;
	}

	/**
	 * @brief 現在Attack Animation中か
	 */
	bool IsAttacking() const
	{
		return
			m_State ==
			CharacterAnimationState::Attack;
	}

	/**
	 * @brief 死亡Animation状態か
	 */
	bool IsDead() const
	{
		return
			m_State ==
			CharacterAnimationState::Dead;
	}

	//====================
	// Animation設定
	//====================

	/**
	 * @brief Idleに使用するAnimation設定
	 */
	void SetIdleAnimation(
		AnimationID id)
	{
		m_IdleAnimation =
			id;
	}

	/**
	 * @brief 移動に使用するAnimation設定
	 */
	void SetMoveAnimation(
		AnimationID id)
	{
		m_MoveAnimation =
			id;
	}

	/**
	 * @brief 攻撃に使用するAnimation設定
	 */
	void SetAttackAnimation(
		AnimationID id)
	{
		m_AttackAnimation =
			id;
	}

	/**
	 * @brief 死亡に使用するAnimation設定
	 */
	void SetDeathAnimation(
		AnimationID id)
	{
		m_DeathAnimation =
			id;
	}

	//=================================================
	// Custom Action
	//=================================================

	/**
	 * @brief 職業固有Animationを再生する
	 *
	 * @details
	 * Rangerの弓など、
	 * CharacterAnimationComponent標準の
	 * Attack 1本では表現できない処理に使用する。
	 */
	void PlayCustomAnimation(
		AnimationID id,
		bool loop = false,
		float blendTime = 0.10f);

	/**
	 * @brief 現在再生しているAnimationが終了したか
	 */
	bool IsCurrentAnimationFinished() const;

	/**
	 * @brief CustomActionなどからIdleへ戻す
	 */
	void ReturnToIdle();

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "CharacterAnimationComponent";
	}

private:

	/**
	 * @brief Stateに対応したAnimationを再生する
	 */
	void PlayStateAnimation();

	/**
	 * @brief State名取得
	 */
	static const char* GetStateName(
		CharacterAnimationState state);

private:

	//====================
	// State
	//====================

	CharacterAnimationState m_State =
		CharacterAnimationState::Idle;

	//====================
	// Animation Mapping
	//====================

	AnimationID m_IdleAnimation =
		AnimationID::Idle;

	AnimationID m_MoveAnimation =
		AnimationID::Walk;

	AnimationID m_AttackAnimation =
		AnimationID::Attack;

	AnimationID m_DeathAnimation =
		AnimationID::Death;
};
#pragma once

/**
 * @file PlayerClassComponent.h
 * @brief Player職業Componentの基底
 */

#include <vector>

#include "Component.h"
#include "AnimationData.h"
#include "PlayerClassType.h"
#include "PlayerActionDefinition.h"

class GameObject;

/**
 * @brief Player Class Component
 *
 * @details
 * PlayerControllerなどの共通処理から
 * 職業固有処理を分離するための基底Component。
 *
 * 共通：
 * ・移動
 * ・HP / MP
 * ・Grid管理
 *
 * 職業固有：
 * ・通常攻撃Animation
 * ・Skill
 * ・攻撃Sequence
 */
class PlayerClassComponent :
	public Component
{
public:

	PlayerClassComponent() = default;

	~PlayerClassComponent() override =
		default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	//=================================================
	// Class
	//=================================================

	virtual PlayerClassType
		GetClassType() const = 0;

	virtual const char*
		GetClassName() const = 0;

	//=================================================
	// Attack
	//=================================================

	/**
	 * @brief 通常攻撃開始
	 *
	 * @return
	 * true : 攻撃開始成功
	 */
	bool BeginNormalAttack(
		GameObject* target);

	/**
	 * @brief 攻撃Sequence終了判定
	 */
	bool IsAttackFinished() const
	{
		return !m_IsAttacking;
	}

	/**
	 * @brief 現在の攻撃対象
	 */
	GameObject* GetAttackTarget() const
	{
		return m_AttackTarget;
	}

	/**
	 * @brief 攻撃判定を発生させるべきか
	 *
	 * @details
	 * trueを返すのは一度だけ。
	 *
	 * PlayerController側がこのイベントを受け取り、
	 * 実際のDamage処理を行う。
	 */
	bool ConsumeAttackHitRequest();

	/**
	 * @brief 指定Actionの職業固有定義を取得する
	 *
	 * @details
	 * AP / MP Costなど、
	 * 職業によって異なるAction情報を返す。
	 *
	 * Move / Itemなど共通Actionについては
	 * Base実装を利用できる。
	 */
	virtual PlayerActionDefinition
		GetActionDefinition(
			PlayerActionType type) const;

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "PlayerClassComponent";
	}

protected:

	/**
	 * @brief 通常攻撃Animation Sequence
	 *
	 * @details
	 * 派生職業がAnimation列を返す。
	 *
	 * Warrior:
	 * Attack
	 *
	 * Ranger:
	 * Attack -> AttackFast
	 */
	virtual std::vector<AnimationID>
		BuildNormalAttackSequence() const = 0;

	/**
	 * @brief どのAnimation終了時にHitを発生させるか
	 *
	 * デフォルトは最後のAnimation。
	 */
	virtual size_t
		GetHitAnimationIndex(
			size_t sequenceSize) const;

	/**
	 * @brief 職業固有Attack Update
	 *
	 * 必要な職業のみoverrideする。
	 */
	virtual void UpdateClassAttack(
		uint64_t delta);

private:

	void StartCurrentAnimation();

	void FinishAttack();

private:

	GameObject*
		m_AttackTarget =
		nullptr;

	std::vector<AnimationID>
		m_AttackSequence;

	size_t
		m_CurrentAnimationIndex =
		0;

	bool
		m_IsAttacking =
		false;

	bool
		m_AttackHitRequested =
		false;

	bool
		m_HitRequestedThisAttack =
		false;
};
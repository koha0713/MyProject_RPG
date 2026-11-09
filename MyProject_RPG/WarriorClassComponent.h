#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Warrior
 *
 * 基本的な近接職。
 */
class WarriorClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Warrior;
	}

	const char* GetClassName()
		const override
	{
		return "Warrior";
	}

	const char* GetComponentName()
		const override
	{
		return "WarriorClassComponent";
	}

	PlayerActionDefinition
		GetActionDefinition(
			PlayerActionType type)
		const override;

protected:

	std::vector<AnimationID>
		BuildNormalAttackSequence()
		const override
	{
		return
		{
			AnimationID::Attack
		};
	}
};

PlayerActionDefinition
WarriorClassComponent::GetActionDefinition(
	PlayerActionType type) const
{
	//=================================================
	// 共通設定取得
	//=================================================

	PlayerActionDefinition definition =
		PlayerClassComponent::
		GetActionDefinition(
			type);

	//=================================================
	// Warrior固有設定
	//=================================================

	switch (type)
	{
	case PlayerActionType::NormalAttack:
	{
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			0;

		definition.Enabled =
			true;

		definition.TargetMode =
			TargetingMode::Unit;

		// 上下左右1マス
		definition.TargetPattern =
			GridPattern::CreateCross(
				1);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		// 近接なので射線判定不要
		definition.Targeting.LineOfSight =
			LineOfSightType::None;

		definition.Targeting.WallsBlock =
			false;

		definition.Targeting.UnitsBlock =
			false;

		definition.Targeting.PierceUnits =
			false;

		definition.Targeting.PierceWalls =
			false;

		// 選択Enemyへ直接効果
		definition.HasEffectPattern =
			false;

		definition.Animation =
			AnimationID::Attack;

		// 以下、Effectの設定は仮値。
		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		// 現段階では仮値。
		// 後でSTR + Weaponなどへ移行する。
		damage.Power =
			3;

		damage.AffectEnemies =
			true;

		damage.AffectAllies =
			false;

		damage.AffectSelf =
			false;

		definition.Effects.push_back(
			damage);

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Enabled =
			true;

		definition.Cost.AP =
			3;

		definition.Cost.MP =
			5;

		definition.TargetMode =
			TargetingMode::Direction;

		//=================================================
		// Effect Pattern
		//=================================================

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::FromMask(
				3,
				2,
				{
					1, 1, 1,
					0, 9, 0
				},
				true);

		definition.Animation =
			AnimationID::Attack;

		//=================================================
		// Damage
		//=================================================

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			0.8f;

		damage.AffectSelf =
			false;

		damage.AffectAllies =
			false;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		break;
	}
	case PlayerActionType::Skill2:
	{
		definition.Enabled =
			true;

		definition.Cost.AP =
			2;

		definition.Cost.MP =
			5;

		//=================================================
		// Unit Target
		//=================================================

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::None;

		definition.Targeting.WallsBlock =
			false;

		definition.Targeting.UnitsBlock =
			false;

		definition.Animation =
			AnimationID::Punch;

		//=================================================
		// Damage
		//=================================================

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			0.7f;

		damage.AffectEnemies =
			true;

		definition.Effects.
			push_back(
				damage);

		//=================================================
		// Knockback
		//=================================================

		EffectDefinition knockback;

		knockback.Type =
			EffectType::Knockback;

		knockback.KnockbackDistance =
			1;

		knockback.AffectEnemies =
			true;

		definition.Effects.
			push_back(
				knockback);

		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Enabled =
			true;

		definition.Cost.AP =
			4;

		definition.Cost.MP =
			20;

		//=================================================
		// Self Center
		//=================================================

		definition.TargetMode =
			TargetingMode::Self;

		definition.Targeting.AllowedTargets =
			TargetType::Self;

		//=================================================
		// Surrounding 8 Cells
		//=================================================

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::CreateSquare(
				1,
				false);

		definition.Animation =
			AnimationID::Attack;

		//=================================================
		// Damage
		//=================================================

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			2.0f;

		damage.AffectEnemies =
			true;

		damage.AffectAllies =
			false;

		damage.AffectSelf =
			false;

		definition.Effects.
			push_back(
				damage);

		break;
	}

	default:
		break;
	}

	return definition;
}
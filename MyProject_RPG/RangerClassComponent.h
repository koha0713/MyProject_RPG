#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Ranger
 *
 * ã|ÇégópÇ∑ÇÈâìãóó£êEÅB
 *
 * í èÌçUåÇÇÕ
 *
 * Draw
 * Å´
 * Release
 *
 * ÇÃ2íiäKAnimationÅB
 */
class RangerClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Ranger;
	}

	const char* GetClassName()
		const override
	{
		return "Ranger";
	}

	const char* GetComponentName()
		const override
	{
		return "RangerClassComponent";
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
		// åªç›ÇÃAnimationíËã`Ç≈ÇÕ
		// Attack -> AttackFastÇ
		// ã|ÇÃà¯Ç´ -> î≠éÀÇ∆ÇµÇƒàµÇ§ÅB
		return
		{
			AnimationID::Attack,
			AnimationID::AttackFast
		};
	}
};

PlayerActionDefinition
RangerClassComponent::GetActionDefinition(
	PlayerActionType type) const
{
	PlayerActionDefinition definition =
		PlayerClassComponent::
		GetActionDefinition(
			type);

	switch (type)
	{
	case PlayerActionType::NormalAttack:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 0;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				5,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		definition.Targeting.UnitsBlock =
			true;

		definition.Animation =
			AnimationID::Attack;

		definition.AnimationLoop =
			false;

		definition.AnimationBlendTime =
			0.10f;

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Enabled = true;

		definition.Cost.AP = 3;
		definition.Cost.MP = 8;

		definition.TargetMode =
			TargetingMode::Direction;

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::CreateLine(
				5,
				false);

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			1.1f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		definition.Animation =
			AnimationID::Attack2;

		definition.AnimationLoop =
			false;

		definition.AnimationBlendTime =
			0.10f;

		break;
	}
	case PlayerActionType::Skill2:
	{
		definition.Enabled = true;

		definition.Cost.AP = 3;
		definition.Cost.MP = 10;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				8,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		definition.Targeting.UnitsBlock =
			true;

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			1.8f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		definition.Animation =
			AnimationID::AttackFast;

		definition.AnimationLoop =
			false;

		definition.AnimationBlendTime =
			0.10f;

		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Enabled = true;

		definition.Cost.AP = 4;
		definition.Cost.MP = 25;

		definition.TargetMode =
			TargetingMode::Grid;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				6,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy |
			TargetType::EmptyCell;

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::CreateSquare(
				2,
				true);

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			1.5f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		definition.Animation =
			AnimationID::Attack3;

		break;
	}

	default:
		break;
	}

	return definition;
}
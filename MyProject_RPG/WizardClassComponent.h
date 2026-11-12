#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Wizard
 */
class WizardClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Wizard;
	}

	const char* GetClassName()
		const override
	{
		return "Wizard";
	}

	const char* GetComponentName()
		const override
	{
		return "WizardClassComponent";
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
		// 未確認：
		// Wizard専用Animation確定後に変更する。
		return
		{
			AnimationID::Attack2
		};
	}
};

PlayerActionDefinition
WizardClassComponent::GetActionDefinition(
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
				3,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		definition.Targeting.UnitsBlock =
			false;

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
		definition.Cost.MP = 10;

		definition.TargetMode =
			TargetingMode::Grid;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				4,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy |
			TargetType::EmptyCell;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		//=============================================
		// 着弾地点周囲
		//=============================================

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::CreateSquare(
				1,
				true);

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			1.3f;

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

		definition.Cost.AP = 2;
		definition.Cost.MP = 12;

		definition.TargetMode =
			TargetingMode::Grid;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				3,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::EmptyCell;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		// Ice Wall生成処理は後で専用Effectへ。
		definition.Animation =
			AnimationID::Attack3;

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
		definition.Cost.MP = 35;

		definition.TargetMode =
			TargetingMode::Grid;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				5,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy |
			TargetType::EmptyCell;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

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
			2.2f;

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

	default:
		break;
	}

	return definition;
}
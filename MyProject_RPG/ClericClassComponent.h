#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Cleric
 *
 * 呪術・支援系職業。
 */
class ClericClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Cleric;
	}

	const char* GetClassName()
		const override
	{
		return "Cleric";
	}

	const char* GetComponentName()
		const override
	{
		return "ClericClassComponent";
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
		// Cleric Animation確定後に変更。
		return
		{
			AnimationID::Attack3
		};
	}
};

PlayerActionDefinition
ClericClassComponent::GetActionDefinition(
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

		definition.Animation =
			AnimationID::Attack;

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 10;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				3,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Self |
			TargetType::Ally;

		EffectDefinition heal;

		heal.Type =
			EffectType::Heal;

		// 現段階では固定値。
		heal.Power =
			30;

		heal.AffectSelf =
			true;

		heal.AffectAllies =
			true;

		heal.AffectEnemies =
			false;

		definition.Effects.push_back(
			heal);

		definition.Animation =
			AnimationID::Attack2;

		break;
	}
	case PlayerActionType::Skill2:
	{
		definition.Enabled = true;

		definition.Cost.AP = 3;
		definition.Cost.MP = 12;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				4,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		// Debuff実処理は後で。
		EffectDefinition debuff;

		debuff.Type =
			EffectType::Debuff;

		debuff.Power =
			1;

		debuff.AffectEnemies =
			true;

		definition.Effects.push_back(
			debuff);

		definition.Animation =
			AnimationID::Attack3;

		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Enabled = true;

		definition.Cost.AP = 4;
		definition.Cost.MP = 35;

		definition.TargetMode =
			TargetingMode::Self;

		definition.HasEffectPattern =
			true;

		definition.EffectPattern =
			GridPattern::CreateSquare(
				2,
				true);

		EffectDefinition heal;

		heal.Type =
			EffectType::Heal;

		heal.Power =
			50;

		heal.AffectSelf =
			true;

		heal.AffectAllies =
			true;

		heal.AffectEnemies =
			false;

		definition.Effects.push_back(
			heal);

		definition.Animation =
			AnimationID::AttackFast;

		break;
	}

	default:
		break;
	}

	return definition;
}
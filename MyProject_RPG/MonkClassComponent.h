#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Monk
 */
class MonkClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Monk;
	}

	const char* GetClassName()
		const override
	{
		return "Monk";
	}

	const char* GetComponentName()
		const override
	{
		return "MonkClassComponent";
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
			AnimationID::Punch
		};
	}
};

PlayerActionDefinition
MonkClassComponent::GetActionDefinition(
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
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			0;

		definition.Enabled =
			true;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

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

		definition.HasEffectPattern =
			false;

		break;
	}
	case PlayerActionType::Skill1:
	{
		// òAåÇ
		definition.Cost.AP = 2;
		definition.Cost.MP = 5;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Skill2:
	{
		// ì•Ç›çûÇ›
		definition.Cost.AP = 2;
		definition.Cost.MP = 5;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Ultimate:
	{
		// ïSóÙåù
		definition.Cost.AP = 4;
		definition.Cost.MP = 20;
		definition.Enabled = true;

		break;
	}

	default:
		break;
	}

	return definition;
}
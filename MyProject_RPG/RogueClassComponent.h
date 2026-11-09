#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Rogue
 */
class RogueClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Rogue;
	}

	const char* GetClassName()
		const override
	{
		return "Rogue";
	}

	const char* GetComponentName()
		const override
	{
		return "RogueClassComponent";
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
		// 高速攻撃を仮使用。
		return
		{
			AnimationID::AttackFast
		};
	}
};

PlayerActionDefinition
RogueClassComponent::GetActionDefinition(
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
		// バックスタブ
		definition.Cost.AP = 2;
		definition.Cost.MP = 8;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Skill2:
	{
		// シャドウステップ
		definition.Cost.AP = 2;
		definition.Cost.MP = 10;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Ultimate:
	{
		// 暗殺
		definition.Cost.AP = 4;
		definition.Cost.MP = 25;
		definition.Enabled = true;

		break;
	}

	default:
		break;
	}

	return definition;
}
#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Cleric
 *
 * éÙèpÅEéxâáånêEã∆ÅB
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
		// ñ¢ämîFÅF
		// Cleric AnimationämíËå„Ç…ïœçXÅB
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
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			0;

		definition.Enabled =
			true;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				3);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

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
		// é°ñ¸ÇÃéÙ
		definition.Cost.AP = 2;
		definition.Cost.MP = 10;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Skill2:
	{
		// ë©îõÇÃéÙ
		definition.Cost.AP = 3;
		definition.Cost.MP = 12;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Ultimate:
	{
		// ê∂ñΩÇÃãVéÆ
		definition.Cost.AP = 4;
		definition.Cost.MP = 35;
		definition.Enabled = true;

		break;
	}

	default:
		break;
	}

	return definition;
}
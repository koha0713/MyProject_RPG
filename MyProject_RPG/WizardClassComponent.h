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

		// 味方やEnemy越しに魔法弾を飛ばせるかは
		// 今後調整可能。
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
		// ファイアボール
		definition.Cost.AP = 3;
		definition.Cost.MP = 10;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Skill2:
	{
		// アイスウォール
		definition.Cost.AP = 2;
		definition.Cost.MP = 12;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Ultimate:
	{
		// メテオ
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
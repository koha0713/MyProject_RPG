#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Ranger
 *
 * 弓を使用する遠距離職。
 *
 * 通常攻撃は
 *
 * Draw
 * ↓
 * Release
 *
 * の2段階Animation。
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
		// 現在のAnimation定義では
		// Attack -> AttackFastを
		// 弓の引き -> 発射として扱う。
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
				5);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Targeting.LineOfSight =
			LineOfSightType::Direct;

		definition.Targeting.WallsBlock =
			true;

		definition.Targeting.UnitsBlock =
			true;

		definition.Targeting.PierceUnits =
			false;

		definition.Targeting.PierceWalls =
			false;

		definition.Targeting.StopOnFirstUnit =
			true;

		definition.Targeting.StopOnFirstWall =
			true;

		definition.HasEffectPattern =
			false;

		break;
	}
	case PlayerActionType::Skill1:
	{
		// 貫通射撃
		definition.Cost.AP = 3;
		definition.Cost.MP = 8;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Skill2:
	{
		// 狙撃
		definition.Cost.AP = 3;
		definition.Cost.MP = 10;
		definition.Enabled = true;

		break;
	}

	case PlayerActionType::Ultimate:
	{
		// アローレイン
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
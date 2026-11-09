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

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Cost.AP =
			3;

		definition.Cost.MP =
			5;

		definition.Enabled =
			true;

		definition.TargetMode =
			TargetingMode::Direction;

		definition.Targeting.AllowedTargets =
			TargetType::Enemy |
			TargetType::EmptyCell;

		definition.Targeting.LineOfSight =
			LineOfSightType::None;

		definition.HasEffectPattern =
			true;

		// North向きを基準として、
		// Characterの向きに合わせて回転する。
		definition.EffectPattern =
			GridPattern::FromMask(
				3,
				2,
				{
					1, 1, 1,
					0, 9, 0
				},
				true);

		break;
	}
	case PlayerActionType::Skill2:
	{
		definition.Cost.AP =
			2;

		definition.Cost.MP =
			5;

		definition.Enabled =
			true;

		// Target / Effectは
		// Knockback実装時に設定する。
		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Cost.AP =
			4;

		definition.Cost.MP =
			20;

		definition.Enabled =
			true;

		definition.TargetMode =
			TargetingMode::Self;

		definition.Targeting.AllowedTargets =
			TargetType::Self;

		definition.Targeting.LineOfSight =
			LineOfSightType::None;

		definition.HasEffectPattern =
			true;

		// 自分を除いた周囲8マス
		definition.EffectPattern =
			GridPattern::CreateSquare(
				1);

		break;
	}

	default:
		break;
	}

	return definition;
}
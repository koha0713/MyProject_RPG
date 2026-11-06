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

	/**
	 * @brief 通常攻撃射程
	 */
	int GetNormalAttackRange()
		const override
	{
		return 3;
	}

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
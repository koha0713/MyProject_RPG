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

	/**
	 * @brief 通常攻撃射程
	 */
	int GetNormalAttackRange()
		const override
	{
		return 2;
	}

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
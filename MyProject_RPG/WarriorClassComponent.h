#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Warrior
 *
 * Šî–{“I‚È‹ßÚEB
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

	/**
	 * @brief ’ÊíUŒ‚Ë’ö
	 */
	int GetNormalAttackRange()
		const override
	{
		return 1;
	}

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
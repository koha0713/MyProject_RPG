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
			AnimationID::Punch
		};
	}
};
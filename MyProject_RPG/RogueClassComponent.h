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
		// ‚‘¬UŒ‚‚ğ‰¼g—pB
		return
		{
			AnimationID::AttackFast
		};
	}
};
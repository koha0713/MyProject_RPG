#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Ranger
 *
 * ‹|‚ğg—p‚·‚é‰“‹——£EB
 *
 * ’ÊíUŒ‚‚Í
 *
 * Draw
 * «
 * Release
 *
 * ‚Ì2’iŠKAnimationB
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

	/**
	 * @brief ’ÊíUŒ‚Ë’ö
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
		// Œ»İ‚ÌAnimation’è‹`‚Å‚Í
		// Attack -> AttackFast‚ğ
		// ‹|‚Ìˆø‚« -> ”­Ë‚Æ‚µ‚Äˆµ‚¤B
		return
		{
			AnimationID::Attack,
			AnimationID::AttackFast
		};
	}
};
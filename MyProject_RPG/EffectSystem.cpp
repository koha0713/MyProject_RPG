#include "EffectSystem.h"

#include <cstdlib>

#include "GameObject.h"
#include "GridMap.h"

#include "CharacterStatusComponent.h"
#include "GridPositionComponent.h"
#include "TransformComponent.h"

//=====================================================
// Apply Effect
//=====================================================

bool EffectSystem::ApplyEffect(
	GridMap& map,
	GameObject* source,
	GameObject* target,
	const EffectDefinition& effect)
{
	if (!source ||
		!target)
	{
		return false;
	}

	if (!CanAffectTarget(
		source,
		target,
		effect))
	{
		return false;
	}

	switch (effect.Type)
	{
	case EffectType::Damage:
		return ApplyDamage(
			source,
			target,
			effect);

	case EffectType::Heal:
		return ApplyHeal(
			source,
			target,
			effect);

	case EffectType::Knockback:
		return ApplyKnockback(
			map,
			source,
			target,
			effect);

	default:
		break;
	}

	return false;
}

//=====================================================
// Apply Effect To Cell
//=====================================================

bool EffectSystem::ApplyEffectToCell(
	GridMap& map,
	GameObject* source,
	const GridPosition& targetPosition,
	const EffectDefinition& effect)
{
	GameObject* target =
		map.GetOccupant(
			targetPosition);

	if (!target)
	{
		return false;
	}

	return ApplyEffect(
		map,
		source,
		target,
		effect);
}

//=====================================================
// Target Filter
//=====================================================

bool EffectSystem::CanAffectTarget(
	GameObject* source,
	GameObject* target,
	const EffectDefinition& effect)
{
	if (!source ||
		!target)
	{
		return false;
	}

	//=================================================
	// Self
	//=================================================

	if (source ==
		target)
	{
		return effect.AffectSelf;
	}

	const Tag sourceTag =
		source->GetTag();

	const Tag targetTag =
		target->GetTag();

	//=================================================
	// Ally
	//=================================================

	if (sourceTag ==
		targetTag)
	{
		return effect.AffectAllies;
	}

	//=================================================
	// Enemy
	//=================================================

	if ((sourceTag == Tag::Player &&
		targetTag == Tag::Enemy) ||
		(sourceTag == Tag::Enemy &&
			targetTag == Tag::Player))
	{
		return effect.AffectEnemies;
	}

	return false;
}

//=====================================================
// Damage
//=====================================================

bool EffectSystem::ApplyDamage(
	GameObject* source,
	GameObject* target,
	const EffectDefinition& effect)
{
	(void)source;

	auto* status =
		target->GetComponent<
		CharacterStatusComponent>();

	if (!status ||
		status->IsDead())
	{
		return false;
	}

	if (effect.Power <= 0)
	{
		return false;
	}

	status->TakeDamage(
		effect.Power);

	return true;
}

//=====================================================
// Heal
//=====================================================

bool EffectSystem::ApplyHeal(
	GameObject* source,
	GameObject* target,
	const EffectDefinition& effect)
{
	(void)source;

	auto* status =
		target->GetComponent<
		CharacterStatusComponent>();

	if (!status ||
		status->IsDead())
	{
		return false;
	}

	if (effect.Power <= 0)
	{
		return false;
	}

	status->RecoverHP(
		effect.Power);

	return true;
}

//=====================================================
// Knockback
//=====================================================

bool EffectSystem::ApplyKnockback(
	GridMap& map,
	GameObject* source,
	GameObject* target,
	const EffectDefinition& effect)
{
	auto* targetStatus =
		target->GetComponent<
		CharacterStatusComponent>();

	if (targetStatus &&
		targetStatus->IsDead())
	{
		return false;
	}

	if (!source ||
		!target ||
		effect.KnockbackDistance <= 0)
	{
		return false;
	}

	auto* sourceGrid =
		source->GetComponent<
		GridPositionComponent>();

	auto* targetGrid =
		target->GetComponent<
		GridPositionComponent>();

	if (!sourceGrid ||
		!targetGrid)
	{
		return false;
	}

	GridPosition sourcePosition =
		sourceGrid->
		GetGridPosition();

	GridPosition targetPosition =
		targetGrid->
		GetGridPosition();

	//=================================================
	// Knockback方向を計算
	//=================================================

	int directionX =
		0;

	int directionY =
		0;

	const int deltaX =
		targetPosition.X -
		sourcePosition.X;

	const int deltaY =
		targetPosition.Y -
		sourcePosition.Y;

	// より大きい軸を優先して1方向へ押す。
	if (std::abs(deltaX) >=
		std::abs(deltaY))
	{
		directionX =
			deltaX > 0
			? 1
			: -1;
	}
	else
	{
		directionY =
			deltaY > 0
			? 1
			: -1;
	}

	//=================================================
	// 1Cellずつ移動
	//=================================================

	GridPosition current =
		targetPosition;

	for (int i = 0;
		i < effect.KnockbackDistance;
		++i)
	{
		const GridPosition next
		{
			current.X + directionX,
			current.Y + directionY
		};

		// 壁 / Occupantがある場合は停止。
		if (!map.CanMoveTo(
			next))
		{
			break;
		}

		current =
			next;
	}

	// 1Cellも動けなかった。
	if (current.X ==
		targetPosition.X &&
		current.Y ==
		targetPosition.Y)
	{
		return false;
	}

	//=================================================
	// Grid Position更新
	//=================================================
	//
	// GridPositionComponent::SetGridPosition()へ
	// Grid占有状態の更新を一本化する。
	//=================================================

	if (!targetGrid->
		SetGridPosition(
			current))
	{
		OutputDebugStringA(
			"[EffectSystem] "
			"Knockback SetGridPosition failed.\n");

		return false;
	}

	//=================================================
	// Visual Position更新
	//=================================================

	auto* transform =
		target->GetComponent<
		TransformComponent>();

	if (transform)
	{
		Vector3 worldPosition =
			map.GridToWorld(
				current);

		// Character固有の高さを維持する。
		worldPosition.y =
			transform->
			GetPosition().y;

		transform->
			SetPosition(
				worldPosition);
	}

	return true;
}
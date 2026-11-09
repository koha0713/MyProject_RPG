#include "PlayerActionComponent.h"

#include "GameObject.h"
#include "CharacterStatusComponent.h"
#include "PlayerClassComponent.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void PlayerActionComponent::Initialize()
{
}

void PlayerActionComponent::Finalize()
{
}

void PlayerActionComponent::Update(
	uint64_t delta)
{
	(void)delta;
}

void PlayerActionComponent::Draw()
{
}

//=====================================================
// Status
//=====================================================

CharacterStatusComponent*
PlayerActionComponent::GetStatus() const
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return nullptr;
	}

	return owner->GetComponent<
		CharacterStatusComponent>();
}

//=====================================================
// Action Definition
//=====================================================

PlayerActionDefinition
PlayerActionComponent::GetActionDefinition(
	PlayerActionType type) const
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		PlayerActionDefinition definition;

		definition.Type =
			type;

		definition.Enabled =
			false;

		return definition;
	}

	//=================================================
	// Player Class
	//=================================================

	auto* playerClass =
		owner->GetComponent<
		PlayerClassComponent>();

	if (!playerClass)
	{
		PlayerActionDefinition definition;

		definition.Type =
			type;

		definition.Enabled =
			false;

		OutputDebugStringA(
			"[PlayerAction] "
			"PlayerClassComponent not found.\n");

		return definition;
	}

	return playerClass->
		GetActionDefinition(
			type);
}

//=====================================================
// Can Pay Cost
//=====================================================

bool PlayerActionComponent::CanPayCost(
	const PlayerActionCost& cost) const
{
	auto* status =
		GetStatus();

	if (!status)
	{
		return false;
	}

	if (!status->CanConsumeAP(
		cost.AP))
	{
		return false;
	}

	if (!status->CanConsumeMP(
		cost.MP))
	{
		return false;
	}

	return true;
}

//=====================================================
// Can Use Action
//=====================================================
bool PlayerActionComponent::CanUseAction(
	PlayerActionType type) const
{
	const PlayerActionDefinition definition =
		GetActionDefinition(
			type);

	// 未実装・使用禁止Action。
	if (!definition.Enabled)
	{
		return false;
	}

	return CanPayCost(
		definition.Cost);
}

//=====================================================
// Consume Cost
//=====================================================

bool PlayerActionComponent::ConsumeCost(
	const PlayerActionCost& cost)
{
	auto* status =
		GetStatus();

	if (!status)
	{
		return false;
	}

	//=================================================
	// 先に両方確認
	//=================================================
	//
	// APだけ消費された後にMP不足、
	// という半端な状態を防ぐ。
	//=================================================

	if (!CanPayCost(
		cost))
	{
		return false;
	}

	//=================================================
	// Consume
	//=================================================

	if (!status->ConsumeAP(
		cost.AP))
	{
		return false;
	}

	if (!status->ConsumeMP(
		cost.MP))
	{
		//=================================================
		// 通常ここには来ない。
		//
		// CanPayCost()済みなので、
		// 外部から同Frame内でMPを書き換えない限り
		// ConsumeMPは成功する。
		//=================================================

		status->RecoverAP(
			cost.AP);

		return false;
	}

	return true;
}

//=====================================================
// Consume Action Cost
//=====================================================
bool PlayerActionComponent::ConsumeActionCost(
	PlayerActionType type)
{
	const PlayerActionDefinition definition =
		GetActionDefinition(
			type);

	if (!definition.Enabled)
	{
		return false;
	}

	return ConsumeCost(
		definition.Cost);
}

//=====================================================
// Begin Turn
//=====================================================

void PlayerActionComponent::BeginTurn()
{
	auto* status =
		GetStatus();

	if (!status)
	{
		return;
	}

	status->ResetAP();
}

//=====================================================
// Has Usable Action
//=====================================================
bool PlayerActionComponent::HasUsableAction() const
{
	const PlayerActionType actionTypes[] =
	{
		PlayerActionType::Move,
		PlayerActionType::NormalAttack,
		PlayerActionType::Skill1,
		PlayerActionType::Skill2,
		PlayerActionType::Ultimate,
		PlayerActionType::Item
	};

	for (const PlayerActionType type :
	actionTypes)
	{
		if (CanUseAction(
			type))
		{
			return true;
		}
	}

	return false;
}

//=====================================================
// Debug UI
//=====================================================

void PlayerActionComponent::DrawDebugUI()
{
	auto* status =
		GetStatus();

	if (!status)
	{
		ImGui::Text(
			"CharacterStatus: None");

		return;
	}

	const PlayerActionType actionTypes[] =
	{
		PlayerActionType::Move,
		PlayerActionType::NormalAttack,
		PlayerActionType::Skill1,
		PlayerActionType::Skill2,
		PlayerActionType::Ultimate,
		PlayerActionType::Item
	};

	const char* actionNames[] =
	{
		"Move",
		"Normal Attack",
		"Skill 1",
		"Skill 2",
		"Ultimate",
		"Item"
	};

	for (int i = 0;
		i < 6;
		++i)
	{
		const PlayerActionDefinition definition =
			GetActionDefinition(
				actionTypes[i]);

		ImGui::Text(
			"%s : AP=%d MP=%d [%s]",
			actionNames[i],
			definition.Cost.AP,
			definition.Cost.MP,
			CanUseAction(
				actionTypes[i])
			? "OK"
			: "NG");
	}
}
#include "CharacterStatusComponent.h"

#include <algorithm>

#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void CharacterStatusComponent::Initialize()
{
}

void CharacterStatusComponent::Finalize()
{
}

void CharacterStatusComponent::Update(
	uint64_t delta)
{
	(void)delta;
}

void CharacterStatusComponent::Draw()
{
}

//=====================================================
// HP
//=====================================================

void CharacterStatusComponent::SetMaxHP(
	int hp)
{
	m_MaxHP =
		(std::max)(
			hp,
			1);

	// ‰Šúİ’è‚ÍÅ‘åHP‚Ü‚Å‰ñ•œ‚·‚éB
	m_HP =
		m_MaxHP;
}

void CharacterStatusComponent::TakeDamage(
	int damage)
{
	if (damage <= 0 ||
		IsDead())
	{
		return;
	}

	m_HP -=
		damage;

	if (m_HP < 0)
	{
		m_HP =
			0;
	}

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[CharacterStatus] Damage=%d HP=%d/%d\n",
		damage,
		m_HP,
		m_MaxHP);

	OutputDebugStringA(
		buffer);

	if (IsDead())
	{
		OutputDebugStringA(
			"[CharacterStatus] Dead\n");
	}
}

//=====================================================
// Attack
//=====================================================

void CharacterStatusComponent::SetAttackPower(
	int power)
{
	m_AttackPower =
		(std::max)(
			power,
			0);
}

//=====================================================
// Agility
//=====================================================
void CharacterStatusComponent::SetAgility(
	int agility)
{
	m_Agility =
		(std::max)(
			agility,
			1);
}

//=====================================================
// Debug UI
//=====================================================

void CharacterStatusComponent::DrawDebugUI()
{
	ImGui::Text(
		"HP: %d / %d",
		m_HP,
		m_MaxHP);

	ImGui::Text(
		"Attack Power: %d",
		m_AttackPower);

	ImGui::Text(
		"Agility: %d",
		m_Agility);

	ImGui::Text(
		"State: %s",
		IsDead()
		? "Dead"
		: "Alive");
}
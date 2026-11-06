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

	// 初期設定時は最大HPまで回復する。
	m_HP =
		m_MaxHP;
}

//=====================================================
// Damage
//=====================================================

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
// Recover HP
//=====================================================

void CharacterStatusComponent::RecoverHP(
	int amount)
{
	if (amount <= 0 ||
		IsDead())
	{
		return;
	}

	m_HP +=
		amount;

	if (m_HP > m_MaxHP)
	{
		m_HP =
			m_MaxHP;
	}
}

//=====================================================
// MP
//=====================================================

void CharacterStatusComponent::SetMaxMP(
	int mp)
{
	// MPを使用しないCharacterは0を許可する。
	m_MaxMP =
		(std::max)(
			mp,
			0);

	m_MP =
		m_MaxMP;
}

//=====================================================
// Consume MP
//=====================================================

bool CharacterStatusComponent::ConsumeMP(
	int amount)
{
	if (amount <= 0)
	{
		return false;
	}

	if (m_MP < amount)
	{
		return false;
	}

	m_MP -=
		amount;

	return true;
}

//=====================================================
// Recover MP
//=====================================================

void CharacterStatusComponent::RecoverMP(
	int amount)
{
	if (amount <= 0)
	{
		return;
	}

	m_MP +=
		amount;

	if (m_MP > m_MaxMP)
	{
		m_MP =
			m_MaxMP;
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
		"MP: %d / %d",
		m_MP,
		m_MaxMP);

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
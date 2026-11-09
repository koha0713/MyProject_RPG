#include "CharacterStatusComponent.h"

#include <algorithm>

#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================

void CharacterStatusComponent::Initialize()
{
	//=================================================
	// Resource Clamp
	//=================================================

	m_HP =
		std::clamp(
			m_HP,
			0,
			m_MaxHP);

	m_MP =
		std::clamp(
			m_MP,
			0,
			m_MaxMP);

	m_AP =
		std::clamp(
			m_AP,
			0,
			m_MaxAP);
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
	// 最大HPは最低1。
	m_MaxHP =
		(std::max)(
			1,
			hp);

	// Character生成時の使用を想定して
	// 現在HPも最大値まで回復する。
	m_HP =
		m_MaxHP;
}

//=====================================================
// Damage
//=====================================================

void CharacterStatusComponent::TakeDamage(
	int damage)
{
	if (damage <= 0)
	{
		return;
	}

	if (IsDead())
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
		"[CharacterStatus] "
		"Damage=%d HP=%d/%d\n",
		damage,
		m_HP,
		m_MaxHP);

	OutputDebugStringA(
		buffer);
}

//=====================================================
// Recover HP
//=====================================================

void CharacterStatusComponent::RecoverHP(
	int amount)
{
	if (amount <= 0)
	{
		return;
	}

	// 現仕様では戦闘不能Characterを
	// 通常回復では復活させない。
	if (IsDead())
	{
		return;
	}

	m_HP =
		(std::min)(
			m_HP + amount,
			m_MaxHP);
}

//=====================================================
// MP
//=====================================================

void CharacterStatusComponent::SetMaxMP(
	int mp)
{
	m_MaxMP =
		(std::max)(
			0,
			mp);

	m_MP =
		m_MaxMP;
}

bool CharacterStatusComponent::CanConsumeMP(
	int amount) const
{
	if (amount < 0)
	{
		return false;
	}

	return
		m_MP >= amount;
}

bool CharacterStatusComponent::ConsumeMP(
	int amount)
{
	if (!CanConsumeMP(
		amount))
	{
		return false;
	}

	m_MP -=
		amount;

	return true;
}

void CharacterStatusComponent::RecoverMP(
	int amount)
{
	if (amount <= 0)
	{
		return;
	}

	m_MP =
		(std::min)(
			m_MP + amount,
			m_MaxMP);
}

//=====================================================
// AP
//=====================================================

void CharacterStatusComponent::SetMaxAP(
	int ap)
{
	m_MaxAP =
		(std::max)(
			0,
			ap);

	// Character生成時の利用を想定。
	m_AP =
		m_MaxAP;
}

bool CharacterStatusComponent::CanConsumeAP(
	int amount) const
{
	if (amount < 0)
	{
		return false;
	}

	return
		m_AP >= amount;
}

bool CharacterStatusComponent::ConsumeAP(
	int amount)
{
	if (!CanConsumeAP(
		amount))
	{
		return false;
	}

	m_AP -=
		amount;

	return true;
}

void CharacterStatusComponent::RecoverAP(
	int amount)
{
	if (amount <= 0)
	{
		return;
	}

	m_AP =
		(std::min)(
			m_AP + amount,
			m_MaxAP);
}

void CharacterStatusComponent::ResetAP()
{
	m_AP =
		m_MaxAP;
}

//=====================================================
// STR
//=====================================================

void CharacterStatusComponent::SetStrength(
	int strength)
{
	m_Strength =
		(std::max)(
			1,
			strength);
}

//=====================================================
// DEX
//=====================================================

void CharacterStatusComponent::SetDexterity(
	int dexterity)
{
	m_Dexterity =
		(std::max)(
			1,
			dexterity);
}

//=====================================================
// AGI
//=====================================================

void CharacterStatusComponent::SetAgility(
	int agility)
{
	m_Agility =
		(std::max)(
			1,
			agility);
}

//=====================================================
// PER
//=====================================================

void CharacterStatusComponent::SetPerception(
	int perception)
{
	m_Perception =
		(std::max)(
			1,
			perception);
}

//=====================================================
// Move Range
//=====================================================

void CharacterStatusComponent::SetBaseMoveRange(
	int range)
{
	m_BaseMoveRange =
		(std::max)(
			0,
			range);
}

int CharacterStatusComponent::
CalculateMoveRange() const
{
	//=================================================
	// 現段階
	//=================================================
	//
	// AGIは能力値として保持するが、
	// 移動距離へ直接変換しない。
	//
	// 将来的にはここへ、
	//
	// ・AGI
	// ・装備重量
	// ・Squad補正
	// ・状態異常
	// ・Buff / Debuff
	//
	// を加味する。
	//=================================================

	return
		m_BaseMoveRange;
}

//=====================================================
// Legacy Attack Power
//=====================================================

void CharacterStatusComponent::SetAttackPower(
	int power)
{
	m_AttackPower =
		(std::max)(
			0,
			power);
}

//=====================================================
// Debug UI
//=====================================================

void CharacterStatusComponent::DrawDebugUI()
{
	//=================================================
	// Resource
	//=================================================

	ImGui::Text(
		"HP: %d / %d",
		m_HP,
		m_MaxHP);

	ImGui::Text(
		"MP: %d / %d",
		m_MP,
		m_MaxMP);

	ImGui::Text(
		"AP: %d / %d",
		m_AP,
		m_MaxAP);

	ImGui::Separator();

	//=================================================
	// Ability
	//=================================================

	ImGui::Text(
		"STR: %d",
		m_Strength);

	ImGui::Text(
		"DEX: %d",
		m_Dexterity);

	ImGui::Text(
		"AGI: %d",
		m_Agility);

	ImGui::Text(
		"PER: %d",
		m_Perception);

	ImGui::Separator();

	//=================================================
	// Movement
	//=================================================

	ImGui::Text(
		"Base Move Range: %d",
		m_BaseMoveRange);

	ImGui::Text(
		"Calculated Move Range: %d",
		CalculateMoveRange());

	ImGui::Separator();

	//=================================================
	// Legacy
	//=================================================

	ImGui::Text(
		"Legacy Attack Power: %d",
		m_AttackPower);

	ImGui::Separator();

	//=================================================
	// State
	//=================================================

	ImGui::Text(
		"State: %s",
		IsDead()
		? "Dead"
		: "Alive");
}
#pragma once

/**
 * @file CharacterStatusComponent.h
 * @brief Character共通の能力値を管理するComponent
 */

#include "Component.h"

 /**
  * @brief Character Status Component
  *
  * @details
  * Player / Enemy共通の基本能力値を管理する。
  *
  * 現段階ではBase値を直接保持する。
  *
  * 将来的には、
  *
  * Base Status
  * + Level補正
  * + Equipment補正
  * + Buff / Debuff
  * = Final Status
  *
  * という構造へ拡張する。
  */
class CharacterStatusComponent :
	public Component
{
public:

	CharacterStatusComponent() = default;

	~CharacterStatusComponent() override =
		default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//=================================================
	// HP
	//=================================================

	void SetMaxHP(
		int hp);

	int GetHP() const
	{
		return m_HP;
	}

	int GetMaxHP() const
	{
		return m_MaxHP;
	}

	/**
	 * @brief Damageを受ける
	 */
	void TakeDamage(
		int damage);

	/**
	 * @brief HP回復
	 */
	void RecoverHP(
		int amount);

	bool IsDead() const
	{
		return
			m_HP <= 0;
	}

	//=================================================
	// MP
	//=================================================

	void SetMaxMP(
		int mp);

	int GetMP() const
	{
		return m_MP;
	}

	int GetMaxMP() const
	{
		return m_MaxMP;
	}

	/**
	 * @brief MPを消費可能か
	 */
	bool CanConsumeMP(
		int amount) const;

	/**
	 * @brief MP消費
	 *
	 * @return
	 * 消費成功時true
	 */
	bool ConsumeMP(
		int amount);

	/**
	 * @brief MP回復
	 */
	void RecoverMP(
		int amount);

	//=================================================
	// AP
	//=================================================

	/**
	 * @brief 最大APを設定
	 *
	 * @details
	 * 基本仕様では全Player共通4。
	 * 将来的なBuff等を考慮しSetterは残す。
	 */
	void SetMaxAP(
		int ap);

	int GetAP() const
	{
		return m_AP;
	}

	int GetMaxAP() const
	{
		return m_MaxAP;
	}

	/**
	 * @brief APを消費可能か
	 */
	bool CanConsumeAP(
		int amount) const;

	/**
	 * @brief AP消費
	 *
	 * @return
	 * 消費成功時true
	 */
	bool ConsumeAP(
		int amount);

	/**
	 * @brief AP回復
	 */
	void RecoverAP(
		int amount);

	/**
	 * @brief Turn開始時などにAPを全回復する
	 */
	void ResetAP();

	//=================================================
	// STR
	//=================================================

	void SetStrength(
		int strength);

	int GetStrength() const
	{
		return m_Strength;
	}

	//=================================================
	// DEX
	//=================================================

	void SetDexterity(
		int dexterity);

	int GetDexterity() const
	{
		return m_Dexterity;
	}

	//=================================================
	// AGI
	//=================================================

	/**
	 * @brief 敏捷値設定
	 *
	 * @details
	 * AGIは移動マス数そのものではなく、
	 * 移動性能などの計算に利用する能力値。
	 */
	void SetAgility(
		int agility);

	int GetAgility() const
	{
		return m_Agility;
	}

	//=================================================
	// PER
	//=================================================

	void SetPerception(
		int perception);

	int GetPerception() const
	{
		return m_Perception;
	}

	//=================================================
	// Move
	//=================================================

	/**
	 * @brief 基本移動距離設定
	 *
	 * @details
	 * 現仕様では、
	 * 2AP消費時に移動できる基本マス数。
	 */
	void SetBaseMoveRange(
		int range);

	int GetBaseMoveRange() const
	{
		return m_BaseMoveRange;
	}

	/**
	 * @brief 実際の移動距離を計算
	 *
	 * @details
	 * 現段階ではBaseMoveRangeをそのまま返す。
	 *
	 * 将来的に、
	 * AGI / 装備重量 / 状態異常 / Buff
	 * などを加味する。
	 */
	int CalculateMoveRange() const;

	//=================================================
	// Legacy Attack Power
	//=================================================
	//
	// 現在のPlayerController / EnemyAIとの
	// 互換性維持用。
	//
	// PlayerActionSystem完成後は、
	// STR / DEX / Weapon等からDamage計算する
	// BattleCalculatorへ置き換える予定。
	//=================================================

	void SetAttackPower(
		int power);

	int GetAttackPower() const
	{
		return m_AttackPower;
	}

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName()
		const override
	{
		return
			"CharacterStatusComponent";
	}

private:

	//=================================================
	// Resource
	//=================================================

	int m_HP =
		10;

	int m_MaxHP =
		10;

	int m_MP =
		0;

	int m_MaxMP =
		0;

	int m_AP =
		4;

	int m_MaxAP =
		4;

	//=================================================
	// Base Ability
	//=================================================

	int m_Strength =
		1;

	int m_Dexterity =
		1;

	int m_Agility =
		1;

	int m_Perception =
		1;

	//=================================================
	// Movement
	//=================================================

	int m_BaseMoveRange =
		2;

	//=================================================
	// Legacy
	//=================================================

	int m_AttackPower =
		1;
};
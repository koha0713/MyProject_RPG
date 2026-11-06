#pragma once

/**
 * @file CharacterStatusComponent.h
 * @brief キャラクターの基本ステータスを管理するComponent
 */

#include "Component.h"

 /**
  * @brief Character Status Component
  *
  * @details
  * Player / Enemy 共通の基本ステータスを保持する。
  *
  * 管理対象
  * ・HP
  * ・MP
  * ・攻撃力
  * ・敏捷値
  *
  * 職業固有のSkill処理などはここへ入れず、
  * 別Componentへ分離する。
  */
class CharacterStatusComponent :
	public Component
{
public:

	CharacterStatusComponent() = default;
	~CharacterStatusComponent() override = default;

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

	/**
	 * @brief 最大HP設定
	 *
	 * 初期設定時はHPも最大値まで回復する。
	 */
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
	 * @brief ダメージを受ける
	 */
	void TakeDamage(
		int damage);

	/**
	 * @brief HPを回復する
	 */
	void RecoverHP(
		int amount);

	/**
	 * @brief 死亡判定
	 */
	bool IsDead() const
	{
		return m_HP <= 0;
	}

	//=================================================
	// MP
	//=================================================

	/**
	 * @brief 最大MP設定
	 *
	 * 0を許可する。
	 * MPを使用しない職業・Enemyでは0でよい。
	 */
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
	 * @brief MP消費
	 *
	 * @return
	 * true  : 消費成功
	 * false : MP不足
	 */
	bool ConsumeMP(
		int amount);

	/**
	 * @brief MP回復
	 */
	void RecoverMP(
		int amount);

	//=================================================
	// Attack
	//=================================================

	void SetAttackPower(
		int power);

	int GetAttackPower() const
	{
		return m_AttackPower;
	}

	//=================================================
	// Agility
	//=================================================

	/**
	 * @brief 敏捷値設定
	 *
	 * 現在は1PlayerTurn中の移動可能マス数として使用する。
	 */
	void SetAgility(
		int agility);

	int GetAgility() const
	{
		return m_Agility;
	}

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "CharacterStatusComponent";
	}

private:

	//=================================================
	// HP
	//=================================================

	int m_HP =
		10;

	int m_MaxHP =
		10;

	//=================================================
	// MP
	//=================================================

	int m_MP =
		0;

	int m_MaxMP =
		0;

	//=================================================
	// Battle
	//=================================================

	int m_AttackPower =
		1;

	/**
	 * @brief 敏捷値
	 *
	 * 現段階では
	 * 1PlayerTurn中の移動可能マス数。
	 */
	int m_Agility =
		1;
};
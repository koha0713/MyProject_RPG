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
  * 現段階では、
  * ・HP
  * ・最大HP
  * ・攻撃力
  *
  * のみ管理する。
  *
  * 将来的に敏捷値、積載量、状態異常などを追加する。
  */
class CharacterStatusComponent :
	public Component
{
public:

	CharacterStatusComponent() = default;
	~CharacterStatusComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// HP
	//====================

	void SetMaxHP(int hp);

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
	void TakeDamage(int damage);

	/**
	 * @brief 死亡しているか
	 */
	bool IsDead() const
	{
		return m_HP <= 0;
	}

	//====================
	// Attack
	//====================

	void SetAttackPower(int power);

	int GetAttackPower() const
	{
		return m_AttackPower;
	}

	//====================
	// Agility
	//====================

	/**
	 * @brief 敏捷値を設定
	 * @details
	 * 現在は1PlayerTurn中に移動できるマス数として使用する。
	 */
	void SetAgility(int agility);

	int GetAgility() const
	{
		return m_Agility;
	}

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "CharacterStatusComponent";
	}

private:

	int m_HP =
		10;

	int m_MaxHP =
		10;

	int m_AttackPower =
		1;

	/**
	 * @brief 敏捷値
	 * 現段階では1PlayerTurn中の移動可能マス数
	 */
	int m_Agility =
		1;
};
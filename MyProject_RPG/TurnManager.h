#pragma once

/**
 * @file TurnManager.h
 * @brief ゲーム全体のTurn Phaseを管理する
 */

#include <cstdint>

 /**
  * @brief 現在のTurn Phase
  *
  * @details
  * TurnManagerは、
  * 「誰が現在行動できるPhaseか」
  * のみを管理する。
  *
  * Player個人のAPや行動状態は管理しない。
  */
enum class TurnPhase : uint8_t
{
	Player,

	Enemy
};

/**
 * @brief Turn進行管理
 *
 * @details
 * TurnManagerの責務は以下だけ。
 *
 * ・現在Phaseの保持
 * ・Player -> Enemyへの遷移
 * ・Enemy -> Playerへの遷移
 * ・Turn番号の管理
 *
 * 以下はTurnManagerでは管理しない。
 *
 * ・PlayerのAP
 * ・Playerの移動力
 * ・Playerが行動済みか
 * ・Skill Cost
 * ・Enemy個別の行動状態
 *
 * これらは各Component / Scene側で管理する。
 */
class TurnManager
{
public:

	TurnManager() = default;

	~TurnManager() = default;

	//=================================================
	// Lifecycle
	//=================================================

	/**
	 * @brief TurnManager初期化
	 */
	void Initialize();

	/**
	 * @brief TurnManager更新
	 *
	 * @details
	 * 現段階では自動進行処理は持たない。
	 *
	 * Scene側が必要なタイミングで
	 * EndPlayerTurn / EndEnemyTurnを呼ぶ。
	 */
	void Update();

	//=================================================
	// Phase Transition
	//=================================================

	/**
	 * @brief Player Phaseを終了してEnemy Phaseへ移行
	 *
	 * @details
	 * Player全員のAction終了判定などは
	 * FieldScene側で行う。
	 */
	void EndPlayerTurn();

	/**
	 * @brief Enemy Phaseを終了してPlayer Phaseへ移行
	 *
	 * @details
	 * Enemy Phase終了時にTurnCountを1増加する。
	 */
	void EndEnemyTurn();

	//=================================================
	// Phase
	//=================================================

	bool IsPlayerTurn() const
	{
		return
			m_Phase ==
			TurnPhase::Player;
	}

	bool IsEnemyTurn() const
	{
		return
			m_Phase ==
			TurnPhase::Enemy;
	}

	TurnPhase GetPhase() const
	{
		return m_Phase;
	}

	//=================================================
	// Turn Count
	//=================================================

	/**
	 * @brief 現在のTurn番号
	 *
	 * @details
	 * Player -> Enemy -> Player
	 *
	 * という1サイクルが終了した時点で
	 * 1増加する。
	 *
	 * 初期PlayerTurnは0。
	 */
	uint64_t GetTurnCount() const
	{
		return m_TurnCount;
	}

private:

	//=================================================
	// Current Phase
	//=================================================

	TurnPhase m_Phase =
		TurnPhase::Player;

	//=================================================
	// Turn Count
	//=================================================

	uint64_t m_TurnCount =
		0;
};
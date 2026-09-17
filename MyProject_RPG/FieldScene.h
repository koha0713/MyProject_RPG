#pragma once
#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"
#include "SoundHandle.h"
#include "GridMap.h"
#include "TurnManager.h"
#include "Quest.h"
#include <vector>

class GameObject;


class CameraComponent;

class FieldScene : public IScene
{
public:
	//====================
	// ライフサイクル
	//====================
	explicit FieldScene();
	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw(uint64_t delta) override;

	//====================
	// デバッグ用関数(予定)
	//====================


	//====================
	// メンバ変数
	//====================
	GameObjectManager m_gameObjectManager;
	TurnManager m_TurnManager;
	CameraComponent* m_MainCamera = nullptr;
	SoundHandle m_BGMHandle;
	GridMap m_GridMap;
	Quest m_CurrentQuest;

private:

	//====================
	// Enemy Turn Sequence
	//====================

	/**
	 * @brief EnemyTurn開始処理
	 */
	void BeginEnemyTurnSequence();

	/**
	 * @brief EnemyTurn進行処理
	 */
	void UpdateEnemyTurnSequence();

	/**
	 * @brief EnemyTurn終了処理
	 */
	void EndEnemyTurnSequence();

	/**
	 * @brief 今回のEnemyTurnで行動するEnemy一覧
	 *
	 * GameObjectManagerが所有しているため、
	 * ここでは非所有ポインタとして保持する。
	 */
	std::vector<GameObject*>
		m_EnemyTurnOrder;

	/**
	 * @brief 現在行動中EnemyのIndex
	 */
	size_t m_CurrentEnemyIndex =
		0;

	/**
	 * @brief 現在のEnemyがAct()を開始済みか
	 */
	bool m_EnemyActionStarted =
		false;

	/**
	 * @brief EnemyTurnシーケンス実行中か
	 */
	bool m_EnemyTurnSequenceActive =
		false;
};
// Scene登録
REGISTER_CLASS(FieldScene)
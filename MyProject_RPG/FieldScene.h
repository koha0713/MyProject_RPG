#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include <vector>

#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"
#include "SoundHandle.h"
#include "GridMap.h"
#include "TurnManager.h"
#include "Quest.h"
#include "TerrainMapData.h"

class GameObject;
struct CharacterSpawnData;
class CameraComponent;
class FieldSceneUI;

class FieldScene :
	public IScene
{
public:

	//=================================================
	// Lifecycle
	//=================================================

	explicit FieldScene();

	~FieldScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw(
		uint64_t delta) override;

private:

	//=================================================
	// Update
	//=================================================

	void UpdatePlayerInput();

	void CheckPlayerTurnEnd();

	void UpdateNextTurnButton();

	bool CheckNextTurnButtonClick();

	//=================================================
	// Enemy Turn Sequence
	//=================================================

	void BeginEnemyTurnSequence();

	void UpdateEnemyTurnSequence();

	void EndEnemyTurnSequence();

	//=================================================
	// Character Create
	//=================================================

	GameObject* CreatePlayer(
		const CharacterSpawnData& data);

	GameObject* CreateEnemy(
		const CharacterSpawnData& data);

	void CreateTerrainTile(
		const TerrainCellData& terrainData);

	//=================================================
	// Turn
	//=================================================

	/**
	 * @brief 全Playerの実行中Actionが終了しているか
	 */
	bool AreAllPlayerActionsFinished() const;

	/**
	 * @brief 全PlayerのAPが0か
	 */
	bool AreAllPlayersOutOfAP() const;

private:

	//=================================================
	// Core
	//=================================================

	GameObjectManager
		m_gameObjectManager;

	TurnManager
		m_TurnManager;

	CameraComponent*
		m_MainCamera =
		nullptr;

	SoundHandle
		m_BGMHandle;

	GridMap
		m_GridMap;

	Quest
		m_CurrentQuest;

	//=================================================
	// UI
	//=================================================

	std::unique_ptr<FieldSceneUI>
		m_FieldUI;

	//=================================================
	// Selected Player
	//=================================================

	/**
	 * @brief 現在操作するPlayer
	 *
	 * 移動・攻撃開始時に解除する。
	 */
	GameObject*
		m_SelectedPlayer =
		nullptr;

	/**
	 * @brief HUDへ表示するPlayer
	 *
	 * 行動開始後も保持することで、
	 * 行動済み状態をHUDで確認できる。
	 */
	GameObject*
		m_FocusedPlayer =
		nullptr;

	//=================================================
	// Enemy Turn
	//=================================================

	std::vector<GameObject*>
		m_EnemyTurnOrder;

	size_t
		m_CurrentEnemyIndex =
		0;

	bool
		m_EnemyActionStarted =
		false;

	bool
		m_EnemyTurnSequenceActive =
		false;

	//=================================================
	// Terrain
	//=================================================

	TerrainMapData
		m_TerrainMapData;

	std::vector<GameObject*>
		m_TerrainObjects;
};

// Scene登録
REGISTER_CLASS(FieldScene)
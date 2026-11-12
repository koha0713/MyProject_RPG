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
#include "RadialMenuUI.h"
#include "GridPattern.h"
#include "PlayerActionDefinition.h"
#include "TerrainRendererComponent.h"
#include "GrassRendererComponent.h"

/**
 * @brief Player操作状態
 */
enum class PlayerInputMode
{
	/**
	 * @brief 通常状態
	 */
	None,

	/**
	 * @brief RadialMenu操作中
	 */
	RadialMenu,

	/**
	 * @brief Action対象選択中
	 */
	TargetSelection,

	/**
	 * @brief Action実行中
	 */
	Executing
};

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

	//=================================================
	// Radial Input
	//=================================================

	/**
	 * @brief Player上で左Mouseが押された
	 */
	void BeginRadialPress(
		float mouseX,
		float mouseY);

	/**
	 * @brief Radial操作更新
	 */
	void UpdateRadialInput(
		uint64_t delta,
		float mouseX,
		float mouseY,
		bool leftHeld,
		bool leftReleased);

	/**
	 * @brief Radial Menuを実際に開く
	 */
	void OpenRadialMenu();

	/**
	 * @brief Radial操作を終了
	 */
	void CloseRadialMenu();

	/**
	 * @brief Radialから選択されたActionを確定
	 */
	void ConfirmRadialAction();

	/**
	 * @brief Mouse方向からActionを求める
	 *
	 * @return
	 * DeadZone外ならtrue。
	 */
	bool CalculateRadialAction(
		float deltaX,
		float deltaY,
		PlayerActionType& outAction) const;

	/**
	 * @brief Action選択後のInputModeへ遷移
	 */
	void BeginActionSelection(
		PlayerActionType action);

	/**
	 * @brief Target選択をキャンセル
	 */
	void CancelPlayerActionSelection();

	/**
	 * @brief Action選択後のTarget入力処理
	 *
	 * @details
	 * TargetingModeに応じて、
	 *
	 * ・Grid
	 * ・Unit
	 * ・Direction
	 *
	 * の入力を処理する。
	 */
	void UpdateTargetSelection();

	/**
	 * @brief Mouse位置からGridを取得
	 *
	 * @param outGrid 取得したGrid
	 *
	 * @return
	 * Grid上をPickできた場合true
	 */
	bool TryGetMouseGrid(
		GridPosition& outGrid) const;

	/**
	 * @brief PlayerからTargetへの方向を4方向へ変換
	 */
	bool CalculateGridDirection(
		const GridPosition& origin,
		const GridPosition& target,
		GridDirection& outDirection) const;

	/**
	 * @brief Actionに選択可能なTargetが存在するか確認
	 *
	 * @details
	 * Moveは移動可能範囲、
	 * Unit / Grid ActionはTargetingSystemを使用して判定する。
	 *
	 * Direction / Selfはこの関数では制限しない。
	 */
	bool HasSelectableTarget(
		PlayerActionType action,
		const PlayerActionDefinition& definition) const;

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

	RadialMenuUI
		m_RadialMenuUI;

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

	TerrainRendererComponent*
		m_TerrainRenderer =
		nullptr;

	GrassRendererComponent*
		m_GrassRenderer =
		nullptr;

	//=================================================
	// Player Input
	//=================================================

	PlayerInputMode
		m_PlayerInputMode =
		PlayerInputMode::None;

	/**
	 * @brief Radial Menuから選択したAction
	 */
	PlayerActionType
		m_SelectedAction =
		PlayerActionType::Move;

	//=================================================
	// Radial Menu
	//=================================================

	/**
	 * @brief Player上で左Mouseを押しているか
	 */
	bool
		m_RadialPressActive =
		false;

	/**
	 * @brief 長押し経過時間 [sec]
	 */
	float
		m_RadialHoldTime =
		0.0f;

	/**
	 * @brief Radial Menuを開くまでの時間
	 */
	float
		m_RadialHoldThreshold =
		0.18f;

	/**
	 * @brief Radial Menu中心
	 */
	float
		m_RadialOriginX =
		0.0f;

	float
		m_RadialOriginY =
		0.0f;

	/**
	 * @brief Action未選択とする中心半径
	 */
	float
		m_RadialDeadZone =
		30.0f;

	/**
	 * @brief 現在Action候補が存在するか
	 */
	bool
		m_HasRadialAction =
		false;

	PlayerActionType
		m_RadialAction =
		PlayerActionType::Move;
};

// Scene登録
REGISTER_CLASS(FieldScene)
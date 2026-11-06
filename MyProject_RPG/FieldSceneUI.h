#pragma once

/**
 * @file FieldSceneUI.h
 * @brief FieldScene用UI管理
 */

class GameObject;
class GameObjectManager;

/**
 * @brief FieldScene UI
 *
 * @details
 * ・次のターンButton
 * ・選択Player HUD
 *
 * を管理する。
 *
 * Turn判定・Player操作などのGameplay処理は
 * FieldScene側が担当する。
 */
class FieldSceneUI
{
public:

	FieldSceneUI() = default;
	~FieldSceneUI() = default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize(
		GameObjectManager& manager);

	void Finalize();

	/**
	 * @brief UI状態更新
	 *
	 * @param focusedPlayer
	 * HUDへ表示するPlayer。
	 * nullptrならHUD非表示。
	 */
	void Update(
		GameObject* focusedPlayer);

	//=================================================
	// Next Turn
	//=================================================

	/**
	 * @brief NextTurn Button表示状態
	 */
	void SetNextTurnVisible(
		bool visible);

	/**
	 * @brief NextTurn Button Click判定
	 */
	bool IsNextTurnClicked() const;

private:

	//=================================================
	// Create
	//=================================================

	void CreateNextTurnUI();

	void CreatePlayerHUD();

	GameObject* CreateImage(
		const char* name);

	GameObject* CreateText(
		const char* name);

	//=================================================
	// Next Turn
	//=================================================

	void UpdateNextTurnVisual();

	//=================================================
	// Player HUD
	//=================================================

	void UpdatePlayerHUD(
		GameObject* player);

	void HidePlayerHUD();

	/**
	 * @brief HUD全体を明るく/暗くする
	 */
	void SetPlayerHUDDarkened(
		bool darkened);

	//=================================================
	// Utility
	//=================================================

	void HideObject(
		GameObject* object);

private:

	GameObjectManager*
		m_Manager =
		nullptr;

	//=================================================
	// Next Turn
	//=================================================

	GameObject*
		m_NextTurnButton =
		nullptr;

	GameObject*
		m_NextTurnText =
		nullptr;

	bool
		m_NextTurnVisible =
		false;

	//=================================================
	// Selected Player HUD
	//=================================================

	GameObject*
		m_PlayerHUDBackground =
		nullptr;

	GameObject*
		m_PlayerIcon =
		nullptr;

	GameObject*
		m_PlayerNameText =
		nullptr;

	GameObject*
		m_HPBarBackground =
		nullptr;

	GameObject*
		m_HPBarFill =
		nullptr;

	GameObject*
		m_HPText =
		nullptr;

	GameObject*
		m_MPBarBackground =
		nullptr;

	GameObject*
		m_MPBarFill =
		nullptr;

	GameObject*
		m_MPText =
		nullptr;
};
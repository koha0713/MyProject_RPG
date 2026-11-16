#pragma once

/**
 * @file FieldSceneUI.h
 * @brief FieldScene用UI管理
 */

#include <vector>

class GameObject;
class GameObjectManager;

/**
 * @brief Player一覧の1要素
 */
struct FieldPlayerIconUI
{
	/**
	 * @brief 対応するPlayer
	 *
	 * GameObjectManager所有。
	 * FieldSceneUIは非所有参照。
	 */
	GameObject*
		Player =
		nullptr;

	/**
	 * @brief Click可能なIcon
	 */
	GameObject*
		Button =
		nullptr;
};

/**
 * @brief FieldScene UI
 *
 * @details
 * ・次のターンButton
 * ・Player一覧
 * ・選択Player HUD
 *
 * Gameplay処理・Camera制御は
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
	 * 詳細HUDへ表示するPlayer。
	 * nullptrならHUD非表示。
	 */
	void Update(
		GameObject* focusedPlayer);

	//=================================================
	// Player List
	//=================================================

	/**
	 * @brief Fieldに存在する自Player一覧を設定
	 *
	 * @details
	 * Player生成完了後に1度呼ぶ。
	 */
	void SetPlayers(
		const std::vector<GameObject*>& players);

	/**
	 * @brief このFrameでIcon ClickされたPlayerを取得
	 *
	 * @return
	 * ClickされたPlayer。
	 * Clickされていなければnullptr。
	 */
	GameObject*
		GetClickedPlayer() const
	{
		return m_ClickedPlayer;
	}

	//=================================================
	// Next Turn
	//=================================================

	void SetNextTurnVisible(
		bool visible);

	bool IsNextTurnClicked() const;

private:

	//=================================================
	// Create
	//=================================================

	void CreateNextTurnUI();

	void CreatePlayerHUD();

	/**
	 * @brief Player一覧UIを生成
	 */
	void CreatePlayerIcons();

	GameObject* CreateImage(
		const char* name);

	GameObject* CreateText(
		const char* name);

	//=================================================
	// Next Turn
	//=================================================

	void UpdateNextTurnVisual();

	//=================================================
	// Player List
	//=================================================

	/**
	 * @brief Player Icon Button状態更新
	 */
	void UpdatePlayerIcons(
		GameObject* focusedPlayer);

	/**
	 * @brief 生成済みPlayer Iconを破棄扱いにする
	 */
	void ClearPlayerIcons();

	//=================================================
	// Player HUD
	//=================================================

	void UpdatePlayerHUD(
		GameObject* player);

	void HidePlayerHUD();

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
	// Player List
	//=================================================

	std::vector<FieldPlayerIconUI>
		m_PlayerIcons;

	/**
	 * @brief このFrameでClickされたPlayer
	 */
	GameObject*
		m_ClickedPlayer =
		nullptr;

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
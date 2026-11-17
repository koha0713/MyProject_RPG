#pragma once

#include <vector>

class GameObject;
class GameObjectManager;

/**
 * @brief Player一覧の1要素
 */
struct FieldPlayerIconUI
{
	GameObject*
		Player =
		nullptr;

	GameObject*
		Button =
		nullptr;
};

class FieldSceneUI
{
public:

	FieldSceneUI() = default;
	~FieldSceneUI() = default;

	void Initialize(
		GameObjectManager& manager);

	void Finalize();

	void Update(
		GameObject* focusedPlayer);

	void SetPlayers(
		const std::vector<GameObject*>& players);

	GameObject*
		GetClickedPlayer() const;

	void SetNextTurnVisible(
		bool visible);

	bool IsNextTurnClicked() const;

private:

	//=================================================
	// Create
	//=================================================

	void CreateNextTurnUI();

	void CreatePlayerHUD();

	void CreatePlayerIcons(const std::vector<GameObject*>& players);

	/**
	 * @brief AP表示用Iconを生成
	 */
	void CreateAPIcons();

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

	void UpdatePlayerIcons(
		GameObject* focusedPlayer);

	void ClearPlayerIcons();

	//=================================================
	// Player HUD
	//=================================================

	void UpdatePlayerHUD(
		GameObject* player);

	/**
	 * @brief AP表示更新
	 */
	void UpdateAPDisplay(
		int currentAP);

	void HidePlayerHUD();

	void SetPlayerHUDDarkened(
		bool darkened);

	//=================================================
	// Utility
	//=================================================

	void HideObject(
		GameObject* object);

	//=================================================
	// AP Preview
	//=================================================

	/**
	 * @brief AP消費予定数を設定
	 *
	 * @param cost
	 * ラジアルメニューで現在選択中のActionが
	 * 消費するAP。
	 *
	 * 0ならPreviewなし。
	 */
	void SetAPPreviewCost(
		int cost)
	{
		// 負数は許可しない。
		m_APPreviewCost =
			(cost > 0)
			? cost
			: 0;
	}

	/**
	 * @brief AP消費Previewを解除
	 */
	void ClearAPPreview()
	{
		m_APPreviewCost =
			0;
	}

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

	GameObject*
		m_ClickedPlayer =
		nullptr;

	/**
	 * @brief 詳細HUDのY座標
	 *
	 * 選択中Player IconのY座標へ追従する。
	 */
	float
		m_PlayerHUDY =
		500.0f;

	//=================================================
	// Selected Player HUD
	//=================================================

	GameObject*
		m_PlayerHUDBackground =
		nullptr;

	GameObject*
		m_PlayerNameText =
		nullptr;

	GameObject*
		m_PlayerLevelText =
		nullptr;

	//=================================================
	// HP
	//=================================================

	/**
	 * @brief HP下地
	 */
	GameObject*
		m_HPBarBackground =
		nullptr;

	/**
	 * @brief HP実量
	 */
	GameObject*
		m_HPBarFill =
		nullptr;

	/**
	 * @brief HP装飾枠
	 *
	 * Background → Fill → Frame
	 * の順で描画する。
	 */
	GameObject*
		m_HPBarFrame =
		nullptr;

	//=================================================
	// MP
	//=================================================

	GameObject*
		m_MPBarBackground =
		nullptr;

	GameObject*
		m_MPBarFill =
		nullptr;

	GameObject*
		m_MPBarFrame =
		nullptr;

	//=================================================
	// AP
	//=================================================

	GameObject*
		m_APText =
		nullptr;

	/**
	 * @brief AP羽Icon
	 */
	std::vector<GameObject*>
		m_APIcons;

	/**
	 * @brief 現在Preview中のAP消費量
	 *
	 * ラジアルメニュー非選択時は0。
	 */
	int
		m_APPreviewCost =
		0;
};
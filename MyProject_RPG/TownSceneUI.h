#pragma once

/**
 * @file TownSceneUI.h
 * @brief TownScene専用UIを管理する
 */

#include "TownSceneState.h"

class GameObject;
class GameObjectManager;

/**
 * @brief TownScene専用UI管理クラス
 *
 * @details
 * TownSceneで使用する
 *
 * - Background
 * - UIButton
 * - Texture
 * - UI配置
 * - UI表示切替
 * - ButtonのHover / Pressed表現
 *
 * を管理する。
 *
 * 施設をクリックした結果、
 * どのScene / Stateへ移動するかという
 * Gameplay処理はTownScene側で管理する。
 */
class TownSceneUI
{
public:

	TownSceneUI() = default;
	~TownSceneUI() = default;

	//=================================================
	// Lifecycle
	//=================================================

	/**
	 * @brief Town用UIを生成する
	 *
	 * @param manager
	 * UI GameObjectを所有するGameObjectManager
	 */
	void Initialize(
		GameObjectManager& manager);

	/**
	 * @brief UI参照を破棄する
	 *
	 * @note
	 * GameObject自体の所有権は
	 * GameObjectManager側にある。
	 */
	void Finalize();

	//=================================================
	// State
	//=================================================

	/**
	 * @brief 指定State用のUI表示へ切り替える
	 */
	void ApplyState(
		TownSceneState state);

	/**
	 * @brief 現在Stateで表示しているButtonの
	 *        Visualを更新する
	 */
	void UpdateVisuals(
		TownSceneState state);

	//=================================================
	// Overview Input
	//=================================================

	bool IsGuildClicked() const;

	bool IsInnClicked() const;

	bool IsTavernClicked() const;

	bool IsMarketClicked() const;

	bool IsGateClicked() const;

	bool IsCastleClicked() const;

	//=================================================
	// Guild Interior Input
	//=================================================

	bool IsReceptionClicked() const;

	bool IsShopClicked() const;

	bool IsQuestBoardClicked() const;

	bool IsGuildBackClicked() const;

	//=================================================
	// Reception Input
	//=================================================

	bool IsReceptionBackClicked() const;

	//=================================================
	// Shop Input
	//=================================================

	bool IsShopBackClicked() const;

	//=================================================
	// QuestBoard Input
	//=================================================

	bool IsQuestBoardBackClicked() const;

	//=================================================
	// QuestDetail Input
	//=================================================

	bool IsQuestSignClicked() const;

	bool IsQuestDetailBackClicked() const;

	//=================================================
	// Inn Input
	//=================================================

	bool IsInnBackClicked() const;

	//=================================================
	// Tavern Input
	//=================================================

	bool IsTavernBackClicked() const;

	//=================================================
	// Market Input
	//=================================================

	bool IsMarketBackClicked() const;

	//=================================================
	// Castle Input
	//=================================================

	bool IsCastleBackClicked() const;

private:

	//=================================================
	// Create
	//=================================================

	void CreateBackground();

	void CreateOverviewUI();

	void CreateGuildInteriorUI();

	void CreateReceptionUI();

	void CreateShopUI();

	void CreateQuestBoardUI();

	void CreateQuestDetailUI();

	void CreateInnUI();

	void CreateTavernUI();

	void CreateMarketUI();

	void CreateCastleUI();

	/**
	 * @brief UIButton用GameObject生成
	 */
	GameObject* CreateButton(
		const char* name);

	//=================================================
	// Apply
	//=================================================

	void ApplyOverview();

	void ApplyGuildInterior();

	void ApplyReception();

	void ApplyShop();

	void ApplyQuestBoard();

	void ApplyQuestDetail();

	void ApplyInn();

	void ApplyTavern();

	void ApplyMarket();

	void ApplyCastle();

	//=================================================
	// Utility
	//=================================================

	/**
	 * @brief Background Texture変更
	 */
	void SetBackgroundTexture(
		const char* texturePath);

	/**
	 * @brief ButtonのTexture / Rect設定
	 */
	void ConfigureButton(
		GameObject* buttonObject,
		const char* texturePath,
		float x,
		float y,
		float width,
		float height);

	/**
	 * @brief Buttonを非表示にする
	 */
	void HideButton(
		GameObject* buttonObject);

	/**
	 * @brief 全Buttonを非表示にする
	 */
	void HideAllButtons();

	/**
	 * @brief Hover / Pressed表示更新
	 */
	void UpdateButtonVisual(
		GameObject* buttonObject);

	/**
	 * @brief ButtonのClick状態取得
	 */
	bool IsButtonClicked(
		GameObject* buttonObject) const;

private:

	/**
	 * @brief UI GameObject所有Manager
	 *
	 * TownSceneが所有しているため非所有。
	 */
	GameObjectManager*
		m_Manager =
		nullptr;

	//=================================================
	// Background
	//=================================================

	GameObject*
		m_Background =
		nullptr;

	//=================================================
	// Overview
	//=================================================

	GameObject*
		m_GuildButton =
		nullptr;

	GameObject*
		m_InnButton =
		nullptr;

	GameObject*
		m_TavernButton =
		nullptr;

	GameObject*
		m_MarketButton =
		nullptr;

	GameObject*
		m_GateButton =
		nullptr;

	GameObject*
		m_CastleButton =
		nullptr;

	//=================================================
	// Guild Interior
	//=================================================

	GameObject*
		m_ReceptionButton =
		nullptr;

	GameObject*
		m_ShopButton =
		nullptr;

	GameObject*
		m_QuestBoardButton =
		nullptr;

	GameObject*
		m_GuildBackButton =
		nullptr;

	//=================================================
	// Reception
	//=================================================

	GameObject*
		m_ReceptionBackButton =
		nullptr;

	//=================================================
	// Shop
	//=================================================

	GameObject*
		m_ShopBackButton =
		nullptr;

	//=================================================
	// QuestBoard
	//=================================================

	GameObject*
		m_QuestBoardBackButton =
		nullptr;

	//=================================================
	// QuestDetail
	//=================================================

	GameObject*
		m_QuestSignButton =
		nullptr;

	GameObject*
		m_QuestDetailBackButton =
		nullptr;

	//=================================================
// Inn
//=================================================

	GameObject*
		m_InnBackButton =
		nullptr;

	//=================================================
	// Tavern
	//=================================================

	GameObject*
		m_TavernBackButton =
		nullptr;

	//=================================================
	// Market
	//=================================================

	GameObject*
		m_MarketBackButton =
		nullptr;

	//=================================================
	// Castle
	//=================================================

	GameObject*
		m_CastleBackButton =
		nullptr;
};
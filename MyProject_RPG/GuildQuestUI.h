#pragma once

/**
 * @file GuildQuestUI.h
 * @brief Guild内のQuest関連UI管理
 */

#include <array>
#include <cstdint>
#include <string>
#include <chrono>

#include "TownSceneState.h"

class GameObject;
class GameObjectManager;
class Quest;

/**
 * @brief GuildのQuest関連UI
 *
 * @details
 * 以下を管理する。
 *
 * ・QuestBoardの依頼書
 * ・ReceptionのQuest一覧
 * ・QuestDetailの文字情報
 * ・Stamp演出
 * ・ReportButton
 *
 * QuestそのもののAccept / Reportは
 * TownSceneが担当する。
 */
class GuildQuestUI
{
public:

	GuildQuestUI() = default;
	~GuildQuestUI() = default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize(
		GameObjectManager& manager);

	void Finalize();

	/**
	 * @brief State変更通知
	 */
	void ApplyState(
		TownSceneState state,
		Quest* selectedQuest);

	/**
	 * @brief UI更新
	 */
	void Update(
		uint64_t delta,
		TownSceneState state);

	//=================================================
	// QuestBoard
	//=================================================

	void RefreshQuestBoard();

	/**
	 * @brief QuestBoardでクリックされたQuest取得
	 */
	Quest* GetClickedBoardQuest() const;

	//=================================================
	// Reception
	//=================================================

	/**
	 * @brief Receptionに表示するQuest一覧を更新する
	 */
	void RefreshReception();

	/**
	 * @brief ReceptionでクリックされたQuestを取得
	 *
	 * @return
	 * クリックされていなければnullptr
	 */
	Quest* GetClickedReceptionQuest() const;

	/**
	 * @brief 報告Buttonがクリックされたか
	 */
	bool IsReportClicked() const;

	/**
	 * @brief 現在選択しているQuestを設定する
	 */
	void SetSelectedQuest(
		Quest* quest);

	//=================================================
	// Accept Effect
	//=================================================

	/**
	 * @brief Stamp演出開始
	 */
	void StartAcceptEffect();

	/**
	 * @brief Stamp演出完了を1回だけ取得
	 */
	bool ConsumeAcceptEffectFinished();

private:

	static constexpr size_t MaxQuestCount =
		8;

	/**
	 * @brief Quest1件分のUI
	 */
	struct QuestEntryUI
	{
		Quest* QuestData =
			nullptr;

		GameObject* Button =
			nullptr;

		GameObject* NameText =
			nullptr;
	};

	//=================================================
	// Create
	//=================================================

	void CreateQuestBoardUI();

	void CreateReceptionUI();

	void CreateQuestDetailUI();

	GameObject* CreateButton(
		const char* name);

	GameObject* CreateText(
		const char* name);

	GameObject* CreateImage(
		const char* name);

	//=================================================
	// Display
	//=================================================

	void ShowQuestBoard();

	void ShowReception();

	void ShowQuestDetail();

	void HideAll();

	void HideObject(
		GameObject* object);

	//=================================================
	// Utility
	//=================================================

	void ConfigureButton(
		GameObject* object,
		const char* texturePath,
		float x,
		float y,
		float width,
		float height);

	void ConfigureText(
		GameObject* object,
		const std::wstring& text,
		float x,
		float y,
		float width,
		float height,
		float fontSize);

	void ConfigureImage(
		GameObject* object,
		const char* texturePath,
		float x,
		float y,
		float width,
		float height);

	void UpdateButtonVisual(
		GameObject* object);

	/**
	 * @brief Reception一覧Buttonの見た目更新
	 *
	 * 選択中Questは通常のHover色とは別の色で表示する。
	 */
	void UpdateReceptionEntryVisual(
		const QuestEntryUI& entry);

	bool IsClicked(
		GameObject* object) const;

	void UpdateDetailText();


private:

	GameObjectManager*
		m_Manager =
		nullptr;

	Quest*
		m_SelectedQuest =
		nullptr;

	//=================================================
	// QuestBoard
	//=================================================

	std::array<
		QuestEntryUI,
		MaxQuestCount>
		m_BoardEntries{};

	//=================================================
	// Reception
	//=================================================

	std::array<
		QuestEntryUI,
		MaxQuestCount>
		m_ReceptionEntries{};

	/**
	 * @brief Questが1件もない場合の表示
	 */
	GameObject*
		m_ReceptionEmptyText =
		nullptr;

	GameObject*
		m_ReceptionDetailName =
		nullptr;

	GameObject*
		m_ReceptionDetailState =
		nullptr;

	GameObject*
		m_ReceptionDetailProgress =
		nullptr;

	GameObject*
		m_ReportButton =
		nullptr;

	//=================================================
	// QuestDetail
	//=================================================

	GameObject*
		m_DetailName =
		nullptr;

	GameObject*
		m_DetailState =
		nullptr;

	GameObject*
		m_DetailProgress =
		nullptr;

	GameObject*
		m_StampImage =
		nullptr;

	//=================================================
	// Stamp Effect
	//=================================================

	bool
		m_IsAcceptEffectPlaying =
		false;

	bool
		m_AcceptEffectFinished =
		false;

	/**
	 * @brief Stamp演出開始時刻
	 *
	 * steady_clockを使用することで、
	 * ゲーム側deltaの単位に依存しない。
	 */
	std::chrono::steady_clock::time_point
		m_AcceptEffectStartTime;

	/**
	 * @brief Stamp表示時間
	 *
	 * 0.8秒表示する。
	 */
	static constexpr float
		AcceptEffectDurationSeconds =
		0.8f;
};
#pragma once

/**
 * @file TownScene.h
 * @brief Town Scene
 */

#include <cstdint>
#include <memory>

#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"
#include "TownSceneState.h"

class Quest;
class TownSceneUI;
class GuildQuestUI;

/**
 * @brief Town Scene
 *
 * @details
 * Scene遷移・施設遷移・QuestのGameplay処理を担当する。
 *
 * 見た目の構築はTownSceneUI / GuildQuestUIへ委譲する。
 */
class TownScene :
	public IScene
{
public:

	TownScene() = default;

	~TownScene() override;

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw(
		uint64_t delta) override;

private:

	//=================================================
	// State
	//=================================================

	void ChangeState(
		TownSceneState state);

	//=================================================
	// Update
	//=================================================

	void UpdateTownOverview();

	void UpdateGuildInterior();

	void UpdateReception();

	void UpdateShop();

	void UpdateQuestBoard();

	void UpdateQuestDetail(
		uint64_t delta);

	void UpdateInn();

	void UpdateTavern();

	void UpdateMarket();

	void UpdateCastle();

	//=================================================
	// Quest
	//=================================================

	void AcceptSelectedQuest();

	bool ReportSelectedQuest();

private:

	GameObjectManager
		m_GameObjectManager;

	std::unique_ptr<TownSceneUI>
		m_UI;

	std::unique_ptr<GuildQuestUI>
		m_GuildQuestUI;

	TownSceneState
		m_CurrentState =
		TownSceneState::Overview;

	/**
	 * @brief 現在選択しているQuest
	 *
	 * QuestManager所有なので非所有。
	 */
	Quest*
		m_SelectedQuest =
		nullptr;

	/**
	 * @brief Stamp演出中に
	 *        Acceptを多重実行しないためのFlag
	 */
	bool
		m_IsQuestAcceptPending =
		false;
};

REGISTER_CLASS(TownScene)
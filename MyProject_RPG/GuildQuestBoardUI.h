#pragma once

/**
 * @file GuildQuestBoardUI.h
 * @brief GuildのQuestBoard専用UI管理
 */

#include <vector>

class GameObject;
class GameObjectManager;
class Quest;

/**
 * @brief QuestBoard上に表示するQuestPaper
 */
struct QuestPaperUI
{
	/**
	 * @brief 対応するQuest
	 *
	 * QuestManager所有なので非所有。
	 */
	Quest* QuestData =
		nullptr;

	/**
	 * @brief 紙本体
	 *
	 * GameObjectManager所有なので非所有。
	 */
	GameObject* PaperObject =
		nullptr;
};

/**
 * @brief Guild QuestBoard UI
 *
 * @details
 * 未受注QuestをQuestPaperとして表示し、
 * クリックされたQuestをTownSceneへ通知する。
 *
 * Questそのものの受注処理は行わない。
 */
class GuildQuestBoardUI
{
public:

	GuildQuestBoardUI() = default;
	~GuildQuestBoardUI() = default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize(
		GameObjectManager& manager);

	void Finalize();

	//=================================================
	// Quest Paper
	//=================================================

	/**
	 * @brief 現在受注可能なQuestから
	 *        QuestPaperを再構築する
	 */
	void Refresh();

	/**
	 * @brief QuestPaperのHover表示を更新
	 */
	void UpdateVisuals();

	/**
	 * @brief 今フレームでクリックされたQuestを取得
	 *
	 * @return
	 * Questがクリックされていなければnullptr
	 */
	Quest* GetClickedQuest() const;

private:

	/**
	 * @brief QuestPaperを生成する
	 */
	GameObject* CreateQuestPaper(
		const char* name);

	/**
	 * @brief QuestPaper配置
	 */
	void ConfigureQuestPaper(
		GameObject* object,
		float x,
		float y,
		float width,
		float height);

	/**
	 * @brief Paper Hover表示
	 */
	void UpdatePaperVisual(
		GameObject* object);

private:

	GameObjectManager*
		m_Manager =
		nullptr;

	std::vector<QuestPaperUI>
		m_QuestPapers;
};
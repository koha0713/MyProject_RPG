#pragma once

#include <vector>

#include "Quest.h"
#include "NonCopyable.h"

/**
 * @brief 複数Questを管理するManager
 *
 * @details
 * Sceneを跨いで保持するゲーム進行データなので、
 * SingletonとしてGame全体から参照する。
 */
class QuestManager :
	private NonCopyable
{
public:

	static QuestManager& GetInstance()
	{
		static QuestManager instance;

		return instance;
	}

	//====================
	// Lifecycle
	//====================

	void Initialize()
	{
		m_Quests.clear();
	}

	void Finalize()
	{
		m_Quests.clear();
	}

	//====================
	// Quest
	//====================

	Quest& AddQuest(
		const Quest& quest)
	{
		m_Quests.push_back(
			quest);

		return m_Quests.back();
	}

	void NotifyEnemyKilled();

	std::vector<Quest*>
		GetReportableQuests();

	/**
	 * @brief 現在受注可能なQuestを取得する
	 *
	 * @details
	 * Inactive状態のQuestのみを返す。
	 *
	 * @return 受注可能Quest一覧
	 */
	std::vector<Quest*>
		GetAcceptableQuests();

	const std::vector<Quest>&
		GetQuests() const
	{
		return m_Quests;
	}



private:

	QuestManager() = default;
	~QuestManager() = default;

private:

	std::vector<Quest>
		m_Quests;
};

/**
 * @brief QuestManager簡易アクセス
 */
#define QUEST_MANAGER \
	QuestManager::GetInstance()
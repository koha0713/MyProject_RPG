#include "QuestManager.h"

void QuestManager::NotifyEnemyKilled()
{
	for (Quest& quest :
		m_Quests)
	{
		if (!quest.IsActive())
		{
			continue;
		}

		if (quest.GetType() !=
			QuestType::KillEnemy)
		{
			continue;
		}

		quest.AddProgress();
	}
}

std::vector<Quest*>
QuestManager::GetReportableQuests()
{
	std::vector<Quest*>
		result;

	for (Quest& quest :
		m_Quests)
	{
		if (quest.IsReadyToReport())
		{
			result.push_back(
				&quest);
		}
	}

	return result;
}
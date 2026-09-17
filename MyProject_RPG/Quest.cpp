#include "Quest.h"

#include <algorithm>

void Quest::Setup(
	const std::string& name,
	QuestType type,
	int requiredCount)
{
	m_Name =
		name;

	m_Type =
		type;

	m_RequiredCount =
		std::max(
			requiredCount,
			1);

	m_CurrentCount =
		0;

	m_State =
		QuestState::Inactive;
}

void Quest::Accept()
{
	if (m_State !=
		QuestState::Inactive)
	{
		return;
	}

	m_CurrentCount =
		0;

	m_State =
		QuestState::Active;
}

void Quest::AddProgress(
	int amount)
{
	if (m_State !=
		QuestState::Active)
	{
		return;
	}

	if (amount <= 0)
	{
		return;
	}

	m_CurrentCount +=
		amount;

	if (m_CurrentCount >=
		m_RequiredCount)
	{
		m_CurrentCount =
			m_RequiredCount;

		// ğŒ’B¬‚µ‚Ä‚à‘¦Completed‚É‚Í‚µ‚È‚¢B
		// Guild‚Å•ñ‚·‚é‚Ü‚Å‚ÍReadyToReportB
		m_State =
			QuestState::ReadyToReport;
	}
}

bool Quest::Report()
{
	if (m_State !=
		QuestState::ReadyToReport)
	{
		return false;
	}

	m_State =
		QuestState::Completed;

	return true;
}
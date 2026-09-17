#pragma once

#include <string>

/**
 * @brief Quest Type
 */
enum class QuestType
{
	KillEnemy
};

/**
 * @brief Quest State
 */
enum class QuestState
{
	Inactive,      // 未受注
	Active,        // 受注中
	ReadyToReport, // 条件達成済み・未報告
	Completed      // ギルド報告済み
};

/**
 * @brief Quest単体の進行状態
 */
class Quest
{
public:

	void Setup(
		const std::string& name,
		QuestType type,
		int requiredCount);

	/**
	 * @brief Quest受注
	 */
	void Accept();

	/**
	 * @brief 進行度を加算
	 */
	void AddProgress(
		int amount = 1);

	/**
	 * @brief ギルドで報告
	 *
	 * @return 報告成功時true
	 */
	bool Report();

	bool IsActive() const
	{
		return
			m_State ==
			QuestState::Active;
	}

	bool IsReadyToReport() const
	{
		return
			m_State ==
			QuestState::ReadyToReport;
	}

	bool IsCompleted() const
	{
		return
			m_State ==
			QuestState::Completed;
	}

	const std::string& GetName() const
	{
		return m_Name;
	}

	QuestType GetType() const
	{
		return m_Type;
	}

	QuestState GetState() const
	{
		return m_State;
	}

	int GetCurrentCount() const
	{
		return m_CurrentCount;
	}

	int GetRequiredCount() const
	{
		return m_RequiredCount;
	}

private:

	std::string m_Name;

	QuestType m_Type =
		QuestType::KillEnemy;

	QuestState m_State =
		QuestState::Inactive;

	int m_CurrentCount =
		0;

	int m_RequiredCount =
		1;
};
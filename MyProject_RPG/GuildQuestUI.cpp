#include "GuildQuestUI.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"
#include "UIFontComponent.h"

#include "QuestManager.h"

#include <array>
#include <cstdint>
#include <string>

#include <Windows.h>

namespace
{
	/**
	 * @brief UTF-8文字列をUTF-16へ変換する
	 *
	 * @param text UTF-8文字列
	 * @return UTF-16文字列
	 */
	std::wstring UTF8ToWide(
		const std::string& text)
	{
		if (text.empty())
		{
			return std::wstring();
		}

		// 必要なUTF-16文字数を取得する。
		const int size =
			MultiByteToWideChar(
				CP_UTF8,
				0,
				text.c_str(),
				static_cast<int>(
					text.size()),
				nullptr,
				0);

		if (size <= 0)
		{
			return std::wstring();
		}

		std::wstring result(
			static_cast<size_t>(size),
			L'\0');

		MultiByteToWideChar(
			CP_UTF8,
			0,
			text.c_str(),
			static_cast<int>(
				text.size()),
			result.data(),
			size);

		return result;
	}

	const wchar_t* GetQuestStateText(
		QuestState state)
	{
		switch (state)
		{
		case QuestState::Inactive:

			return L"Not Accepted";

		case QuestState::Active:

			return L"Active";

		case QuestState::ReadyToReport:

			return L"Ready To Report";

		case QuestState::Completed:

			return L"Completed";

		default:

			return L"Unknown";
		}
	}
}

//=====================================================
// Initialize
//=====================================================

void GuildQuestUI::Initialize(
	GameObjectManager& manager)
{
	m_Manager =
		&manager;

	CreateQuestBoardUI();

	CreateReceptionUI();

	CreateQuestDetailUI();

	HideAll();
}

//=====================================================
// Finalize
//=====================================================

void GuildQuestUI::Finalize()
{
	for (auto& entry :
		m_BoardEntries)
	{
		entry =
			QuestEntryUI{};
	}

	for (auto& entry :
		m_ReceptionEntries)
	{
		entry =
			QuestEntryUI{};
	}

	m_ReceptionDetailName =
		nullptr;

	m_ReceptionDetailState =
		nullptr;

	m_ReceptionDetailProgress =
		nullptr;

	m_ReceptionEmptyText =
		nullptr;

	m_ReportButton =
		nullptr;

	m_DetailName =
		nullptr;

	m_DetailState =
		nullptr;

	m_DetailProgress =
		nullptr;

	m_StampImage =
		nullptr;

	m_SelectedQuest =
		nullptr;

	m_Manager =
		nullptr;
}

//=====================================================
// Create QuestBoard
//=====================================================

void GuildQuestUI::CreateQuestBoardUI()
{
	for (size_t i = 0;
		i < MaxQuestCount;
		++i)
	{
		const std::string buttonName =
			"QuestBoardPaper_" +
			std::to_string(i);

		const std::string textName =
			"QuestBoardText_" +
			std::to_string(i);

		m_BoardEntries[i].Button =
			CreateButton(
				buttonName.c_str());

		m_BoardEntries[i].NameText =
			CreateText(
				textName.c_str());
	}
}

//=====================================================
// Create Reception
//=====================================================

void GuildQuestUI::CreateReceptionUI()
{
	//=================================================
	// Quest List
	//=================================================

	for (size_t i = 0;
		i < MaxQuestCount;
		++i)
	{
		const std::string buttonName =
			"ReceptionQuest_" +
			std::to_string(i);

		const std::string textName =
			"ReceptionQuestText_" +
			std::to_string(i);

		m_ReceptionEntries[i].Button =
			CreateButton(
				buttonName.c_str());

		m_ReceptionEntries[i].NameText =
			CreateText(
				textName.c_str());
	}

	//=================================================
	// Empty Message
	//=================================================

	m_ReceptionEmptyText =
		CreateText(
			"ReceptionEmptyText");

	//=================================================
	// Selected Quest Detail
	//=================================================

	m_ReceptionDetailName =
		CreateText(
			"ReceptionDetailName");

	m_ReceptionDetailState =
		CreateText(
			"ReceptionDetailState");

	m_ReceptionDetailProgress =
		CreateText(
			"ReceptionDetailProgress");

	//=================================================
	// Report Button
	//=================================================

	m_ReportButton =
		CreateButton(
			"QuestReportButton");
}

//=====================================================
// Create QuestDetail
//=====================================================

void GuildQuestUI::CreateQuestDetailUI()
{
	m_DetailName =
		CreateText(
			"QuestDetailName");

	m_DetailState =
		CreateText(
			"QuestDetailState");

	m_DetailProgress =
		CreateText(
			"QuestDetailProgress");

	m_StampImage =
		CreateImage(
			"QuestStamp");
}

//=====================================================
// Create Button
//=====================================================

GameObject*
GuildQuestUI::CreateButton(
	const char* name)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* object =
		m_Manager->Create<GameObject>();

	if (!object)
	{
		return nullptr;
	}

	object->SetName(
		name);

	object->SetTag(
		Tag::UI);

	object->AddComponent<
		UIRectComponent>();

	object->AddComponent<
		UIImageComponent>();

	object->AddComponent<
		UIButtonComponent>();

	return object;
}

//=====================================================
// Create Text
//=====================================================

GameObject*
GuildQuestUI::CreateText(
	const char* name)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* object =
		m_Manager->Create<GameObject>();

	if (!object)
	{
		return nullptr;
	}

	object->SetName(
		name);

	// FontRenderer用Tag。
	object->SetTag(
		Tag::Font);

	object->AddComponent<
		UIRectComponent>();

	object->AddComponent<
		UIFontComponent>();

	return object;
}

//=====================================================
// Create Image
//=====================================================

GameObject*
GuildQuestUI::CreateImage(
	const char* name)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* object =
		m_Manager->Create<GameObject>();

	if (!object)
	{
		return nullptr;
	}

	object->SetName(
		name);

	object->SetTag(
		Tag::UI);

	object->AddComponent<
		UIRectComponent>();

	object->AddComponent<
		UIImageComponent>();

	return object;
}

//=====================================================
// Apply State
//=====================================================

void GuildQuestUI::ApplyState(
	TownSceneState state,
	Quest* selectedQuest)
{
	HideAll();

	m_SelectedQuest =
		selectedQuest;

	m_IsAcceptEffectPlaying =
		false;

	m_AcceptEffectFinished =
		false;

	switch (state)
	{
	case TownSceneState::QuestBoard:

		RefreshQuestBoard();

		ShowQuestBoard();

		break;

	case TownSceneState::QuestDetail:

		ShowQuestDetail();

		break;

	case TownSceneState::Reception:

		RefreshReception();

		ShowReception();

		break;

	default:
		break;
	}
}

//=====================================================
// Update
//=====================================================

void GuildQuestUI::Update(
	uint64_t delta,
	TownSceneState state)
{
	switch (state)
	{
	case TownSceneState::QuestBoard:

		for (auto& entry :
			m_BoardEntries)
		{
			UpdateButtonVisual(
				entry.Button);
		}

		break;

	case TownSceneState::Reception:

		for (const auto& entry :
			m_ReceptionEntries)
		{
			UpdateReceptionEntryVisual(
				entry);
		}

		UpdateButtonVisual(
			m_ReportButton);

		break;

	case TownSceneState::QuestDetail:

		if (m_IsAcceptEffectPlaying)
		{
			//=============================================
			// 実時間取得
			//=============================================

			const auto currentTime =
				std::chrono::steady_clock::now();

			const std::chrono::duration<float>
				elapsed =
				currentTime -
				m_AcceptEffectStartTime;

			//=============================================
			// Stamp演出終了
			//=============================================

			if (elapsed.count() >=
				AcceptEffectDurationSeconds)
			{
				m_IsAcceptEffectPlaying =
					false;

				m_AcceptEffectFinished =
					true;
			}
		}

		break;
	default:
		break;
	}
}

//=====================================================
// Refresh QuestBoard
//=====================================================

void GuildQuestUI::RefreshQuestBoard()
{
	for (auto& entry :
		m_BoardEntries)
	{
		entry.QuestData =
			nullptr;

		HideObject(
			entry.Button);

		HideObject(
			entry.NameText);
	}

	const auto quests =
		QUEST_MANAGER.
		GetAcceptableQuests();


	const size_t displayCount =
		(quests.size() < MaxQuestCount)
		? quests.size()
		: MaxQuestCount;

	constexpr float startX =
		180.0f;

	constexpr float startY =
		120.0f;

	constexpr float width =
		240.0f;

	constexpr float height =
		300.0f;

	constexpr float spacingX =
		65.0f;

	constexpr float spacingY =
		30.0f;

	constexpr int columns =
		3;

	for (size_t i = 0;
		i < displayCount;
		++i)
	{
		Quest* quest =
			quests[i];

		if (!quest)
		{
			continue;
		}

		const int column =
			static_cast<int>(i) %
			columns;

		const int row =
			static_cast<int>(i) /
			columns;

		const float x =
			startX +
			static_cast<float>(column) *
			(width + spacingX);

		const float y =
			startY +
			static_cast<float>(row) *
			(height + spacingY);

		auto& entry =
			m_BoardEntries[i];

		entry.QuestData =
			quest;

		ConfigureButton(
			entry.Button,
			"Assets/Textures/Town/QuestPaper.png",
			x,
			y,
			width,
			height);

		ConfigureText(
			entry.NameText,
			UTF8ToWide(
				quest->GetName()),
			x + 25.0f,
			y + 45.0f,
			width - 50.0f,
			60.0f,
			24.0f);
	}
}

//=====================================================
// Refresh Reception
//=====================================================

void GuildQuestUI::RefreshReception()
{
	//=================================================
	// Reset
	//=================================================

	for (auto& entry :
		m_ReceptionEntries)
	{
		entry.QuestData =
			nullptr;

		HideObject(
			entry.Button);

		HideObject(
			entry.NameText);
	}

	HideObject(
		m_ReceptionEmptyText);

	//=================================================
	// Quest取得
	//=================================================

	const auto& quests =
		QUEST_MANAGER.GetQuests();

	size_t displayIndex =
		0;

	//=================================================
	// Active / ReadyToReportのみ表示
	//=================================================

	for (const Quest& questRef :
		quests)
	{
		if (displayIndex >=
			MaxQuestCount)
		{
			break;
		}

		const QuestState state =
			questRef.GetState();

		if (state !=
			QuestState::Active &&
			state !=
			QuestState::ReadyToReport)
		{
			continue;
		}

		// QuestManager所有なので非所有。
		Quest* quest =
			const_cast<Quest*>(
				&questRef);

		auto& entry =
			m_ReceptionEntries[
				displayIndex];

		entry.QuestData =
			quest;

		//=============================================
		// Layout
		//=============================================

		const float y =
			110.0f +
			static_cast<float>(
				displayIndex) *
			68.0f;

		ConfigureButton(
			entry.Button,
			"Assets/Textures/Town/QuestPaper.png",
			650.0f,
			y,
			500.0f,
			58.0f);

		//=============================================
		// 一覧表示文字列
		//=============================================

		std::wstring listText =
			UTF8ToWide(
				quest->GetName());

		listText +=
			L"    ";

		// 報告可能Questは一覧でも分かるようにする。
		if (state ==
			QuestState::ReadyToReport)
		{
			listText +=
				L"[Ready To Report]";
		}
		else
		{
			listText +=
				L"[";

			listText +=
				std::to_wstring(
					quest->
					GetCurrentCount());

			listText +=
				L" / ";

			listText +=
				std::to_wstring(
					quest->
					GetRequiredCount());

			listText +=
				L"]";
		}

		ConfigureText(
			entry.NameText,
			listText,
			675.0f,
			y + 10.0f,
			450.0f,
			38.0f,
			20.0f);

		++displayIndex;
	}

	//=================================================
	// Questなし
	//=================================================

	if (displayIndex == 0)
	{
		// 選択中Questも無効化。
		m_SelectedQuest =
			nullptr;

		ConfigureText(
			m_ReceptionEmptyText,
			L"No active quests.",
			690.0f,
			210.0f,
			400.0f,
			50.0f,
			26.0f);
	}

	//=================================================
	// SelectedQuestが一覧に存在するか確認
	//=================================================

	if (m_SelectedQuest)
	{
		bool found =
			false;

		for (size_t i = 0;
			i < displayIndex;
			++i)
		{
			if (m_ReceptionEntries[i].
				QuestData ==
				m_SelectedQuest)
			{
				found =
					true;

				break;
			}
		}

		// Report後などで一覧から消えた場合。
		if (!found)
		{
			m_SelectedQuest =
				nullptr;
		}
	}

	//=================================================
	// Detail更新
	//=================================================

	UpdateDetailText();
}

//=====================================================
// Board Quest Click
//=====================================================

Quest*
GuildQuestUI::GetClickedBoardQuest() const
{
	for (const auto& entry :
		m_BoardEntries)
	{
		if (!entry.QuestData)
		{
			continue;
		}

		if (IsClicked(
			entry.Button))
		{
			return entry.QuestData;
		}
	}

	return nullptr;
}

//=====================================================
// Reception Quest Click
//=====================================================

Quest*
GuildQuestUI::GetClickedReceptionQuest() const
{
	for (const auto& entry :
		m_ReceptionEntries)
	{
		if (!entry.QuestData)
		{
			continue;
		}

		if (IsClicked(
			entry.Button))
		{
			return entry.QuestData;
		}
	}

	return nullptr;
}

//=====================================================
// Report Click
//=====================================================

bool GuildQuestUI::IsReportClicked() const
{
	return IsClicked(
		m_ReportButton);
}

//=====================================================
// Set Selected Quest
//=====================================================

void GuildQuestUI::SetSelectedQuest(
	Quest* quest)
{
	m_SelectedQuest =
		quest;

	UpdateDetailText();
}

//=====================================================
// Update Detail Text
//=====================================================

void GuildQuestUI::UpdateDetailText()
{
	//=================================================
	// Reset
	//=================================================

	HideObject(
		m_ReceptionDetailName);

	HideObject(
		m_ReceptionDetailState);

	HideObject(
		m_ReceptionDetailProgress);

	HideObject(
		m_ReportButton);

	//=================================================
	// 未選択
	//=================================================

	if (!m_SelectedQuest)
	{
		ConfigureText(
			m_ReceptionDetailName,
			L"Select a quest.",
			100.0f,
			180.0f,
			480.0f,
			50.0f,
			28.0f);

		return;
	}

	//=================================================
	// Quest Name
	//=================================================

	ConfigureText(
		m_ReceptionDetailName,
		UTF8ToWide(
			m_SelectedQuest->
			GetName()),
		100.0f,
		170.0f,
		480.0f,
		60.0f,
		30.0f);

	//=================================================
	// State
	//=================================================

	std::wstring stateText;

	if (m_SelectedQuest->GetState() ==
		QuestState::ReadyToReport)
	{
		stateText =
			L"State : Ready To Report";
	}
	else
	{
		stateText =
			L"State : Active";
	}

	ConfigureText(
		m_ReceptionDetailState,
		stateText,
		100.0f,
		250.0f,
		480.0f,
		45.0f,
		22.0f);

	//=================================================
	// Progress
	//=================================================

	const std::wstring progressText =
		std::wstring(
			L"Progress : ") +
		std::to_wstring(
			m_SelectedQuest->
			GetCurrentCount()) +
		L" / " +
		std::to_wstring(
			m_SelectedQuest->
			GetRequiredCount());

	ConfigureText(
		m_ReceptionDetailProgress,
		progressText,
		100.0f,
		310.0f,
		480.0f,
		45.0f,
		22.0f);

	//=================================================
	// Report
	//=================================================

	// ReadyToReportのみ報告可能。
	if (m_SelectedQuest->GetState() ==
		QuestState::ReadyToReport)
	{
		ConfigureButton(
			m_ReportButton,
			"Assets/Textures/Town/ReportButton.png",
			170.0f,
			410.0f,
			300.0f,
			80.0f);
	}
}

//=====================================================
// Show Board
//=====================================================

void GuildQuestUI::ShowQuestBoard()
{
	// RefreshQuestBoard()ですでに
	// 使用Entryだけ表示されているため追加処理なし。
}

//=====================================================
// Show Reception
//=====================================================

void GuildQuestUI::ShowReception()
{
	// RefreshReception()で表示済み。
}

//=====================================================
// Show QuestDetail
//=====================================================
void GuildQuestUI::ShowQuestDetail()
{
	if (!m_SelectedQuest)
	{
		return;
	}

	//=================================================
	// Quest Name
	//=================================================

	ConfigureText(
		m_DetailName,
		UTF8ToWide(
			m_SelectedQuest->GetName()),
		350.0f,
		150.0f,
		580.0f,
		70.0f,
		36.0f);

	//=================================================
	// State
	//=================================================

	const std::wstring stateText =
		std::wstring(L"State : ") +
		GetQuestStateText(
			m_SelectedQuest->GetState());

	ConfigureText(
		m_DetailState,
		stateText,
		350.0f,
		260.0f,
		580.0f,
		50.0f,
		24.0f);

	//=================================================
	// Progress
	//=================================================

	const std::wstring progressText =
		std::wstring(L"Target : ") +
		std::to_wstring(
			m_SelectedQuest->
			GetCurrentCount()) +
		L" / " +
		std::to_wstring(
			m_SelectedQuest->
			GetRequiredCount());

	ConfigureText(
		m_DetailProgress,
		progressText,
		350.0f,
		320.0f,
		580.0f,
		50.0f,
		24.0f);

	// Stampは署名前なので隠す。
	HideObject(
		m_StampImage);
}

//=====================================================
// Start Accept Effect
//=====================================================

void GuildQuestUI::StartAcceptEffect()
{
	if (!m_SelectedQuest)
	{
		return;
	}

	// 多重開始防止
	if (m_IsAcceptEffectPlaying)
	{
		return;
	}

	m_IsAcceptEffectPlaying =
		true;

	m_AcceptEffectFinished =
		false;

	//=================================================
	// 開始時刻を保存
	//=================================================

	m_AcceptEffectStartTime =
		std::chrono::steady_clock::now();

	//=================================================
	// Stamp表示
	//=================================================

	ConfigureImage(
		m_StampImage,
		"Assets/Textures/Town/Stamp.png",
		790.0f,
		430.0f,
		190.0f,
		190.0f);
}

//=====================================================
// Consume Effect Finished
//=====================================================

bool GuildQuestUI::
ConsumeAcceptEffectFinished()
{
	if (!m_AcceptEffectFinished)
	{
		return false;
	}

	m_AcceptEffectFinished =
		false;

	return true;
}

//=====================================================
// Hide All
//=====================================================

void GuildQuestUI::HideAll()
{
	for (auto& entry :
		m_BoardEntries)
	{
		HideObject(
			entry.Button);

		HideObject(
			entry.NameText);
	}

	for (auto& entry :
		m_ReceptionEntries)
	{
		HideObject(
			entry.Button);

		HideObject(
			entry.NameText);
	}

	HideObject(
		m_ReceptionEmptyText);

	HideObject(
		m_ReceptionDetailName);

	HideObject(
		m_ReceptionDetailState);

	HideObject(
		m_ReceptionDetailProgress);

	HideObject(
		m_ReportButton);

	HideObject(
		m_DetailName);

	HideObject(
		m_DetailState);

	HideObject(
		m_DetailProgress);

	HideObject(
		m_StampImage);
}

//=====================================================
// Hide Object
//=====================================================

void GuildQuestUI::HideObject(
	GameObject* object)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	if (!rect)
	{
		return;
	}

	rect->SetSize(
		0.0f,
		0.0f);
}

//=====================================================
// Configure Button
//=====================================================

void GuildQuestUI::ConfigureButton(
	GameObject* object,
	const char* texturePath,
	float x,
	float y,
	float width,
	float height)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	auto* image =
		object->GetComponent<
		UIImageComponent>();

	if (rect)
	{
		rect->SetPosition(
			x,
			y);

		rect->SetSize(
			width,
			height);
	}

	if (image)
	{
		image->SetTexture(
			texturePath);

		image->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}
}

//=====================================================
// Configure Text
//=====================================================
void GuildQuestUI::ConfigureText(
	GameObject* object,
	const std::wstring& text,
	float x,
	float y,
	float width,
	float height,
	float fontSize)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	auto* font =
		object->GetComponent<
		UIFontComponent>();

	if (rect)
	{
		rect->SetPosition(
			x,
			y);

		rect->SetSize(
			width,
			height);
	}

	if (font)
	{
		font->SetText(
			text);

		font->SetFontSize(
			fontSize);

		font->SetColor(
			Color(
				0.15f,
				0.10f,
				0.07f,
				1.0f));
	}
}

//=====================================================
// Configure Image
//=====================================================

void GuildQuestUI::ConfigureImage(
	GameObject* object,
	const char* texturePath,
	float x,
	float y,
	float width,
	float height)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	auto* image =
		object->GetComponent<
		UIImageComponent>();

	if (rect)
	{
		rect->SetPosition(
			x,
			y);

		rect->SetSize(
			width,
			height);
	}

	if (image)
	{
		image->SetTexture(
			texturePath);

		image->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}
}

//=====================================================
// Button Visual
//=====================================================

void GuildQuestUI::UpdateButtonVisual(
	GameObject* object)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	auto* image =
		object->GetComponent<
		UIImageComponent>();

	auto* button =
		object->GetComponent<
		UIButtonComponent>();

	if (!rect ||
		!image ||
		!button)
	{
		return;
	}

	if (rect->GetSize().x <= 0.0f ||
		rect->GetSize().y <= 0.0f)
	{
		return;
	}

	switch (button->GetState())
	{
	case UIButtonState::Normal:

		image->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));

		break;

	case UIButtonState::Hovered:

		image->SetColor(
			Color(
				1.0f,
				0.94f,
				0.80f,
				1.0f));

		break;

	case UIButtonState::Pressed:

		image->SetColor(
			Color(
				0.75f,
				0.75f,
				0.75f,
				1.0f));

		break;

	default:
		break;
	}
}

//=====================================================
// Reception Entry Visual
//=====================================================

void GuildQuestUI::
UpdateReceptionEntryVisual(
	const QuestEntryUI& entry)
{
	if (!entry.Button)
	{
		return;
	}

	// まず通常のButton表現を適用。
	UpdateButtonVisual(
		entry.Button);

	// 選択されていない場合は終了。
	if (!entry.QuestData ||
		entry.QuestData !=
		m_SelectedQuest)
	{
		return;
	}

	auto* rect =
		entry.Button->
		GetComponent<
		UIRectComponent>();

	auto* image =
		entry.Button->
		GetComponent<
		UIImageComponent>();

	if (!rect ||
		!image)
	{
		return;
	}

	// 非表示中なら処理しない。
	if (rect->GetSize().x <= 0.0f ||
		rect->GetSize().y <= 0.0f)
	{
		return;
	}

	// 選択中Questを少し黄色寄りにする。
	image->SetColor(
		Color(
			1.0f,
			0.85f,
			0.55f,
			1.0f));
}

//=====================================================
// Is Clicked
//=====================================================

bool GuildQuestUI::IsClicked(
	GameObject* object) const
{
	if (!object)
	{
		return false;
	}

	auto* button =
		object->GetComponent<
		UIButtonComponent>();

	if (!button)
	{
		return false;
	}

	return button->IsClicked();
}
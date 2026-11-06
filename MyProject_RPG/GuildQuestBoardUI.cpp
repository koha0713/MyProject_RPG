/**
 * @file GuildQuestBoardUI.cpp
 */

#include "GuildQuestBoardUI.h"

#include "GameObjectManager.h"
#include "GameObject.h"

#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"

#include "QuestManager.h"

#include <string>

 //=====================================================
 // Initialize
 //=====================================================

void GuildQuestBoardUI::Initialize(
	GameObjectManager& manager)
{
	m_Manager =
		&manager;
}

//=====================================================
// Finalize
//=====================================================

void GuildQuestBoardUI::Finalize()
{
	// GameObject自体はGameObjectManager所有なので、
	// ここではdeleteしない。
	m_QuestPapers.clear();

	m_Manager =
		nullptr;
}

//=====================================================
// Refresh
//=====================================================

void GuildQuestBoardUI::Refresh()
{
	if (!m_Manager)
	{
		return;
	}

	//=================================================
	// 既存Paperを非表示にする
	//=================================================
	//
	// GameObjectManagerがDestroy APIを持つ場合は、
	// 後でDestroy方式へ変更してもよい。
	//
	// 現段階では再利用せず、
	// 古いPaperを0サイズ化する。
	//=================================================

	for (auto& paper :
		m_QuestPapers)
	{
		if (!paper.PaperObject)
		{
			continue;
		}

		auto* rect =
			paper.PaperObject->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetSize(
				0.0f,
				0.0f);
		}
	}

	m_QuestPapers.clear();

	//=================================================
	// 受注可能Quest取得
	//=================================================

	const std::vector<Quest*>
		quests =
		QUEST_MANAGER.GetAcceptableQuests();

	if (quests.empty())
	{
		return;
	}

	//=================================================
	// Layout
	//=================================================

	constexpr float startX =
		190.0f;

	constexpr float startY =
		130.0f;

	constexpr float paperWidth =
		230.0f;

	constexpr float paperHeight =
		310.0f;

	constexpr float spacingX =
		60.0f;

	constexpr float spacingY =
		40.0f;

	constexpr int columnCount =
		3;

	//=================================================
	// QuestPaper生成
	//=================================================

	for (size_t i = 0;
		i < quests.size();
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
			columnCount;

		const int row =
			static_cast<int>(i) /
			columnCount;

		const float x =
			startX +
			static_cast<float>(column) *
			(
				paperWidth +
				spacingX
				);

		const float y =
			startY +
			static_cast<float>(row) *
			(
				paperHeight +
				spacingY
				);

		//=============================================
		// GameObject Name
		//=============================================

		const std::string objectName =
			"QuestPaper_" +
			std::to_string(i);

		GameObject* paperObject =
			CreateQuestPaper(
				objectName.c_str());

		if (!paperObject)
		{
			continue;
		}

		ConfigureQuestPaper(
			paperObject,
			x,
			y,
			paperWidth,
			paperHeight);

		QuestPaperUI paperUI{};

		paperUI.QuestData =
			quest;

		paperUI.PaperObject =
			paperObject;

		m_QuestPapers.emplace_back(
			paperUI);
	}
}

//=====================================================
// Create QuestPaper
//=====================================================

GameObject*
GuildQuestBoardUI::CreateQuestPaper(
	const char* name)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* object =
		m_Manager->Create<
		GameObject>();

	if (!object)
	{
		return nullptr;
	}

	object->SetName(
		name);

	object->SetTag(
		Tag::UI);

	//=================================================
	// Rect
	//=================================================

	object->AddComponent<
		UIRectComponent>();

	//=================================================
	// Image
	//=================================================

	auto* image =
		object->AddComponent<
		UIImageComponent>();

	if (image)
	{
		image->SetTexture(
			"Assets/Textures/Town/QuestPaper.png");
	}

	//=================================================
	// Button
	//=================================================

	object->AddComponent<
		UIButtonComponent>();

	return object;
}

//=====================================================
// Configure QuestPaper
//=====================================================

void GuildQuestBoardUI::ConfigureQuestPaper(
	GameObject* object,
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

	if (!rect)
	{
		return;
	}

	rect->SetPosition(
		x,
		y);

	rect->SetSize(
		width,
		height);
}

//=====================================================
// Update Visuals
//=====================================================

void GuildQuestBoardUI::UpdateVisuals()
{
	for (auto& paper :
		m_QuestPapers)
	{
		UpdatePaperVisual(
			paper.PaperObject);
	}
}

//=====================================================
// Update Paper Visual
//=====================================================

void GuildQuestBoardUI::UpdatePaperVisual(
	GameObject* object)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	auto* button =
		object->GetComponent<
		UIButtonComponent>();

	auto* image =
		object->GetComponent<
		UIImageComponent>();

	if (!rect ||
		!button ||
		!image)
	{
		return;
	}

	if (rect->GetSize().x <=
		0.0f ||
		rect->GetSize().y <=
		0.0f)
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

		// 少し明るくして
		// 選択可能であることを示す。
		image->SetColor(
			Color(
				1.0f,
				0.95f,
				0.85f,
				1.0f));

		break;

	case UIButtonState::Pressed:

		image->SetColor(
			Color(
				0.8f,
				0.8f,
				0.8f,
				1.0f));

		break;

	default:
		break;
	}
}

//=====================================================
// Get Clicked Quest
//=====================================================

Quest*
GuildQuestBoardUI::GetClickedQuest() const
{
	for (const auto& paper :
		m_QuestPapers)
	{
		if (!paper.QuestData ||
			!paper.PaperObject)
		{
			continue;
		}

		auto* button =
			paper.PaperObject->
			GetComponent<
			UIButtonComponent>();

		if (!button)
		{
			continue;
		}

		if (button->IsClicked())
		{
			return paper.QuestData;
		}
	}

	return nullptr;
}
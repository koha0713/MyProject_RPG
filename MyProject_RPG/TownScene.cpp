#include "TownScene.h"

// Component
#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"

#include "GameObject.h"
#include "QuestManager.h"
#include "SceneManager.h"
#include "SpriteRenderer.h"
#include "Window.h"

#include "DebugUI.h"

namespace
{
	const char* GetQuestStateName(
		QuestState state)
	{
		switch (state)
		{
		case QuestState::Inactive:
			return "Not Accepted";

		case QuestState::Active:
			return "Active";

		case QuestState::ReadyToReport:
			return "Ready To Report";

		case QuestState::Completed:
			return "Completed";

		default:
			return "Unknown";
		}
	}
}

//=====================================================
// Initialize
//=====================================================

void TownScene::Initialize()
{
	//=================================================
	// Background
	//=================================================

	CreateBackground();

	//=================================================
	// Buttons
	//=================================================

	CreateButtons();

	//=================================================
	// GameObject Initialize
	//=================================================

	m_GameObjectManager.Initialize();

	//=================================================
	// 初期位置
	//=================================================

	m_CurrentLocation =
		TownLocation::Overview;

	RefreshLocationUI();
}

//=====================================================
// Finalize
//=====================================================

void TownScene::Finalize()
{
	m_Background =
		nullptr;

	m_PrimaryButton =
		nullptr;

	m_SecondaryButton =
		nullptr;

	m_BackButton =
		nullptr;

	m_GameObjectManager.Clear();
}

//=====================================================
// Update
//=====================================================

void TownScene::Update(
	uint64_t delta)
{
	//=================================================
	// Component Update
	//=================================================

	m_GameObjectManager.Update(
		delta);

	//=================================================
	// Button Visual
	//=================================================

	UpdateButtonVisual(
		m_PrimaryButton);

	UpdateButtonVisual(
		m_SecondaryButton);

	UpdateButtonVisual(
		m_BackButton);

	//=================================================
	// Location別処理
	//=================================================

	switch (m_CurrentLocation)
	{
	case TownLocation::Overview:

		UpdateTownOverview();

		break;

	case TownLocation::Guild:

		UpdateGuild();

		break;

	default:
		break;
	}
}

//=====================================================
// Draw
//=====================================================

void TownScene::Draw(
	uint64_t delta)
{
	(void)delta;

	// Townは完全2D表示なので
	// Camera設定は不要。
	SpriteRenderer::Begin();

	m_GameObjectManager.Draw();

	SpriteRenderer::End();

	//=================================================
	// (仮)Guild Quest List
	//=================================================
	if (m_CurrentLocation ==
		TownLocation::Guild)
	{
		DebugUI::RegisterDebugFunction(
			[this]()
			{
				DrawGuildQuestListUI();
			});
	}
}

//=====================================================
// Create Background
//=====================================================

void TownScene::CreateBackground()
{
	m_Background =
		m_GameObjectManager.Create<
		GameObject>();

	if (!m_Background)
	{
		return;
	}

	m_Background->SetName(
		"TownBackground");

	//=================================================
	// Rect
	//=================================================

	auto* rect =
		m_Background->AddComponent<
		UIRectComponent>();

	if (rect)
	{
		rect->SetPosition(
			0.0f,
			0.0f);

		rect->SetSize(
			static_cast<float>(
				Window::GetWidth()),
			static_cast<float>(
				Window::GetHeight()));
	}

	//=================================================
	// Image
	//=================================================

	auto* image =
		m_Background->AddComponent<
		UIImageComponent>();

	if (image)
	{
		image->SetTexture(
			"Assets/Textures/Town/TownBackground.png");
	}
}

//=====================================================
// Create Buttons
//=====================================================

void TownScene::CreateButtons()
{
	//=================================================
	// Primary
	//=================================================

	m_PrimaryButton =
		m_GameObjectManager.Create<
		GameObject>();

	if (m_PrimaryButton)
	{
		m_PrimaryButton->SetName(
			"TownPrimaryButton");

		m_PrimaryButton->
			AddComponent<
			UIRectComponent>();

		m_PrimaryButton->
			AddComponent<
			UIImageComponent>();

		m_PrimaryButton->
			AddComponent<
			UIButtonComponent>();
	}

	//=================================================
	// Secondary
	//=================================================

	m_SecondaryButton =
		m_GameObjectManager.Create<
		GameObject>();

	if (m_SecondaryButton)
	{
		m_SecondaryButton->SetName(
			"TownSecondaryButton");

		m_SecondaryButton->
			AddComponent<
			UIRectComponent>();

		m_SecondaryButton->
			AddComponent<
			UIImageComponent>();

		m_SecondaryButton->
			AddComponent<
			UIButtonComponent>();
	}

	//=================================================
	// Back
	//=================================================

	m_BackButton =
		m_GameObjectManager.Create<
		GameObject>();

	if (m_BackButton)
	{
		m_BackButton->SetName(
			"TownBackButton");

		m_BackButton->
			AddComponent<
			UIRectComponent>();

		m_BackButton->
			AddComponent<
			UIImageComponent>();

		m_BackButton->
			AddComponent<
			UIButtonComponent>();
	}
}

//=====================================================
// Change Location
//=====================================================

void TownScene::ChangeLocation(
	TownLocation location)
{
	if (m_CurrentLocation ==
		location)
	{
		return;
	}

	m_CurrentLocation =
		location;

	RefreshLocationUI();
}

//=====================================================
// Refresh UI
//=====================================================

void TownScene::RefreshLocationUI()
{
	const float screenWidth =
		static_cast<float>(
			Window::GetWidth());

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	//=================================================
	// Town Overview
	//=================================================

	if (m_CurrentLocation ==
		TownLocation::Overview)
	{
		//=============================================
		// Background
		//=============================================

		if (m_Background)
		{
			auto* image =
				m_Background->GetComponent<
				UIImageComponent>();

			if (image)
			{
				image->SetTexture(
					"Assets/Textures/Town/TownBackground.png");
			}
		}

		const float buttonWidth =
			400.0f;

		const float buttonHeight =
			100.0f;

		const float x =
			(
				screenWidth -
				buttonWidth
				) *
			0.5f;

		//=============================================
		// Guild
		//=============================================

		ConfigureButton(
			m_PrimaryButton,
			"Assets/Textures/Town/GuildButton.png",
			x,
			screenHeight * 0.62f,
			buttonWidth,
			buttonHeight);

		//=============================================
		// Town Exit
		//=============================================

		ConfigureButton(
			m_SecondaryButton,
			"Assets/Textures/Town/TownExitButton.png",
			x,
			screenHeight * 0.78f,
			buttonWidth,
			buttonHeight);

		// OverviewではBack不要
		HideButton(
			m_BackButton);

		return;
	}

	//=================================================
	// Guild
	//=================================================

	if (m_CurrentLocation ==
		TownLocation::Guild)
	{
		//=============================================
		// Guild Background
		//=============================================

		if (m_Background)
		{
			auto* image =
				m_Background->GetComponent<
				UIImageComponent>();

			if (image)
			{
				image->SetTexture(
					"Assets/Textures/Town/GuildBackground.png");
			}
		}

		const float buttonWidth =
			400.0f;

		const float buttonHeight =
			100.0f;

		const float x =
			(
				screenWidth -
				buttonWidth
				) *
			0.5f;

		//=============================================
		// Quest Accept
		//=============================================

		ConfigureButton(
			m_PrimaryButton,
			"Assets/Textures/Town/QuestAcceptButton.png",
			x,
			screenHeight * 0.58f,
			buttonWidth,
			buttonHeight);

		//=============================================
		// Quest Report
		//=============================================

		ConfigureButton(
			m_SecondaryButton,
			"Assets/Textures/Town/QuestReportButton.png",
			x,
			screenHeight * 0.72f,
			buttonWidth,
			buttonHeight);

		//=============================================
		// Back
		//=============================================

		ConfigureButton(
			m_BackButton,
			"Assets/Textures/Town/BackButton.png",
			40.0f,
			screenHeight - 110.0f,
			240.0f,
			70.0f);
	}
}

//=====================================================
// Configure Button
//=====================================================

void TownScene::ConfigureButton(
	GameObject* buttonObject,
	const char* texturePath,
	float x,
	float y,
	float width,
	float height)
{
	if (!buttonObject)
	{
		return;
	}

	auto* rect =
		buttonObject->GetComponent<
		UIRectComponent>();

	auto* image =
		buttonObject->GetComponent<
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

		// 状態切替直後はNormal色へ戻す
		image->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}
}

//=====================================================
// Hide Button
//=====================================================

void TownScene::HideButton(
	GameObject* buttonObject)
{
	if (!buttonObject)
	{
		return;
	}

	auto* rect =
		buttonObject->GetComponent<
		UIRectComponent>();

	if (!rect)
	{
		return;
	}

	// 0x0にすることで
	// DrawとMouse Hitの両方を実質無効化する。
	rect->SetSize(
		0.0f,
		0.0f);
}

//=====================================================
// Button Visual
//=====================================================

void TownScene::UpdateButtonVisual(
	GameObject* buttonObject)
{
	if (!buttonObject)
	{
		return;
	}

	auto* rect =
		buttonObject->GetComponent<
		UIRectComponent>();

	auto* button =
		buttonObject->GetComponent<
		UIButtonComponent>();

	auto* image =
		buttonObject->GetComponent<
		UIImageComponent>();

	if (!rect ||
		!button ||
		!image)
	{
		return;
	}

	// 非表示Buttonは処理不要
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
				0.9f,
				0.9f,
				0.9f,
				1.0f));

		break;

	case UIButtonState::Pressed:

		image->SetColor(
			Color(
				0.7f,
				0.7f,
				0.7f,
				1.0f));

		break;

	default:
		break;
	}
}

//=====================================================
// Town Overview
//=====================================================

void TownScene::UpdateTownOverview()
{
	//=================================================
	// Guild
	//=================================================

	if (m_PrimaryButton)
	{
		auto* button =
			m_PrimaryButton->GetComponent<
			UIButtonComponent>();

		if (button &&
			button->IsClicked())
		{
			ChangeLocation(
				TownLocation::Guild);

			return;
		}
	}

	//=================================================
	// Town Exit
	//=================================================

	if (m_SecondaryButton)
	{
		auto* button =
			m_SecondaryButton->GetComponent<
			UIButtonComponent>();

		if (button &&
			button->IsClicked())
		{
			SceneManager::SetCurrentScene(
				"FieldScene");

			return;
		}
	}
}

//=====================================================
// Guild
//=====================================================

void TownScene::UpdateGuild()
{
	//=================================================
	// Quest Accept
	//=================================================

	if (m_PrimaryButton)
	{
		auto* button =
			m_PrimaryButton->GetComponent<
			UIButtonComponent>();

		if (button &&
			button->IsClicked())
		{
			AcceptQuest();

			return;
		}
	}

	//=================================================
	// Quest Report
	//=================================================

	if (m_SecondaryButton)
	{
		auto* button =
			m_SecondaryButton->GetComponent<
			UIButtonComponent>();

		if (button &&
			button->IsClicked())
		{
			ReportQuest();

			return;
		}
	}

	//=================================================
	// Back
	//=================================================

	if (m_BackButton)
	{
		auto* button =
			m_BackButton->GetComponent<
			UIButtonComponent>();

		if (button &&
			button->IsClicked())
		{
			ChangeLocation(
				TownLocation::Overview);

			return;
		}
	}
}

void TownScene::AcceptQuest()
{
	std::vector<Quest*> acceptableQuests =
		QUEST_MANAGER.GetAcceptableQuests();

	//=================================================
	// 受注可能Questなし
	//=================================================

	if (acceptableQuests.empty())
	{
		OutputDebugStringA(
			"[Guild] "
			"No quests available.\n");

		return;
	}

	//=================================================
	// β版
	//
	// 現在は最初の未受注Questを受注する。
	// 将来的にはQuest一覧UIから選択する。
	//=================================================

	Quest* quest =
		acceptableQuests.front();

	if (!quest)
	{
		return;
	}

	quest->Accept();

	OutputDebugStringA(
		"[Guild] Quest Accepted: ");

	OutputDebugStringA(
		quest->GetName().c_str());

	OutputDebugStringA(
		"\n");
}

void TownScene::ReportQuest()
{
	std::vector<Quest*> reportableQuests =
		QUEST_MANAGER.GetReportableQuests();

	//=================================================
	// 報告可能Questなし
	//=================================================

	if (reportableQuests.empty())
	{
		OutputDebugStringA(
			"[Guild] "
			"No quests ready to report.\n");

		return;
	}

	//=================================================
	// β版
	//
	// 現在は最初の報告可能Questを報告する。
	// 将来的にはQuest一覧UIから選択する。
	//=================================================

	Quest* quest =
		reportableQuests.front();

	if (!quest)
	{
		return;
	}

	if (!quest->Report())
	{
		return;
	}

	OutputDebugStringA(
		"[Guild] Quest Reported: ");

	OutputDebugStringA(
		quest->GetName().c_str());

	OutputDebugStringA(
		"\n");
}

void TownScene::DrawGuildQuestListUI()
{
	// Guild以外では表示しない
	if (m_CurrentLocation !=
		TownLocation::Guild)
	{
		return;
	}

	ImGui::SetNextWindowSize(
		ImVec2(
			500.0f,
			400.0f),
		ImGuiCond_FirstUseEver);

	ImGui::Begin(
		"Quest List");

	const auto& quests =
		QUEST_MANAGER.GetQuests();

	if (quests.empty())
	{
		ImGui::Text(
			"No quests available.");

		ImGui::End();

		return;
	}

	//=================================================
	// Quest一覧
	//=================================================

	for (size_t i = 0;
		i < quests.size();
		++i)
	{
		const Quest& quest =
			quests[i];

		ImGui::PushID(
			static_cast<int>(i));

		//=============================================
		// Quest Name
		//=============================================

		ImGui::Text(
			"%s",
			quest.GetName().c_str());

		//=============================================
		// State
		//=============================================

		ImGui::Text(
			"State: %s",
			GetQuestStateName(
				quest.GetState()));

		//=============================================
		// Progress
		//=============================================

		ImGui::Text(
			"Progress: %d / %d",
			quest.GetCurrentCount(),
			quest.GetRequiredCount());

		//=============================================
		// ProgressBar
		//=============================================

		float progress =
			0.0f;

		if (quest.GetRequiredCount() > 0)
		{
			progress =
				static_cast<float>(
					quest.GetCurrentCount()) /
				static_cast<float>(
					quest.GetRequiredCount());

			if (progress > 1.0f)
			{
				progress =
					1.0f;
			}
		}

		ImGui::ProgressBar(
			progress,
			ImVec2(
				-1.0f,
				20.0f));

		ImGui::Separator();

		ImGui::PopID();
	}

	ImGui::End();
}
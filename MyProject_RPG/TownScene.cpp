#include "TownScene.h"

#include "TownSceneUI.h"
#include "GuildQuestUI.h"

#include "QuestManager.h"
#include "SceneManager.h"

#include "SpriteRenderer.h"
#include "FontRenderer.h"

#include "DebugUI.h"

#include <memory>

//=====================================================
// Destructor
//=====================================================

TownScene::~TownScene() =
default;

//=====================================================
// Initialize
//=====================================================

void TownScene::Initialize()
{
	//=================================================
	// Town UI
	//=================================================

	m_UI =
		std::make_unique<
		TownSceneUI>();

	m_UI->Initialize(
		m_GameObjectManager);

	//=================================================
	// Guild Quest UI
	//=================================================

	m_GuildQuestUI =
		std::make_unique<
		GuildQuestUI>();

	m_GuildQuestUI->Initialize(
		m_GameObjectManager);

	//=================================================
	// GameObject
	//=================================================

	m_GameObjectManager.Initialize();

	//=================================================
	// Initial State
	//=================================================

	m_CurrentState =
		TownSceneState::Overview;

	m_SelectedQuest =
		nullptr;

	m_IsQuestAcceptPending =
		false;

	m_UI->ApplyState(
		m_CurrentState);

	m_GuildQuestUI->ApplyState(
		m_CurrentState,
		nullptr);
}

//=====================================================
// Finalize
//=====================================================

void TownScene::Finalize()
{
	m_SelectedQuest =
		nullptr;

	m_IsQuestAcceptPending =
		false;

	if (m_GuildQuestUI)
	{
		m_GuildQuestUI->
			Finalize();

		m_GuildQuestUI.reset();
	}

	if (m_UI)
	{
		m_UI->Finalize();

		m_UI.reset();
	}

	m_GameObjectManager.Clear();
}

//=====================================================
// Update
//=====================================================

void TownScene::Update(
	uint64_t delta)
{
	m_GameObjectManager.Update(
		delta);

	//=================================================
	// UI Visual
	//=================================================

	if (m_UI)
	{
		m_UI->UpdateVisuals(
			m_CurrentState);
	}

	if (m_GuildQuestUI)
	{
		m_GuildQuestUI->Update(
			delta,
			m_CurrentState);
	}

	//=================================================
	// State
	//=================================================

	switch (m_CurrentState)
	{
	case TownSceneState::Overview:

		UpdateTownOverview();

		break;

	case TownSceneState::GuildInterior:

		UpdateGuildInterior();

		break;

	case TownSceneState::Reception:

		UpdateReception();

		break;

	case TownSceneState::Shop:

		UpdateShop();

		break;

	case TownSceneState::QuestBoard:

		UpdateQuestBoard();

		break;

	case TownSceneState::QuestDetail:

		UpdateQuestDetail(
			delta);

		break;

	case TownSceneState::Inn:

		UpdateInn();

		break;

	case TownSceneState::Tavern:

		UpdateTavern();

		break;

	case TownSceneState::Market:

		UpdateMarket();

		break;

	case TownSceneState::Castle:

		UpdateCastle();

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

	//=================================================
	// Sprite
	//=================================================

	SpriteRenderer::Begin();

	m_GameObjectManager.Draw();

	SpriteRenderer::End();

	//=================================================
	// Font
	//=================================================

	FontRenderer::Begin();

	m_GameObjectManager.DrawFont();

	FontRenderer::End();

	//=================================================
	// Debug
	//=================================================

	DebugUI::RegisterDebugFunction(
		[this]()
		{
			m_GameObjectManager.
				DrawDebugUI();
		});
}

//=====================================================
// Change State
//=====================================================
void TownScene::ChangeState(
	TownSceneState state)
{
	if (m_CurrentState ==
		state)
	{
		return;
	}

	m_CurrentState =
		state;

	//=================================================
	// Quest Selection
	//=================================================

	// QuestDetailへ移動するときだけ
	// QuestBoardで選択したQuestを保持する。
	if (state !=
		TownSceneState::QuestDetail)
	{
		m_SelectedQuest =
			nullptr;
	}

	m_IsQuestAcceptPending =
		false;

	//=================================================
	// Town UI
	//=================================================

	if (m_UI)
	{
		m_UI->ApplyState(
			state);
	}

	//=================================================
	// Guild Quest UI
	//=================================================

	if (m_GuildQuestUI)
	{
		m_GuildQuestUI->ApplyState(
			state,
			m_SelectedQuest);
	}
}

//=====================================================
// Town Overview
//=====================================================

void TownScene::UpdateTownOverview()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsGuildClicked())
	{
		ChangeState(
			TownSceneState::GuildInterior);

		return;
	}

	if (m_UI->IsInnClicked())
	{
		ChangeState(
			TownSceneState::Inn);

		return;
	}

	if (m_UI->IsTavernClicked())
	{
		ChangeState(
			TownSceneState::Tavern);

		return;
	}

	if (m_UI->IsMarketClicked())
	{
		ChangeState(
			TownSceneState::Market);

		return;
	}

	if (m_UI->IsGateClicked())
	{
		SceneManager::SetCurrentScene(
			"FieldScene");

		return;
	}

	if (m_UI->IsCastleClicked())
	{
		ChangeState(
			TownSceneState::Castle);

		return;
	}
}

//=====================================================
// Guild Interior
//=====================================================

void TownScene::UpdateGuildInterior()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsReceptionClicked())
	{
		ChangeState(
			TownSceneState::Reception);

		return;
	}

	if (m_UI->IsShopClicked())
	{
		ChangeState(
			TownSceneState::Shop);

		return;
	}

	if (m_UI->IsQuestBoardClicked())
	{
		ChangeState(
			TownSceneState::QuestBoard);

		return;
	}

	if (m_UI->IsGuildBackClicked())
	{
		ChangeState(
			TownSceneState::Overview);

		return;
	}
}

//=====================================================
// Reception
//=====================================================

void TownScene::UpdateReception()
{
	if (!m_UI ||
		!m_GuildQuestUI)
	{
		return;
	}

	//=================================================
	// Quest Select
	//=================================================

	Quest* clickedQuest =
		m_GuildQuestUI->
		GetClickedReceptionQuest();

	if (clickedQuest)
	{
		m_SelectedQuest =
			clickedQuest;

		m_GuildQuestUI->
			SetSelectedQuest(
				m_SelectedQuest);

		// 選択だけなので
		// State遷移はしない。
	}

	//=================================================
	// Report
	//=================================================

	if (m_GuildQuestUI->
		IsReportClicked())
	{
		if (ReportSelectedQuest())
		{
			//=========================================
			// 報告成功
			//=========================================

			m_GuildQuestUI->
				SetSelectedQuest(
					nullptr);

			// CompletedになったQuestを
			// Reception一覧から除外する。
			m_GuildQuestUI->
				RefreshReception();

			return;
		}
	}

	//=================================================
	// Back
	//=================================================

	if (m_UI->
		IsReceptionBackClicked())
	{
		m_SelectedQuest =
			nullptr;

		ChangeState(
			TownSceneState::GuildInterior);

		return;
	}
}

//=====================================================
// Guild Shop
//=====================================================

void TownScene::UpdateShop()
{
	if (!m_UI)
	{
		return;
	}

	//=================================================
	// TODO
	//
	// Monster素材等の専門買取を
	// Item / Inventory完成後に実装する。
	//=================================================

	if (m_UI->
		IsShopBackClicked())
	{
		ChangeState(
			TownSceneState::GuildInterior);

		return;
	}
}

//=====================================================
// Quest Board
//=====================================================

void TownScene::UpdateQuestBoard()
{
	if (!m_UI ||
		!m_GuildQuestUI)
	{
		return;
	}

	//=================================================
	// QuestPaper Click
	//=================================================

	Quest* clickedQuest =
		m_GuildQuestUI->
		GetClickedBoardQuest();

	if (clickedQuest)
	{
		m_SelectedQuest =
			clickedQuest;

		ChangeState(
			TownSceneState::QuestDetail);

		return;
	}

	//=================================================
	// Back
	//=================================================

	if (m_UI->
		IsQuestBoardBackClicked())
	{
		ChangeState(
			TownSceneState::GuildInterior);

		return;
	}
}

//=====================================================
// Quest Detail
//=====================================================

void TownScene::UpdateQuestDetail(
	uint64_t delta)
{
	(void)delta;

	if (!m_UI ||
		!m_GuildQuestUI ||
		!m_SelectedQuest)
	{
		return;
	}

	//=================================================
	// Sign
	//=================================================

	if (!m_IsQuestAcceptPending &&
		m_UI->IsQuestSignClicked())
	{
		// Inactiveのみ受注可能。
		if (m_SelectedQuest->GetState() ==
			QuestState::Inactive)
		{
			m_IsQuestAcceptPending =
				true;

			m_GuildQuestUI->
				StartAcceptEffect();

			return;
		}
	}

	//=================================================
	// Stamp Effect Finished
	//=================================================

	if (m_IsQuestAcceptPending &&
		m_GuildQuestUI->
		ConsumeAcceptEffectFinished())
	{
		AcceptSelectedQuest();

		m_IsQuestAcceptPending =
			false;

		ChangeState(
			TownSceneState::QuestBoard);

		return;
	}

	//=================================================
	// Back
	//=================================================

	if (!m_IsQuestAcceptPending &&
		m_UI->
		IsQuestDetailBackClicked())
	{
		ChangeState(
			TownSceneState::QuestBoard);

		return;
	}
}

//=====================================================
// Accept Selected Quest
//=====================================================

void TownScene::AcceptSelectedQuest()
{
	if (!m_SelectedQuest)
	{
		return;
	}

	if (m_SelectedQuest->GetState() !=
		QuestState::Inactive)
	{
		return;
	}

	m_SelectedQuest->Accept();

	OutputDebugStringA(
		"[Guild] Quest Accepted: ");

	OutputDebugStringA(
		m_SelectedQuest->
		GetName().c_str());

	OutputDebugStringA(
		"\n");
}

//=====================================================
// Report Selected Quest
//=====================================================

bool TownScene::ReportSelectedQuest()
{
	if (!m_SelectedQuest)
	{
		return false;
	}

	//=================================================
	// State Check
	//=================================================

	if (m_SelectedQuest->GetState() !=
		QuestState::ReadyToReport)
	{
		return false;
	}

	//=================================================
	// Report
	//=================================================

	if (!m_SelectedQuest->Report())
	{
		return false;
	}

	OutputDebugStringA(
		"[Guild] Quest Reported: ");

	OutputDebugStringA(
		m_SelectedQuest->
		GetName().c_str());

	OutputDebugStringA(
		"\n");

	// 完了Questは選択解除。
	m_SelectedQuest =
		nullptr;

	return true;
}

//=====================================================
// Inn
//=====================================================

void TownScene::UpdateInn()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsInnBackClicked())
	{
		ChangeState(
			TownSceneState::Overview);

		return;
	}
}

//=====================================================
// Tavern
//=====================================================

void TownScene::UpdateTavern()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsTavernBackClicked())
	{
		ChangeState(
			TownSceneState::Overview);

		return;
	}
}

//=====================================================
// Market
//=====================================================

void TownScene::UpdateMarket()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsMarketBackClicked())
	{
		ChangeState(
			TownSceneState::Overview);

		return;
	}
}

//=====================================================
// Castle
//=====================================================

void TownScene::UpdateCastle()
{
	if (!m_UI)
	{
		return;
	}

	if (m_UI->IsCastleBackClicked())
	{
		ChangeState(
			TownSceneState::Overview);

		return;
	}
}
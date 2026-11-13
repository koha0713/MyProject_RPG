/**
 * @file TownSceneUI.cpp
 */

#include "TownSceneUI.h"

#include "GameObjectManager.h"
#include "GameObject.h"

#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"

#include "Window.h"

 //=====================================================
 // Initialize
 //=====================================================

void TownSceneUI::Initialize(
	GameObjectManager& manager)
{
	m_Manager =
		&manager;

	CreateBackground();

	//=================================================
	// Town
	//=================================================

	CreateOverviewUI();

	//=================================================
	// Guild
	//=================================================

	CreateGuildInteriorUI();

	CreateReceptionUI();

	CreateShopUI();

	CreateQuestBoardUI();

	CreateQuestDetailUI();

	//=================================================
	// Other Town Facilities
	//=================================================

	CreateInnUI();

	CreateTavernUI();

	CreateMarketUI();

	CreateCastleUI();
}

//=====================================================
// Finalize
//=====================================================

void TownSceneUI::Finalize()
{
	//=================================================
	// Background
	//=================================================

	m_Background =
		nullptr;

	//=================================================
	// Overview
	//=================================================

	m_GuildButton =
		nullptr;

	m_InnButton =
		nullptr;

	m_TavernButton =
		nullptr;

	m_MarketButton =
		nullptr;

	m_GateButton =
		nullptr;

	m_CastleButton =
		nullptr;

	//=================================================
	// Guild
	//=================================================

	m_ReceptionButton =
		nullptr;

	m_ShopButton =
		nullptr;

	m_QuestBoardButton =
		nullptr;

	m_GuildBackButton =
		nullptr;

	//=================================================
	// Reception
	//=================================================

	m_ReceptionBackButton =
		nullptr;

	//=================================================
	// Shop
	//=================================================

	m_ShopBackButton =
		nullptr;

	//=================================================
	// QuestBoard
	//=================================================

	m_QuestBoardBackButton =
		nullptr;

	//=================================================
	// QuestDetail
	//=================================================

	m_QuestSignButton =
		nullptr;

	m_QuestDetailBackButton =
		nullptr;

	// Managerは非所有。
	m_Manager =
		nullptr;

	//=================================================
// Inn
//=================================================

	m_InnBackButton =
		nullptr;

	//=================================================
	// Tavern
	//=================================================

	m_TavernBackButton =
		nullptr;

	//=================================================
	// Market
	//=================================================

	m_MarketBackButton =
		nullptr;

	//=================================================
	// Castle
	//=================================================

	m_CastleBackButton =
		nullptr;
}

//=====================================================
// Create Background
//=====================================================

void TownSceneUI::CreateBackground()
{
	if (!m_Manager)
	{
		return;
	}

	m_Background =
		m_Manager->Create<
		GameObject>();

	if (!m_Background)
	{
		return;
	}

	m_Background->SetName(
		"TownBackground");

	m_Background->SetTag(
		Tag::UI);

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
			"Assets/Textures/Town/mati.png");
	}
}

//=====================================================
// Create Overview UI
//=====================================================

void TownSceneUI::CreateOverviewUI()
{
	m_GuildButton =
		CreateButton(
			"OverviewGuildButton");

	m_InnButton =
		CreateButton(
			"OverviewInnButton");

	m_TavernButton =
		CreateButton(
			"OverviewTavernButton");

	m_MarketButton =
		CreateButton(
			"OverviewMarketButton");

	m_GateButton =
		CreateButton(
			"OverviewGateButton");

	m_CastleButton =
		CreateButton(
			"OverviewCastleButton");
}

//=====================================================
// Create Guild Interior UI
//=====================================================

void TownSceneUI::CreateGuildInteriorUI()
{
	m_ReceptionButton =
		CreateButton(
			"GuildReceptionButton");

	m_ShopButton =
		CreateButton(
			"GuildShopButton");

	m_QuestBoardButton =
		CreateButton(
			"GuildQuestBoardButton");

	m_GuildBackButton =
		CreateButton(
			"GuildBackButton");
}

//=====================================================
// Create Reception UI
//=====================================================

void TownSceneUI::CreateReceptionUI()
{
	m_ReceptionBackButton =
		CreateButton(
			"ReceptionBackButton");
}

//=====================================================
// Create Shop UI
//=====================================================

void TownSceneUI::CreateShopUI()
{
	m_ShopBackButton =
		CreateButton(
			"ShopBackButton");
}

//=====================================================
// Create QuestBoard UI
//=====================================================

void TownSceneUI::CreateQuestBoardUI()
{
	m_QuestBoardBackButton =
		CreateButton(
			"QuestBoardBackButton");
}

//=====================================================
// Create QuestDetail UI
//=====================================================

void TownSceneUI::CreateQuestDetailUI()
{
	m_QuestSignButton =
		CreateButton(
			"QuestSignButton");

	m_QuestDetailBackButton =
		CreateButton(
			"QuestDetailBackButton");
}

//=====================================================
// Create Inn UI
//=====================================================

void TownSceneUI::CreateInnUI()
{
	m_InnBackButton =
		CreateButton(
			"InnBackButton");
}

//=====================================================
// Create Tavern UI
//=====================================================

void TownSceneUI::CreateTavernUI()
{
	m_TavernBackButton =
		CreateButton(
			"TavernBackButton");
}

//=====================================================
// Create Market UI
//=====================================================

void TownSceneUI::CreateMarketUI()
{
	m_MarketBackButton =
		CreateButton(
			"MarketBackButton");
}

//=====================================================
// Create Castle UI
//=====================================================

void TownSceneUI::CreateCastleUI()
{
	m_CastleBackButton =
		CreateButton(
			"CastleBackButton");
}

//=====================================================
// Create Button
//=====================================================

GameObject* TownSceneUI::CreateButton(
	const char* name)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* buttonObject =
		m_Manager->Create<
		GameObject>();

	if (!buttonObject)
	{
		return nullptr;
	}

	buttonObject->SetName(
		name);

	buttonObject->SetTag(
		Tag::UI);

	// Rect
	buttonObject->AddComponent<
		UIRectComponent>();

	// Image
	buttonObject->AddComponent<
		UIImageComponent>();

	// Button
	buttonObject->AddComponent<
		UIButtonComponent>();

	return buttonObject;
}

//=====================================================
// Apply State
//=====================================================

void TownSceneUI::ApplyState(
	TownSceneState state)
{
	// 前StateのButtonを全て隠す。
	HideAllButtons();

	switch (state)
	{
	case TownSceneState::Overview:

		ApplyOverview();

		break;

	case TownSceneState::GuildInterior:

		ApplyGuildInterior();

		break;

	case TownSceneState::Reception:

		ApplyReception();

		break;

	case TownSceneState::Shop:

		ApplyShop();

		break;

	case TownSceneState::QuestBoard:

		ApplyQuestBoard();

		break;

	case TownSceneState::QuestDetail:

		ApplyQuestDetail();

		break;

	case TownSceneState::Inn:

		ApplyInn();

		break;

	case TownSceneState::Tavern:

		ApplyTavern();

		break;

	case TownSceneState::Market:

		ApplyMarket();

		break;

	case TownSceneState::Castle:

		ApplyCastle();

		break;

	default:
		break;
	}
}

//=====================================================
// Apply Overview
//=====================================================

void TownSceneUI::ApplyOverview()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/mati.png");

	//=================================================
	// 冒険者ギルド
	//=================================================

	ConfigureButton(
		m_GuildButton,
		"Assets/Textures/Town/UI_ギルドA.png",
		110.0f,
		205.0f,
		150.0f,
		130.0f);

	//=================================================
	// 宿屋
	//=================================================

	ConfigureButton(
		m_InnButton,
		"Assets/Textures/Town/UI_宿屋A.png",
		600.0f,
		130.0f,
		130.0f,
		115.0f);

	//=================================================
	// 酒場
	//=================================================

	ConfigureButton(
		m_TavernButton,
		"Assets/Textures/Town/UI_酒場A.png",
		500.0f,
		420.0f,
		140.0f,
		120.0f);

	//=================================================
	// 市場
	//=================================================

	ConfigureButton(
		m_MarketButton,
		"Assets/Textures/Town/UI_市場A.png",
		700.0f,
		395.0f,
		170.0f,
		145.0f);

	//=================================================
	// 町の外への門
	//=================================================

	ConfigureButton(
		m_GateButton,
		"Assets/Textures/Town/UI_城門A.png",
		180.0f,
		0.0f,
		140.0f,
		150.0f);

	//=================================================
	// 王城
	//=================================================

	ConfigureButton(
		m_CastleButton,
		"Assets/Textures/Town/UI_王城A.png",
		890.0f,
		0.0f,
		180.0f,
		160.0f);
}

//=====================================================
// Apply Guild Interior
//=====================================================

void TownSceneUI::ApplyGuildInterior()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/GuildBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	//=================================================
	// Reception
	//=================================================
	//
	// 現状は仮Texture / 仮位置。
	// GuildBackgroundに合わせて後で調整する。
	//=================================================

	ConfigureButton(
		m_ReceptionButton,
		"Assets/Textures/Town/Skelton.png",
		0.0f,
		150.0f,
		410.0f,
		445.0f);

	//=================================================
	// Shop
	//=================================================

	ConfigureButton(
		m_ShopButton,
		"Assets/Textures/Town/Skelton.png",
		410.0f,
		300.0f,
		200.0f,
		250.0f);

	//=================================================
	// Quest Board
	//=================================================

	ConfigureButton(
		m_QuestBoardButton,
		"Assets/Textures/Town/Skelton.png",
		980.0f,
		140.0f,
		300.0f,
		500.0f);

	//=================================================
	// Back
	//=================================================

	ConfigureButton(
		m_GuildBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Reception
//=====================================================

void TownSceneUI::ApplyReception()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/ReceptionBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_ReceptionBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Shop
//=====================================================

void TownSceneUI::ApplyShop()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/ShopBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_ShopBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply QuestBoard
//=====================================================

void TownSceneUI::ApplyQuestBoard()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/QuestBoardBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_QuestBoardBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply QuestDetail
//=====================================================

void TownSceneUI::ApplyQuestDetail()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/QuestPaper.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	//=================================================
	// Sign
	//=================================================

	ConfigureButton(
		m_QuestSignButton,
		"Assets/Textures/Town/SignButton.png",
		850.0f,
		550.0f,
		280.0f,
		80.0f);

	//=================================================
	// Back
	//=================================================

	ConfigureButton(
		m_QuestDetailBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Inn
//=====================================================

void TownSceneUI::ApplyInn()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/InnBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_InnBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Tavern
//=====================================================

void TownSceneUI::ApplyTavern()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/TavernBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_TavernBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Market
//=====================================================

void TownSceneUI::ApplyMarket()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/MarketBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_MarketBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Apply Castle
//=====================================================

void TownSceneUI::ApplyCastle()
{
	SetBackgroundTexture(
		"Assets/Textures/Town/CastleBackground.png");

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	ConfigureButton(
		m_CastleBackButton,
		"Assets/Textures/Town/BackButton.png",
		40.0f,
		screenHeight - 110.0f,
		240.0f,
		70.0f);
}

//=====================================================
// Update Visuals
//=====================================================

void TownSceneUI::UpdateVisuals(
	TownSceneState state)
{
	switch (state)
	{
	case TownSceneState::Overview:

		UpdateButtonVisual(
			m_GuildButton);

		UpdateButtonVisual(
			m_InnButton);

		UpdateButtonVisual(
			m_TavernButton);

		UpdateButtonVisual(
			m_MarketButton);

		UpdateButtonVisual(
			m_GateButton);

		UpdateButtonVisual(
			m_CastleButton);

		break;

	case TownSceneState::GuildInterior:

		UpdateButtonVisual(
			m_ReceptionButton);

		UpdateButtonVisual(
			m_ShopButton);

		UpdateButtonVisual(
			m_QuestBoardButton);

		UpdateButtonVisual(
			m_GuildBackButton);

		break;

	case TownSceneState::Reception:

		UpdateButtonVisual(
			m_ReceptionBackButton);

		break;

	case TownSceneState::Shop:

		UpdateButtonVisual(
			m_ShopBackButton);

		break;

	case TownSceneState::QuestBoard:

		UpdateButtonVisual(
			m_QuestBoardBackButton);

		break;

	case TownSceneState::QuestDetail:

		UpdateButtonVisual(
			m_QuestSignButton);

		UpdateButtonVisual(
			m_QuestDetailBackButton);

		break;

	case TownSceneState::Inn:

		UpdateButtonVisual(
			m_InnBackButton);

		break;

	case TownSceneState::Tavern:

		UpdateButtonVisual(
			m_TavernBackButton);

		break;

	case TownSceneState::Market:

		UpdateButtonVisual(
			m_MarketBackButton);

		break;

	case TownSceneState::Castle:

		UpdateButtonVisual(
			m_CastleBackButton);

		break;

	default:
		break;
	}
}

//=====================================================
// Set Background
//=====================================================

void TownSceneUI::SetBackgroundTexture(
	const char* texturePath)
{
	if (!m_Background ||
		!texturePath)
	{
		return;
	}

	auto* image =
		m_Background->GetComponent<
		UIImageComponent>();

	if (!image)
	{
		return;
	}

	image->SetTexture(
		texturePath);
}

//=====================================================
// Configure Button
//=====================================================

void TownSceneUI::ConfigureButton(
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

		// State切替直後はNormal色へ戻す。
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

void TownSceneUI::HideButton(
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

	// Sizeを0にして
	// Draw / MouseHitの両方を無効化する。
	rect->SetSize(
		0.0f,
		0.0f);
}

//=====================================================
// Hide All Buttons
//=====================================================

void TownSceneUI::HideAllButtons()
{
	// Overview
	HideButton(
		m_GuildButton);

	HideButton(
		m_InnButton);

	HideButton(
		m_TavernButton);

	HideButton(
		m_MarketButton);

	HideButton(
		m_GateButton);

	HideButton(
		m_CastleButton);

	// Guild
	HideButton(
		m_ReceptionButton);

	HideButton(
		m_ShopButton);

	HideButton(
		m_QuestBoardButton);

	HideButton(
		m_GuildBackButton);

	// Reception
	HideButton(
		m_ReceptionBackButton);

	// Shop
	HideButton(
		m_ShopBackButton);

	// QuestBoard
	HideButton(
		m_QuestBoardBackButton);

	// QuestDetail
	HideButton(
		m_QuestSignButton);

	HideButton(
		m_QuestDetailBackButton);

	// Inn
	HideButton(
		m_InnBackButton);

	// Tavern
	HideButton(
		m_TavernBackButton);

	// Market
	HideButton(
		m_MarketBackButton);

	// Castle
	HideButton(
		m_CastleBackButton);
}

//=====================================================
// Update Button Visual
//=====================================================

void TownSceneUI::UpdateButtonVisual(
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

	// 非表示Buttonは処理不要。
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
				0.8f,
				0.8f,
				0.8f,
				1.0f));

		break;

	case UIButtonState::Pressed:

		image->SetColor(
			Color(
				0.6f,
				0.6f,
				0.6f,
				1.0f));

		break;

	default:
		break;
	}
}

//=====================================================
// Is Button Clicked
//=====================================================

bool TownSceneUI::IsButtonClicked(
	GameObject* buttonObject) const
{
	if (!buttonObject)
	{
		return false;
	}

	auto* button =
		buttonObject->GetComponent<
		UIButtonComponent>();

	if (!button)
	{
		return false;
	}

	return button->IsClicked();
}

//=====================================================
// Overview Input
//=====================================================

bool TownSceneUI::IsGuildClicked() const
{
	return IsButtonClicked(
		m_GuildButton);
}

bool TownSceneUI::IsInnClicked() const
{
	return IsButtonClicked(
		m_InnButton);
}

bool TownSceneUI::IsTavernClicked() const
{
	return IsButtonClicked(
		m_TavernButton);
}

bool TownSceneUI::IsMarketClicked() const
{
	return IsButtonClicked(
		m_MarketButton);
}

bool TownSceneUI::IsGateClicked() const
{
	return IsButtonClicked(
		m_GateButton);
}

bool TownSceneUI::IsCastleClicked() const
{
	return IsButtonClicked(
		m_CastleButton);
}

//=====================================================
// Guild Input
//=====================================================

bool TownSceneUI::IsReceptionClicked() const
{
	return IsButtonClicked(
		m_ReceptionButton);
}

bool TownSceneUI::IsShopClicked() const
{
	return IsButtonClicked(
		m_ShopButton);
}

bool TownSceneUI::IsQuestBoardClicked() const
{
	return IsButtonClicked(
		m_QuestBoardButton);
}

bool TownSceneUI::IsGuildBackClicked() const
{
	return IsButtonClicked(
		m_GuildBackButton);
}

//=====================================================
// Reception Input
//=====================================================

bool TownSceneUI::IsReceptionBackClicked() const
{
	return IsButtonClicked(
		m_ReceptionBackButton);
}

//=====================================================
// Shop Input
//=====================================================

bool TownSceneUI::IsShopBackClicked() const
{
	return IsButtonClicked(
		m_ShopBackButton);
}

//=====================================================
// QuestBoard Input
//=====================================================

bool TownSceneUI::IsQuestBoardBackClicked() const
{
	return IsButtonClicked(
		m_QuestBoardBackButton);
}

//=====================================================
// QuestDetail Input
//=====================================================

bool TownSceneUI::IsQuestSignClicked() const
{
	return IsButtonClicked(
		m_QuestSignButton);
}

bool TownSceneUI::IsQuestDetailBackClicked() const
{
	return IsButtonClicked(
		m_QuestDetailBackButton);
}

bool TownSceneUI::IsInnBackClicked() const
{
	return IsButtonClicked(
		m_InnBackButton);
}

bool TownSceneUI::IsTavernBackClicked() const
{
	return IsButtonClicked(
		m_TavernBackButton);
}

bool TownSceneUI::IsMarketBackClicked() const
{
	return IsButtonClicked(
		m_MarketBackButton);
}

bool TownSceneUI::IsCastleBackClicked() const
{
	return IsButtonClicked(
		m_CastleBackButton);
}
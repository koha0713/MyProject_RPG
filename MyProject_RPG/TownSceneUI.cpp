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

	// ManagerÇÕîÒèäóLÅB
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
	// ëOStateÇÃButtonÇëSÇƒâBÇ∑ÅB
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
	// ñ`åØé“ÉMÉãÉh
	//=================================================

	ConfigureButton(
		m_GuildButton,
		"Assets/Textures/Town/UI_ÉMÉãÉh.png",
		180.0f,
		350.0f,
		150.0f,
		130.0f);

	//=================================================
	// èhâÆ
	//=================================================

	ConfigureButton(
		m_InnButton,
		"Assets/Textures/Town/UI_èhâÆ.png",
		410.0f,
		430.0f,
		130.0f,
		115.0f);

	//=================================================
	// éèÍ
	//=================================================

	ConfigureButton(
		m_TavernButton,
		"Assets/Textures/Town/UI_éèÍ.png",
		585.0f,
		420.0f,
		140.0f,
		120.0f);

	//=================================================
	// ésèÍ
	//=================================================

	ConfigureButton(
		m_MarketButton,
		"Assets/Textures/Town/UI_ésèÍ.png",
		880.0f,
		395.0f,
		170.0f,
		145.0f);

	//=================================================
	// í¨ÇÃäOÇ÷ÇÃñÂ
	//=================================================

	ConfigureButton(
		m_GateButton,
		"Assets/Textures/Town/UI_èÈñÂ.png",
		1030.0f,
		230.0f,
		140.0f,
		150.0f);

	//=================================================
	// â§èÈ
	//=================================================

	ConfigureButton(
		m_CastleButton,
		"Assets/Textures/Town/UI_â§èÈ.png",
		860.0f,
		60.0f,
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
	// åªèÛÇÕâºTexture / âºà íuÅB
	// GuildBackgroundÇ…çáÇÌÇπÇƒå„Ç≈í≤êÆÇ∑ÇÈÅB
	//=================================================

	ConfigureButton(
		m_ReceptionButton,
		"Assets/Textures/Town/UI_ÉMÉãÉh.png",
		45.0f,
		290.0f,
		450.0f,
		280.0f);

	//=================================================
	// Shop
	//=================================================

	ConfigureButton(
		m_ShopButton,
		"Assets/Textures/Town/UI_â§èÈ.png",
		490.0f,
		300.0f,
		300.0f,
		100.0f);

	//=================================================
	// Quest Board
	//=================================================

	ConfigureButton(
		m_QuestBoardButton,
		"Assets/Textures/Town/UI_ésèÍ.png",
		860.0f,
		300.0f,
		300.0f,
		100.0f);

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

		// Stateêÿë÷íºå„ÇÕNormalêFÇ÷ñﬂÇ∑ÅB
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

	// SizeÇ0Ç…ÇµÇƒ
	// Draw / MouseHitÇÃóºï˚Çñ≥å¯âªÇ∑ÇÈÅB
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

	// îÒï\é¶ButtonÇÕèàóùïsóvÅB
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
#include "FieldSceneUI.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"
#include "UIFontComponent.h"

#include "CharacterStatusComponent.h"

#include <string>
#include <Windows.h>

namespace
{
	/**
	 * @brief UTF-8 -> UTF-16
	 */
	std::wstring UTF8ToWide(
		const std::string& text)
	{
		if (text.empty())
		{
			return std::wstring();
		}

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
}

//=====================================================
// Initialize
//=====================================================
void FieldSceneUI::Initialize(
	GameObjectManager& manager)
{
	m_Manager =
		&manager;

	CreateNextTurnUI();

	CreatePlayerHUD();

	SetNextTurnVisible(
		false);

	HidePlayerHUD();

	m_ClickedPlayer =
		nullptr;
}

//=====================================================
// Finalize
//=====================================================

void FieldSceneUI::Finalize()
{
	m_PlayerIcons.clear();

	m_ClickedPlayer =
		nullptr;
	m_NextTurnButton =
		nullptr;

	m_NextTurnText =
		nullptr;

	m_PlayerHUDBackground =
		nullptr;

	m_PlayerIcon =
		nullptr;

	m_PlayerNameText =
		nullptr;

	m_HPBarBackground =
		nullptr;

	m_HPBarFill =
		nullptr;

	m_HPText =
		nullptr;

	m_MPBarBackground =
		nullptr;

	m_MPBarFill =
		nullptr;

	m_MPText =
		nullptr;

	m_Manager =
		nullptr;
}

//=====================================================
// Create Next Turn
//=====================================================

void FieldSceneUI::CreateNextTurnUI()
{
	if (!m_Manager)
	{
		return;
	}

	//=================================================
	// Button
	//=================================================

	m_NextTurnButton =
		m_Manager->Create<GameObject>();

	if (m_NextTurnButton)
	{
		m_NextTurnButton->SetName(
			"NextTurnButton");

		m_NextTurnButton->SetTag(
			Tag::UI);

		auto* rect =
			m_NextTurnButton->
			AddComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				1030.0f,
				610.0f);

			rect->SetSize(
				220.0f,
				70.0f);
		}

		auto* image =
			m_NextTurnButton->
			AddComponent<
			UIImageComponent>();

		if (image)
		{
			// 既存Textureを利用。
			// 必要なら後で専用Button Textureへ変更可能。
			image->SetTexture(
				"Assets/Textures/Field/Next.png");

			image->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));
		}

		m_NextTurnButton->
			AddComponent<
			UIButtonComponent>();
	}

	//=================================================
	// Text
	//=================================================

	m_NextTurnText =
		CreateText(
			"NextTurnText");

	if (m_NextTurnText)
	{
		auto* rect =
			m_NextTurnText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_NextTurnText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				1050.0f,
				625.0f);

			rect->SetSize(
				180.0f,
				45.0f);
		}

		if (font)
		{
			font->SetText(
				L"次のターンへ");

			font->SetFontSize(
				26.0f);

			font->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));
		}
	}
}

//=====================================================
// Create Player HUD
//=====================================================

void FieldSceneUI::CreatePlayerHUD()
{
	//=================================================
	// Background
	//=================================================

	m_PlayerHUDBackground =
		CreateImage(
			"PlayerHUDBackground");

	if (m_PlayerHUDBackground)
	{
		auto* rect =
			m_PlayerHUDBackground->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_PlayerHUDBackground->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				25.0f,
				530.0f);

			rect->SetSize(
				420.0f,
				160.0f);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/PlayerHUD.png");
		}
	}

	//=================================================
	// Character Icon
	//=================================================

	m_PlayerIcon =
		CreateImage(
			"PlayerHUDIcon");

	if (m_PlayerIcon)
	{
		auto* rect =
			m_PlayerIcon->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_PlayerIcon->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				45.0f,
				550.0f);

			rect->SetSize(
				115.0f,
				115.0f);
		}

		if (image)
		{
			// 現在PlayerはWarriorのみ対応。
			image->SetTexture(
				"Assets/Textures/Field/WarriorIcon.png");
		}
	}

	//=================================================
	// Character Name
	//=================================================

	m_PlayerNameText =
		CreateText(
			"PlayerHUDName");

	if (m_PlayerNameText)
	{
		auto* rect =
			m_PlayerNameText->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				545.0f);

			rect->SetSize(
				220.0f,
				40.0f);
		}
	}

	//=================================================
	// HP Background
	//=================================================

	m_HPBarBackground =
		CreateImage(
			"PlayerHUDHPBackground");

	if (m_HPBarBackground)
	{
		auto* rect =
			m_HPBarBackground->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_HPBarBackground->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				600.0f);

			rect->SetSize(
				210.0f,
				18.0f);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/BarBackground.png");
		}
	}

	//=================================================
	// HP Fill
	//=================================================

	m_HPBarFill =
		CreateImage(
			"PlayerHUDHPFill");

	if (m_HPBarFill)
	{
		auto* image =
			m_HPBarFill->
			GetComponent<
			UIImageComponent>();

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/BarFill.png");

			image->SetColor(
				Color(
					0.85f,
					0.15f,
					0.15f,
					1.0f));
		}
	}

	m_HPText =
		CreateText(
			"PlayerHUDHPText");

	//=================================================
	// MP Background
	//=================================================

	m_MPBarBackground =
		CreateImage(
			"PlayerHUDMPBackground");

	if (m_MPBarBackground)
	{
		auto* rect =
			m_MPBarBackground->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_MPBarBackground->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				640.0f);

			rect->SetSize(
				210.0f,
				18.0f);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/BarBackground.png");
		}
	}

	//=================================================
	// MP Fill
	//=================================================

	m_MPBarFill =
		CreateImage(
			"PlayerHUDMPFill");

	if (m_MPBarFill)
	{
		auto* image =
			m_MPBarFill->
			GetComponent<
			UIImageComponent>();

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/BarFill.png");

			image->SetColor(
				Color(
					0.15f,
					0.35f,
					0.90f,
					1.0f));
		}
	}

	m_MPText =
		CreateText(
			"PlayerHUDMPText");
}

void FieldSceneUI::CreatePlayerIcons()
{
	if (!m_Manager)
	{
		return;
	}

	//=================================================
	// Layout
	//=================================================
	//
	// 現在1280x720前提のUI配置。
	//
	// 左下から上方向へ並べる。
	//=================================================

	constexpr float ICON_X =
		20.0f;

	constexpr float ICON_START_Y =
		630.0f;

	constexpr float ICON_SIZE =
		64.0f;

	constexpr float ICON_INTERVAL =
		72.0f;

	for (size_t i = 0;
		i < m_PlayerIcons.size();
		++i)
	{
		FieldPlayerIconUI& iconUI =
			m_PlayerIcons[i];

		if (!iconUI.Player)
		{
			continue;
		}

		//=============================================
		// Object
		//=============================================

		GameObject* button =
			CreateImage(
				"FieldPlayerIcon");

		if (!button)
		{
			continue;
		}

		iconUI.Button =
			button;

		//=============================================
		// Rect
		//=============================================

		auto* rect =
			button->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			const float y =
				ICON_START_Y -
				static_cast<float>(i) *
				ICON_INTERVAL;

			rect->SetPosition(
				ICON_X,
				y);

			rect->SetSize(
				ICON_SIZE,
				ICON_SIZE);
		}

		//=============================================
		// Image
		//=============================================

		auto* image =
			button->
			GetComponent<
			UIImageComponent>();

		if (image)
		{
			// 現在はWarriorのみ。
			// 将来CharacterData等からIconPathを取得する。
			image->SetTexture(
				"Assets/Textures/Field/WarriorIcon.png");

			image->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));
		}

		//=============================================
		// Button
		//=============================================

		button->
			AddComponent<
			UIButtonComponent>();
	}
}

//=====================================================
// Create Image
//=====================================================

GameObject* FieldSceneUI::CreateImage(
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
// Create Text
//=====================================================

GameObject* FieldSceneUI::CreateText(
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
		Tag::Font);

	object->AddComponent<
		UIRectComponent>();

	auto* font =
		object->AddComponent<
		UIFontComponent>();

	if (font)
	{
		font->SetFontSize(
			20.0f);

		font->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}

	return object;
}

//=====================================================
// Update
//=====================================================
void FieldSceneUI::Update(
	GameObject* focusedPlayer)
{
	//=================================================
	// Reset Frame Event
	//=================================================

	m_ClickedPlayer =
		nullptr;

	//=================================================
	// Next Turn
	//=================================================

	UpdateNextTurnVisual();

	//=================================================
	// Player List
	//=================================================

	UpdatePlayerIcons(
		focusedPlayer);

	//=================================================
	// Detail HUD
	//=================================================

	if (!focusedPlayer)
	{
		HidePlayerHUD();

		return;
	}

	UpdatePlayerHUD(
		focusedPlayer);
}

void FieldSceneUI::SetPlayers(
	const std::vector<GameObject*>& players)
{
	if (!m_Manager)
	{
		return;
	}

	//=================================================
	// Old Icons
	//=================================================
	//
	// 現段階ではField中にParty構成が変わらない前提。
	// SetPlayers()は基本的に初期化時1回だけ呼ぶ。
	//=================================================

	ClearPlayerIcons();

	//=================================================
	// Player References
	//=================================================

	m_PlayerIcons.reserve(
		players.size());

	for (GameObject* player :
		players)
	{
		if (!player)
		{
			continue;
		}

		FieldPlayerIconUI iconUI{};

		iconUI.Player =
			player;

		m_PlayerIcons.push_back(
			iconUI);
	}

	//=================================================
	// UI Create
	//=================================================

	CreatePlayerIcons();
}

//=====================================================
// Next Turn Visible
//=====================================================

void FieldSceneUI::SetNextTurnVisible(
	bool visible)
{
	m_NextTurnVisible =
		visible;

	if (!m_NextTurnButton ||
		!m_NextTurnText)
	{
		return;
	}

	auto* buttonRect =
		m_NextTurnButton->
		GetComponent<
		UIRectComponent>();

	auto* button =
		m_NextTurnButton->
		GetComponent<
		UIButtonComponent>();

	auto* textRect =
		m_NextTurnText->
		GetComponent<
		UIRectComponent>();

	if (!visible)
	{
		if (buttonRect)
		{
			buttonRect->SetSize(
				0.0f,
				0.0f);
		}

		if (textRect)
		{
			textRect->SetSize(
				0.0f,
				0.0f);
		}

		if (button)
		{
			button->SetEnabled(
				false);
		}

		return;
	}

	if (buttonRect)
	{
		buttonRect->SetPosition(
			1030.0f,
			610.0f);

		buttonRect->SetSize(
			220.0f,
			70.0f);
	}

	if (textRect)
	{
		textRect->SetPosition(
			1050.0f,
			625.0f);

		textRect->SetSize(
			180.0f,
			45.0f);
	}

	if (button)
	{
		button->SetEnabled(
			true);
	}
}

//=====================================================
// Next Turn Visual
//=====================================================

void FieldSceneUI::UpdateNextTurnVisual()
{
	if (!m_NextTurnVisible ||
		!m_NextTurnButton)
	{
		return;
	}

	auto* button =
		m_NextTurnButton->
		GetComponent<
		UIButtonComponent>();

	auto* image =
		m_NextTurnButton->
		GetComponent<
		UIImageComponent>();

	if (!button ||
		!image ||
		!button->IsEnabled())
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

void FieldSceneUI::UpdatePlayerIcons(
	GameObject* focusedPlayer)
{
	for (FieldPlayerIconUI& iconUI :
		m_PlayerIcons)
	{
		if (!iconUI.Player ||
			!iconUI.Button)
		{
			continue;
		}

		auto* button =
			iconUI.Button->
			GetComponent<
			UIButtonComponent>();

		auto* image =
			iconUI.Button->
			GetComponent<
			UIImageComponent>();

		if (!button ||
			!image)
		{
			continue;
		}

		//=============================================
		// Click
		//=============================================

		if (button->IsClicked())
		{
			m_ClickedPlayer =
				iconUI.Player;
		}

		//=============================================
		// Selected Visual
		//=============================================
		//
		// HUD表示対象Playerだけ少し明るくする。
		//=================================================

		if (iconUI.Player ==
			focusedPlayer)
		{
			image->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));
		}
		else
		{
			image->SetColor(
				Color(
					0.65f,
					0.65f,
					0.65f,
					1.0f));
		}
	}
}

void FieldSceneUI::ClearPlayerIcons()
{
	//=================================================
	// NOTE
	//=================================================
	//
	// GameObject本体はGameObjectManager所有。
	//
	// 現段階ではSetPlayers()をField初期化時に
	// 1度だけ呼ぶ前提なので、参照だけ破棄する。
	//
	// Party再編成をField中に行う場合は、
	// GameObjectManager側のDestroy APIを使って
	// UI Object自体も破棄する必要がある。
	//=================================================

	m_PlayerIcons.clear();

	m_ClickedPlayer =
		nullptr;
}

//=====================================================
// Next Turn Click
//=====================================================

bool FieldSceneUI::IsNextTurnClicked() const
{
	if (!m_NextTurnButton ||
		!m_NextTurnVisible)
	{
		return false;
	}

	auto* button =
		m_NextTurnButton->
		GetComponent<
		UIButtonComponent>();

	if (!button ||
		!button->IsEnabled())
	{
		return false;
	}

	return button->IsClicked();
}

//=====================================================
// Player HUD
//=====================================================

void FieldSceneUI::UpdatePlayerHUD(
	GameObject* player)
{
	if (!player)
	{
		HidePlayerHUD();

		return;
	}

	auto* status =
		player->GetComponent<
		CharacterStatusComponent>();

	if (!status)
	{
		HidePlayerHUD();

		return;
	}

	//=================================================
	// Background / Icon
	//=================================================

	auto* backgroundRect =
		m_PlayerHUDBackground->
		GetComponent<
		UIRectComponent>();

	if (backgroundRect)
	{
		backgroundRect->SetPosition(
			25.0f,
			530.0f);

		backgroundRect->SetSize(
			420.0f,
			160.0f);
	}

	auto* iconRect =
		m_PlayerIcon->
		GetComponent<
		UIRectComponent>();

	if (iconRect)
	{
		iconRect->SetPosition(
			45.0f,
			550.0f);

		iconRect->SetSize(
			115.0f,
			115.0f);
	}

	//=================================================
	// Name
	//=================================================

	if (m_PlayerNameText)
	{
		auto* rect =
			m_PlayerNameText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_PlayerNameText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				545.0f);

			rect->SetSize(
				220.0f,
				40.0f);
		}

		if (font)
		{
			font->SetText(
				UTF8ToWide(
					player->GetName()));

			font->SetFontSize(
				24.0f);
		}
	}

	//=================================================
	// HP
	//=================================================

	const int hp =
		status->GetHP();

	const int maxHP =
		status->GetMaxHP();

	const float hpRate =
		maxHP > 0
		? static_cast<float>(hp) /
		static_cast<float>(maxHP)
		: 0.0f;

	if (m_HPBarFill)
	{
		auto* rect =
			m_HPBarFill->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				600.0f);

			rect->SetSize(
				210.0f * hpRate,
				18.0f);
		}
	}

	if (m_HPText)
	{
		auto* rect =
			m_HPText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_HPText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				580.0f);

			rect->SetSize(
				210.0f,
				24.0f);
		}

		if (font)
		{
			const std::wstring text =
				L"HP " +
				std::to_wstring(hp) +
				L" / " +
				std::to_wstring(maxHP);

			font->SetText(
				text);
		}
	}

	//=================================================
	// MP
	//=================================================

	const int mp =
		status->GetMP();

	const int maxMP =
		status->GetMaxMP();

	const float mpRate =
		maxMP > 0
		? static_cast<float>(mp) /
		static_cast<float>(maxMP)
		: 0.0f;

	if (m_MPBarFill)
	{
		auto* rect =
			m_MPBarFill->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				640.0f);

			rect->SetSize(
				210.0f * mpRate,
				18.0f);
		}
	}

	if (m_MPText)
	{
		auto* rect =
			m_MPText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_MPText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				185.0f,
				620.0f);

			rect->SetSize(
				210.0f,
				24.0f);
		}

		if (font)
		{
			const std::wstring text =
				L"MP " +
				std::to_wstring(mp) +
				L" / " +
				std::to_wstring(maxMP);

			font->SetText(
				text);
		}
	}

	//=================================================
	// AP State
	//=================================================
	//
	// AP制では旧HasActedフラグを使用しない。
	//
	// APが0になったCharacterは、
	// このTurnでこれ以上通常Actionを実行できないため
	// HUDを暗く表示する。
	//=================================================

	const bool outOfAP =
		status->GetAP() <= 0;

	SetPlayerHUDDarkened(
		outOfAP);
}

//=====================================================
// HUD Darken
//=====================================================
void FieldSceneUI::SetPlayerHUDDarkened(
	bool darkened)
{
	const float brightness =
		darkened
		? 0.50f
		: 1.00f;

	//=================================================
	// Background
	//=================================================

	GameObject* neutralImages[] =
	{
		m_PlayerHUDBackground,
		m_PlayerIcon,
		m_HPBarBackground,
		m_MPBarBackground
	};

	for (GameObject* object :
		neutralImages)
	{
		if (!object)
		{
			continue;
		}

		auto* image =
			object->GetComponent<
			UIImageComponent>();

		if (image)
		{
			image->SetColor(
				Color(
					brightness,
					brightness,
					brightness,
					1.0f));
		}
	}

	//=================================================
	// HP
	//=================================================

	if (m_HPBarFill)
	{
		auto* image =
			m_HPBarFill->
			GetComponent<
			UIImageComponent>();

		if (image)
		{
			image->SetColor(
				Color(
					0.85f * brightness,
					0.15f * brightness,
					0.15f * brightness,
					1.0f));
		}
	}

	//=================================================
	// MP
	//=================================================

	if (m_MPBarFill)
	{
		auto* image =
			m_MPBarFill->
			GetComponent<
			UIImageComponent>();

		if (image)
		{
			image->SetColor(
				Color(
					0.15f * brightness,
					0.35f * brightness,
					0.90f * brightness,
					1.0f));
		}
	}

	//=================================================
	// Text
	//=================================================

	GameObject* texts[] =
	{
		m_PlayerNameText,
		m_HPText,
		m_MPText
	};

	for (GameObject* object :
		texts)
	{
		if (!object)
		{
			continue;
		}

		auto* font =
			object->GetComponent<
			UIFontComponent>();

		if (font)
		{
			font->SetColor(
				Color(
					brightness,
					brightness,
					brightness,
					1.0f));
		}
	}
}

//=====================================================
// Hide HUD
//=====================================================

void FieldSceneUI::HidePlayerHUD()
{
	GameObject* objects[] =
	{
		m_PlayerHUDBackground,
		m_PlayerIcon,
		m_PlayerNameText,
		m_HPBarBackground,
		m_HPBarFill,
		m_HPText,
		m_MPBarBackground,
		m_MPBarFill,
		m_MPText
	};

	for (GameObject* object :
		objects)
	{
		HideObject(
			object);
	}
}

//=====================================================
// Hide Object
//=====================================================

void FieldSceneUI::HideObject(
	GameObject* object)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->GetComponent<
		UIRectComponent>();

	if (rect)
	{
		rect->SetSize(
			0.0f,
			0.0f);
	}
}
#include "FieldSceneUI.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"
#include "UIFontComponent.h"

#include "CharacterStatusComponent.h"

#ifdef _DEBUG
#include "DebugUI.h"
#endif

#include <string>
#include <Windows.h>

namespace
{
	//=================================================
	// Player HUD Layout
	//=================================================
	//
	// NOTE:
	// UI配置調整中のみfloat変数として使用する。
	//
	// 配置確定後は、
	// float → constexpr float
	// に戻して問題ない。
	//=================================================

	float PLAYER_ICON_X =
		10.0f;

	float PLAYER_ICON_START_Y =
		615.0f;

	float PLAYER_ICON_SELECTED_SIZE =
		100.0f;

	float PLAYER_ICON_NORMAL_SIZE =
		80.0f;

	float PLAYER_ICON_INTERVAL =
		82.0f;

	//=================================================
	// HUD
	//=================================================

	float PLAYER_HUD_X =
		37.0f;

	float PLAYER_HUD_Y_OFFSET =
		4.0f;

	float PLAYER_HUD_WIDTH =
		350.0f;

	float PLAYER_HUD_HEIGHT =
		100.0f;

	//=================================================
	// Name / Level / AP
	//=================================================
	//
	// HUDの左上を基準としたOffset。
	//=================================================

	float PLAYER_INFO_OFFSET_X =
		46.0f;

	float PLAYER_NAME_OFFSET_Y =
		21.0f;

	float PLAYER_LEVEL_OFFSET_Y =
		50.0f;

	float PLAYER_AP_OFFSET_Y =
		-25.0f;

	//=================================================
	// HP / MP Bar
	//=================================================
	//
	// HUD左上からのOffset。
	//=================================================

	float PLAYER_BAR_OFFSET_X =
		75.0f;

	float PLAYER_HP_BAR_OFFSET_Y =
		10.0f;

	float PLAYER_MP_BAR_OFFSET_Y =
		47.0f;

	float PLAYER_BAR_WIDTH =
		237.0f;

	float PLAYER_BAR_HEIGHT =
		40.0f;

	//=================================================
	// Fill
	//=================================================
	//
	// Frameの内側にFillを収めるための値。
	//=================================================

	float PLAYER_BAR_FILL_OFFSET_X =
		52.0f;

	float PLAYER_BAR_FILL_OFFSET_Y =
		15.0f;

	float PLAYER_BAR_FILL_RIGHT_MARGIN =
		16.0f;

	float PLAYER_BAR_FILL_HEIGHT =
		19.0f;

	//=================================================
	// AP
	//=================================================

	constexpr int AP_ICON_COUNT =
		6;

	float AP_ICON_SIZE =
		50.0f;

	float AP_ICON_INTERVAL =
		25.0f;

	float AP_ICON_OFFSET_X =
		112.0f;

	//=================================================
	// Helper
	//=================================================

	float GetPlayerInfoX()
	{
		return PLAYER_HUD_X +
			PLAYER_INFO_OFFSET_X;
	}

	float GetPlayerBarX()
	{
		return PLAYER_HUD_X +
			PLAYER_BAR_OFFSET_X;
	}

	float GetAPIconStartX()
	{
		return GetPlayerInfoX() +
			AP_ICON_OFFSET_X;
	}

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

	std::wstring GetPlayerName(GameObject* player)
	{
		const std::string& text =
					player->GetName();
		if(text == "Warrior")
		{
			return L"戦士";
		}
		else if(text == "Ranger")
		{
			return L"狩人";
		}
		else if(text == "Wizard")
		{
			return L"魔法使い";
		}
		else if(text == "Monk")
		{
			return L"拳闘士";
		}
		else if(text == "Rogue")
		{
			return L"盗賊";
		}
		else if(text == "Cleric")
		{
			return L"呪術師";
		}
		return UTF8ToWide(text);
	}

	void SetTextureToClassUI(
		GameObject* player,
		UIImageComponent* image)
	{
		const std::string& text =
			player->GetName();
		if (text == "Warrior")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
		else if (text == "Ranger")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
		else if (text == "Wizard")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
		else if (text == "Monk")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
		else if (text == "Rogue")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
		else if (text == "Cleric")
		{
			image->SetTexture(
				"Assets/Textures/Field/Class_Warrior.png");
		}
	}
}

#ifdef _DEBUG

//=====================================================
// Player HUD Layout Debug
//=====================================================

/**
 * @brief PlayerHUD配置調整用DebugUI
 *
 * @details
 * UI配置確定後は、
 * この関数とRegisterDebugFunction()呼び出しを
 * 丸ごと削除してよい。
 */
void DrawPlayerHUDLayoutDebugUI()
{
	ImGui::Begin(
		"Field Player HUD Layout");

	//=================================================
	// Player Icon
	//=================================================

	if (ImGui::CollapsingHeader(
		"Player Icon",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"Icon X",
			&PLAYER_ICON_X,
			1.0f);

		ImGui::DragFloat(
			"Icon Start Y",
			&PLAYER_ICON_START_Y,
			1.0f);

		ImGui::DragFloat(
			"Selected Size",
			&PLAYER_ICON_SELECTED_SIZE,
			1.0f,
			1.0f,
			200.0f);

		ImGui::DragFloat(
			"Normal Size",
			&PLAYER_ICON_NORMAL_SIZE,
			1.0f,
			1.0f,
			200.0f);

		ImGui::DragFloat(
			"Icon Interval",
			&PLAYER_ICON_INTERVAL,
			1.0f,
			1.0f,
			200.0f);
	}

	//=================================================
	// HUD
	//=================================================

	if (ImGui::CollapsingHeader(
		"HUD",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"HUD X",
			&PLAYER_HUD_X,
			1.0f);

		ImGui::DragFloat(
			"HUD Y",
			&PLAYER_HUD_Y_OFFSET,
			1.0f);

		ImGui::DragFloat(
			"HUD Width",
			&PLAYER_HUD_WIDTH,
			1.0f,
			1.0f,
			1000.0f);

		ImGui::DragFloat(
			"HUD Height",
			&PLAYER_HUD_HEIGHT,
			1.0f,
			1.0f,
			300.0f);
	}

	//=================================================
	// Name / Level / AP
	//=================================================

	if (ImGui::CollapsingHeader(
		"Character Info",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"Info X Offset",
			&PLAYER_INFO_OFFSET_X,
			1.0f);

		ImGui::DragFloat(
			"Name Y Offset",
			&PLAYER_NAME_OFFSET_Y,
			1.0f);

		ImGui::DragFloat(
			"Level Y Offset",
			&PLAYER_LEVEL_OFFSET_Y,
			1.0f);

		ImGui::DragFloat(
			"AP Y Offset",
			&PLAYER_AP_OFFSET_Y,
			1.0f);
	}

	//=================================================
	// HP / MP Frame
	//=================================================

	if (ImGui::CollapsingHeader(
		"HP / MP Frame",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"Bar X Offset",
			&PLAYER_BAR_OFFSET_X,
			1.0f);

		ImGui::DragFloat(
			"HP Y Offset",
			&PLAYER_HP_BAR_OFFSET_Y,
			1.0f);

		ImGui::DragFloat(
			"MP Y Offset",
			&PLAYER_MP_BAR_OFFSET_Y,
			1.0f);

		ImGui::DragFloat(
			"Bar Width",
			&PLAYER_BAR_WIDTH,
			1.0f,
			1.0f,
			500.0f);

		ImGui::DragFloat(
			"Bar Height",
			&PLAYER_BAR_HEIGHT,
			1.0f,
			1.0f,
			100.0f);
	}

	//=================================================
	// HP / MP Fill
	//=================================================

	if (ImGui::CollapsingHeader(
		"HP / MP Fill",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"Fill X Offset",
			&PLAYER_BAR_FILL_OFFSET_X,
			0.5f);

		ImGui::DragFloat(
			"Fill Y Offset",
			&PLAYER_BAR_FILL_OFFSET_Y,
			0.5f);

		ImGui::DragFloat(
			"Fill Right Margin",
			&PLAYER_BAR_FILL_RIGHT_MARGIN,
			0.5f);

		ImGui::DragFloat(
			"Fill Height",
			&PLAYER_BAR_FILL_HEIGHT,
			0.5f,
			1.0f,
			100.0f);

		// 実際にFillとして使える幅も確認可能。
		const float usableWidth =
			PLAYER_BAR_WIDTH -
			PLAYER_BAR_FILL_OFFSET_X -
			PLAYER_BAR_FILL_RIGHT_MARGIN;

		ImGui::Text(
			"Usable Fill Width : %.1f",
			usableWidth);
	}

	//=================================================
	// AP Icons
	//=================================================

	if (ImGui::CollapsingHeader(
		"AP Icons",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(
			"AP Icon X Offset",
			&AP_ICON_OFFSET_X,
			1.0f);

		ImGui::DragFloat(
			"AP Icon Size",
			&AP_ICON_SIZE,
			0.5f,
			1.0f,
			100.0f);

		ImGui::DragFloat(
			"AP Icon Interval",
			&AP_ICON_INTERVAL,
			0.5f,
			1.0f,
			100.0f);
	}

	//=================================================
	// Current Values
	//=================================================

	if (ImGui::CollapsingHeader(
		"Current Values"))
	{
		ImGui::Text(
			"PLAYER_ICON_X = %.1ff;",
			PLAYER_ICON_X);

		ImGui::Text(
			"PLAYER_ICON_START_Y = %.1ff;",
			PLAYER_ICON_START_Y);

		ImGui::Text(
			"PLAYER_HUD_X = %.1ff;",
			PLAYER_HUD_X);

		ImGui::Text(
			"PLAYER_HUD_WIDTH = %.1ff;",
			PLAYER_HUD_WIDTH);

		ImGui::Text(
			"PLAYER_HUD_HEIGHT = %.1ff;",
			PLAYER_HUD_HEIGHT);

		ImGui::Separator();

		ImGui::Text(
			"PLAYER_INFO_OFFSET_X = %.1ff;",
			PLAYER_INFO_OFFSET_X);

		ImGui::Text(
			"PLAYER_NAME_OFFSET_Y = %.1ff;",
			PLAYER_NAME_OFFSET_Y);

		ImGui::Text(
			"PLAYER_LEVEL_OFFSET_Y = %.1ff;",
			PLAYER_LEVEL_OFFSET_Y);

		ImGui::Text(
			"PLAYER_AP_OFFSET_Y = %.1ff;",
			PLAYER_AP_OFFSET_Y);

		ImGui::Separator();

		ImGui::Text(
			"PLAYER_BAR_OFFSET_X = %.1ff;",
			PLAYER_BAR_OFFSET_X);

		ImGui::Text(
			"PLAYER_HP_BAR_OFFSET_Y = %.1ff;",
			PLAYER_HP_BAR_OFFSET_Y);

		ImGui::Text(
			"PLAYER_MP_BAR_OFFSET_Y = %.1ff;",
			PLAYER_MP_BAR_OFFSET_Y);

		ImGui::Text(
			"PLAYER_BAR_WIDTH = %.1ff;",
			PLAYER_BAR_WIDTH);

		ImGui::Text(
			"PLAYER_BAR_HEIGHT = %.1ff;",
			PLAYER_BAR_HEIGHT);

		ImGui::Separator();

		ImGui::Text(
			"PLAYER_BAR_FILL_OFFSET_X = %.1ff;",
			PLAYER_BAR_FILL_OFFSET_X);

		ImGui::Text(
			"PLAYER_BAR_FILL_OFFSET_Y = %.1ff;",
			PLAYER_BAR_FILL_OFFSET_Y);

		ImGui::Text(
			"PLAYER_BAR_FILL_RIGHT_MARGIN = %.1ff;",
			PLAYER_BAR_FILL_RIGHT_MARGIN);

		ImGui::Text(
			"PLAYER_BAR_FILL_HEIGHT = %.1ff;",
			PLAYER_BAR_FILL_HEIGHT);

		ImGui::Separator();

		ImGui::Text(
			"AP_ICON_OFFSET_X = %.1ff;",
			AP_ICON_OFFSET_X);

		ImGui::Text(
			"AP_ICON_SIZE = %.1ff;",
			AP_ICON_SIZE);

		ImGui::Text(
			"AP_ICON_INTERVAL = %.1ff;",
			AP_ICON_INTERVAL);
	}

	ImGui::End();
}

#endif

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

	m_APIcons.clear();

	m_ClickedPlayer =
		nullptr;

	m_NextTurnButton =
		nullptr;

	m_NextTurnText =
		nullptr;

	m_PlayerHUDBackground =
		nullptr;

	m_PlayerNameText =
		nullptr;

	m_PlayerLevelText =
		nullptr;

	m_HPBarBackground =
		nullptr;

	m_HPBarFill =
		nullptr;

	m_HPBarFrame =
		nullptr;

	m_MPBarBackground =
		nullptr;

	m_MPBarFill =
		nullptr;

	m_MPBarFrame =
		nullptr;

	m_APText =
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
	if (!m_Manager)
	{
		return;
	}

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
				PLAYER_HUD_X,
				m_PlayerHUDY);

			rect->SetSize(
				PLAYER_HUD_WIDTH,
				PLAYER_HUD_HEIGHT);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/PlayerHUD_1.png");
		}
	}

	//=================================================
	// Character Name
	//=================================================
	//
	// HUD左側上段
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

		auto* font =
			m_PlayerNameText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_NAME_OFFSET_Y);

			rect->SetSize(
				165.0f,
				23.0f);
		}

		if (font)
		{
			font->SetFontSize(
				20.0f);
		}
	}

	//=================================================
	// Level
	//=================================================
	//
	// 名前の下へ表示。
	//=================================================

	m_PlayerLevelText =
		CreateText(
			"PlayerHUDLevel");

	if (m_PlayerLevelText)
	{
		auto* rect =
			m_PlayerLevelText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_PlayerLevelText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_LEVEL_OFFSET_Y);

			rect->SetSize(
				90.0f,
				20.0f);
		}

		if (font)
		{
			font->SetText(
				L"Lv.--");

			font->SetFontSize(
				16.0f);
		}
	}

	//=================================================
	// HP Background
	//=================================================
	//
	// ① 下地
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
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/HPBarBackground.png");
		}
	}

	//=================================================
	// HP Fill
	//=================================================
	//
	// ② Fill
	//=================================================

	m_HPBarFill =
		CreateImage(
			"PlayerHUDHPFill");

	if (m_HPBarFill)
	{
		auto* rect =
			m_HPBarFill->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_HPBarFill->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			const float fillWidth =
				PLAYER_BAR_WIDTH -
				PLAYER_BAR_FILL_OFFSET_X -
				PLAYER_BAR_FILL_RIGHT_MARGIN;

			rect->SetPosition(
				GetPlayerBarX() +
				PLAYER_BAR_FILL_OFFSET_X,

				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y +
				PLAYER_BAR_FILL_OFFSET_Y);

			rect->SetSize(
				fillWidth,
				PLAYER_BAR_FILL_HEIGHT);
		}

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

	//=================================================
	// HP Frame
	//=================================================
	//
	// ③ 装飾枠
	//=================================================

	m_HPBarFrame =
		CreateImage(
			"PlayerHUDHPFrame");

	if (m_HPBarFrame)
	{
		auto* rect =
			m_HPBarFrame->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_HPBarFrame->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/HPBarFrame.png");
		}
	}

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
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/MPBarBackground.png");
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
		auto* rect =
			m_MPBarFill->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_MPBarFill->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			const float fillWidth =
				PLAYER_BAR_WIDTH -
				PLAYER_BAR_FILL_OFFSET_X -
				PLAYER_BAR_FILL_RIGHT_MARGIN;

			rect->SetPosition(
				GetPlayerBarX() +
				PLAYER_BAR_FILL_OFFSET_X,

				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y +
				PLAYER_BAR_FILL_OFFSET_Y);

			rect->SetSize(
				fillWidth,
				PLAYER_BAR_FILL_HEIGHT);
		}

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

	//=================================================
	// MP Frame
	//=================================================

	m_MPBarFrame =
		CreateImage(
			"PlayerHUDMPFrame");

	if (m_MPBarFrame)
	{
		auto* rect =
			m_MPBarFrame->
			GetComponent<
			UIRectComponent>();

		auto* image =
			m_MPBarFrame->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/MPBarFrame.png");
		}
	}

	//=================================================
	// AP Label
	//=================================================
	//
	// HUD左下。
	//=================================================

	m_APText =
		CreateText(
			"PlayerHUDAPText");

	if (m_APText)
	{
		auto* rect =
			m_APText->
			GetComponent<
			UIRectComponent>();

		auto* font =
			m_APText->
			GetComponent<
			UIFontComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_AP_OFFSET_Y);

			rect->SetSize(
				30.0f,
				20.0f);
		}

		if (font)
		{
			font->SetText(
				L"AP");

			font->SetFontSize(
				15.0f);
		}
	}

	//=================================================
	// AP Icons
	//=================================================

	CreateAPIcons();
}

void FieldSceneUI::CreatePlayerIcons(const std::vector<GameObject*>& players)
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
			SetTextureToClassUI(players[i], image);

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

void FieldSceneUI::CreateAPIcons()
{
	if (!m_Manager)
	{
		return;
	}

	m_APIcons.clear();

	m_APIcons.reserve(
		AP_ICON_COUNT);

	for (int i = 0;
		i < AP_ICON_COUNT;
		++i)
	{
		GameObject* icon =
			CreateImage(
				"PlayerHUDAPIcon");

		if (!icon)
		{
			continue;
		}

		auto* rect =
			icon->
			GetComponent<
			UIRectComponent>();

		auto* image =
			icon->
			GetComponent<
			UIImageComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetAPIconStartX() +
				static_cast<float>(i) *
				AP_ICON_INTERVAL,

				m_PlayerHUDY +
				65.0f);

			rect->SetSize(
				AP_ICON_SIZE,
				AP_ICON_SIZE);
		}

		if (image)
		{
			image->SetTexture(
				"Assets/Textures/Field/APFeather.png");

			image->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));
		}

		m_APIcons.push_back(
			icon);
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

#ifdef _DEBUG
		DebugUI::RegisterDebugFunction(
			[]()
			{
				DrawPlayerHUDLayoutDebugUI();
			});
#endif

		return;
	}

	UpdatePlayerHUD(
		focusedPlayer);

	//=================================================
	// Temporary Layout Debug UI
	//=================================================
	//
	// UI配置確定後はこのブロックを削除する。
	//=================================================

#ifdef _DEBUG

	DebugUI::RegisterDebugFunction(
		[]()
		{
			DrawPlayerHUDLayoutDebugUI();
		});

#endif
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

	CreatePlayerIcons(players);
}

//=====================================================
// Player Icon Click
//=====================================================

/**
 * @brief このFrameでClickされたPlayerを取得
 *
 * @return
 * ClickされたPlayer。
 * Clickされていなければnullptr。
 */
GameObject*
FieldSceneUI::GetClickedPlayer() const
{
	return m_ClickedPlayer;
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
	for (size_t i = 0;
		i < m_PlayerIcons.size();
		++i)
	{
		FieldPlayerIconUI& iconUI =
			m_PlayerIcons[i];

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

		auto* rect =
			iconUI.Button->
			GetComponent<
			UIRectComponent>();

		if (!button ||
			!image ||
			!rect)
		{
			continue;
		}

		const bool selected =
			iconUI.Player ==
			focusedPlayer;

		//=================================================
		// Click
		//=================================================

		if (button->IsClicked())
		{
			m_ClickedPlayer =
				iconUI.Player;
		}

		//=================================================
		// Size
		//=================================================

		const float size =
			selected
			? PLAYER_ICON_SELECTED_SIZE
			: PLAYER_ICON_NORMAL_SIZE;

		const float baseY =
			PLAYER_ICON_START_Y -
			static_cast<float>(i) *
			PLAYER_ICON_INTERVAL;

		const float offset =
			(
				PLAYER_ICON_SELECTED_SIZE -
				size
				) *
			0.5f;

		const float iconY =
			baseY +
			offset;

		rect->SetPosition(
			PLAYER_ICON_X +
			offset,
			iconY);

		rect->SetSize(
			size,
			size);

		//=================================================
		// Selected HUD Position
		//=================================================
		//
		// 詳細ステータスを
		// 「選択中Iconの真横」に追従させる。
		//=================================================

		if (selected)
		{
			m_PlayerHUDY =
				iconY + PLAYER_HUD_Y_OFFSET;
		}

		//=================================================
		// Color
		//=================================================

		if (selected)
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
					0.60f,
					0.60f,
					0.60f,
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
		player->
		GetComponent<
		CharacterStatusComponent>();

	if (!status)
	{
		HidePlayerHUD();

		return;
	}

	//=================================================
	// Background
	//=================================================

	if (m_PlayerHUDBackground)
	{
		auto* rect =
			m_PlayerHUDBackground->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				PLAYER_HUD_X,
				m_PlayerHUDY);

			rect->SetSize(
				PLAYER_HUD_WIDTH,
				PLAYER_HUD_HEIGHT);
		}

		HideObject(m_PlayerHUDBackground);
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
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_NAME_OFFSET_Y);

			rect->SetSize(
				170.0f,
				24.0f);
		}

		if (font)
		{
			font->SetText(
				GetPlayerName(player));

			font->SetFontSize(
				20.0f);
		}

		HideObject(
			m_PlayerNameText);
	}

	//=================================================
	// Level
	//=================================================

	if (m_PlayerLevelText)
	{
		auto* rect =
			m_PlayerLevelText->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_LEVEL_OFFSET_Y);

			rect->SetSize(
				90.0f,
				20.0f);
		}

		HideObject(
			m_PlayerLevelText);
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

	if (m_HPBarBackground)
	{
		auto* rect =
			m_HPBarBackground->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}
	}

	if (m_HPBarFill)
	{
		auto* rect =
			m_HPBarFill->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			//=========================================
			// Frame内部でFill可能な領域
			//=========================================

			const float usableWidth =
				PLAYER_BAR_WIDTH -
				PLAYER_BAR_FILL_OFFSET_X -
				PLAYER_BAR_FILL_RIGHT_MARGIN;

			const float fillWidth =
				usableWidth *
				hpRate;

			rect->SetPosition(
				GetPlayerBarX() +
				PLAYER_BAR_FILL_OFFSET_X,

				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y +
				PLAYER_BAR_FILL_OFFSET_Y);

			rect->SetSize(
				fillWidth,
				PLAYER_BAR_FILL_HEIGHT);
		}
	}

	if (m_HPBarFrame)
	{
		auto* rect =
			m_HPBarFrame->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_HP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
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

	if (m_MPBarBackground)
	{
		auto* rect =
			m_MPBarBackground->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}
	}

	if (m_MPBarFill)
	{
		auto* rect =
			m_MPBarFill->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			const float usableWidth =
				PLAYER_BAR_WIDTH -
				PLAYER_BAR_FILL_OFFSET_X -
				PLAYER_BAR_FILL_RIGHT_MARGIN;

			const float fillWidth =
				usableWidth *
				mpRate;

			rect->SetPosition(
				GetPlayerBarX() +
				PLAYER_BAR_FILL_OFFSET_X,

				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y +
				PLAYER_BAR_FILL_OFFSET_Y);

			rect->SetSize(
				fillWidth,
				PLAYER_BAR_FILL_HEIGHT);
		}
	}

	if (m_MPBarFrame)
	{
		auto* rect =
			m_MPBarFrame->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerBarX(),
				m_PlayerHUDY +
				PLAYER_MP_BAR_OFFSET_Y);

			rect->SetSize(
				PLAYER_BAR_WIDTH,
				PLAYER_BAR_HEIGHT);
		}
	}

	//=================================================
	// AP
	//=================================================

	const int ap =
		status->GetAP();

	UpdateAPDisplay(
		ap);

	//=================================================
	// Out Of AP
	//=================================================

	SetPlayerHUDDarkened(
		ap <= 0);
}

void FieldSceneUI::UpdateAPDisplay(
	int currentAP)
{
	//=================================================
	// AP Label
	//=================================================

	if (m_APText)
	{
		auto* rect =
			m_APText->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				GetPlayerInfoX(),
				m_PlayerHUDY +
				PLAYER_AP_OFFSET_Y);

			rect->SetSize(
				30.0f,
				20.0f);
		}

		// AP文字を使用しない場合は
		// このHideObject()をそのまま残す。
		HideObject(
			m_APText);
	}

	//=================================================
	// AP State Calculation
	//=================================================
	//
	// 例:
	//
	// 最大AP = 6
	// 現在AP = 4
	//
	// [0][1][2][3][4][5]
	//  ×  ×  ◆  ◆  ◆  ◆
	//
	// 左2個が既に消費されたAP。
	//=================================================

	const int maxAP =
		AP_ICON_COUNT;

	const int safeCurrentAP =
		(currentAP < 0)
		? 0
		: (currentAP > maxAP)
		? maxAP
		: currentAP;

	/**
	 * @brief 左側から何個消費済みか
	 */
	const int consumedCount =
		maxAP -
		safeCurrentAP;

	/**
	 * @brief Preview可能なAP数
	 *
	 * 現在APより多くPreviewしない。
	 */
	const int previewCount =
		(m_APPreviewCost < safeCurrentAP)
		? m_APPreviewCost
		: safeCurrentAP;

	/**
	 * @brief Preview終了Index
	 *
	 * [consumedCount, previewEnd)
	 * が消費予定AP。
	 */
	const int previewEnd =
		consumedCount +
		previewCount;

	//=================================================
	// AP Icons
	//=================================================

	for (size_t i = 0;
		i < m_APIcons.size();
		++i)
	{
		GameObject* icon =
			m_APIcons[i];

		if (!icon)
		{
			continue;
		}

		auto* rect =
			icon->
			GetComponent<
			UIRectComponent>();

		auto* image =
			icon->
			GetComponent<
			UIImageComponent>();

		if (!rect ||
			!image)
		{
			continue;
		}

		const int index =
			static_cast<int>(i);

		//=============================================
		// ① Consumed AP
		//=============================================
		//
		// 既に消費したAP。
		//
		// 完全に見えない状態にする。
		//=============================================

		if (index <
			consumedCount)
		{
			rect->SetSize(
				0.0f,
				0.0f);

			continue;
		}

		//=============================================
		// ② Preview AP
		//=============================================

		const bool preview =
			m_APPreviewCost > 0 &&
			index >= consumedCount &&
			index < previewEnd;

		if (preview)
		{
			//=========================================
			// Preview Size
			//=========================================
			//
			// 通常より少し大きくして
			// 消費予定であることを強調する。
			//=========================================

			const float previewScale =
				1.20f;

			const float previewSize =
				AP_ICON_SIZE *
				previewScale;

			// 拡大しても中心が大きくズレないよう補正。
			const float sizeOffset =
				(
					previewSize -
					AP_ICON_SIZE
					) *
				0.5f;

			rect->SetPosition(
				GetAPIconStartX() +
				static_cast<float>(i) *
				AP_ICON_INTERVAL -
				sizeOffset,

				m_PlayerHUDY +
				PLAYER_AP_OFFSET_Y -
				sizeOffset);

			rect->SetSize(
				previewSize,
				previewSize);

			//=========================================
			// Preview Color
			//=========================================
			//
			// 半透明にして
			// 「まだ消費されてはいない」
			// ことを表現する。
			//=========================================

			image->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					0.45f));

			continue;
		}

		//=============================================
		// ③ Remaining AP
		//=============================================
		//
		// 通常使用可能なAP。
		//=============================================

		rect->SetPosition(
			GetAPIconStartX() +
			static_cast<float>(i) *
			AP_ICON_INTERVAL,

			m_PlayerHUDY +
			PLAYER_AP_OFFSET_Y);

		rect->SetSize(
			AP_ICON_SIZE,
			AP_ICON_SIZE);

		image->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}
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
	// Neutral Images
	//=================================================

	GameObject* neutralImages[] =
	{
		m_PlayerHUDBackground,

		m_HPBarBackground,
		m_HPBarFrame,

		m_MPBarBackground,
		m_MPBarFrame
	};

	for (GameObject* object :
		neutralImages)
	{
		if (!object)
		{
			continue;
		}

		auto* image =
			object->
			GetComponent<
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
	// HP Fill
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
					0.85f *
					brightness,

					0.15f *
					brightness,

					0.15f *
					brightness,

					1.0f));
		}
	}

	//=================================================
	// MP Fill
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
					0.15f *
					brightness,

					0.35f *
					brightness,

					0.90f *
					brightness,

					1.0f));
		}
	}

	//=================================================
	// Text
	//=================================================

	GameObject* texts[] =
	{
		m_PlayerNameText,
		m_PlayerLevelText,
		m_APText
	};

	for (GameObject* object :
		texts)
	{
		if (!object)
		{
			continue;
		}

		auto* font =
			object->
			GetComponent<
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

		m_PlayerNameText,
		m_PlayerLevelText,

		m_HPBarBackground,
		m_HPBarFill,
		m_HPBarFrame,

		m_MPBarBackground,
		m_MPBarFill,
		m_MPBarFrame,

		m_APText
	};

	for (GameObject* object :
		objects)
	{
		HideObject(
			object);
	}

	// AP Icon
	for (GameObject* icon :
		m_APIcons)
	{
		HideObject(
			icon);
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
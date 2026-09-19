#include "TitleScene.h"

// Component
#include "UIRectComponent.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"

#include "SceneManager.h"
#include "SpriteRenderer.h"
#include "Window.h"

#include <Windows.h>

//=====================================================
// Initialize
//=====================================================

void TitleScene::Initialize()
{
	//=================================================
	// 背景を先に生成
	//=================================================
	CreateBackground();

	//=================================================
	// START Button
	//=================================================
	CreateStartButton();

	//=================================================
	// EXIT Button
	//=================================================
	CreateExitButton();
}

//=====================================================
// Finalize
//=====================================================

void TitleScene::Finalize()
{
	m_Background = nullptr;
	m_StartButton = nullptr;
	m_ExitButton = nullptr;

	// GameObjectの終了処理
	m_GameObjectManager.Clear();
}

//=====================================================
// Update
//=====================================================

void TitleScene::Update(uint64_t delta)
{
	m_GameObjectManager.Update(delta);

	//=================================================
	// START Button Visual
	//=================================================
	UpdateButtonVisual(m_StartButton);

	//=================================================
	// EXIT Button Visual
	//=================================================
	UpdateButtonVisual(m_ExitButton);

	//=================================================
	// START Click
	//=================================================
	if (m_StartButton)
	{
		auto* startButton =
			m_StartButton->GetComponent<UIButtonComponent>();

		if (startButton &&
			startButton->IsClicked())
		{
			SceneManager::SetCurrentScene(
				"TownScene");

			return;
		}
	}

	//=================================================
	// EXIT Click
	//=================================================
	if (m_ExitButton)
	{
		auto* exitButton =
			m_ExitButton->GetComponent<UIButtonComponent>();

		if (exitButton &&
			exitButton->IsClicked())
		{
			// アプリ終了
			::PostQuitMessage(0);
			return;
		}
	}
}

//=====================================================
// Draw
//=====================================================

void TitleScene::Draw(uint64_t delta)
{
	(void)delta;

	SpriteRenderer::Begin();

	m_GameObjectManager.Draw();

	SpriteRenderer::End();
}

//=====================================================
// Button Visual
//=====================================================

void TitleScene::UpdateButtonVisual(
	GameObject* buttonObject)
{
	if (!buttonObject)
	{
		return;
	}

	auto* button =
		buttonObject->GetComponent<UIButtonComponent>();

	auto* image =
		buttonObject->GetComponent<UIImageComponent>();

	if (!button ||
		!image)
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
// Create Background
//=====================================================

void TitleScene::CreateBackground()
{
	m_Background =
		m_GameObjectManager.Create<GameObject>();

	if (!m_Background)
	{
		return;
	}

	m_Background->SetName(
		"TitleBackground");

	//====================
	// UI Rect
	//====================
	auto* rect =
		m_Background->AddComponent<UIRectComponent>();

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

	//====================
	// Image
	//====================
	auto* image =
		m_Background->AddComponent<UIImageComponent>();

	if (image)
	{
		image->SetTexture(
			"Assets/Textures/Title/TitleBackground.png");
	}
}

//=====================================================
// Create Start Button
//=====================================================

void TitleScene::CreateStartButton()
{
	m_StartButton =
		m_GameObjectManager.Create<GameObject>();

	if (!m_StartButton)
	{
		return;
	}

	m_StartButton->SetName(
		"StartButton");

	//====================
	// UI Rect
	//====================
	auto* rect =
		m_StartButton->AddComponent<UIRectComponent>();

	if (rect)
	{
		const float buttonWidth =
			400.0f;

		const float buttonHeight =
			100.0f;

		// 画面中央より少し下
		const float x =
			(
				static_cast<float>(
					Window::GetWidth()) -
				buttonWidth
				) * 0.5f;

		const float y =
			static_cast<float>(
				Window::GetHeight()) *
			0.62f;

		rect->SetPosition(
			x,
			y);

		rect->SetSize(
			buttonWidth,
			buttonHeight);
	}

	//====================
	// Image
	//====================
	auto* image =
		m_StartButton->AddComponent<UIImageComponent>();

	if (image)
	{
		image->SetTexture(
			"Assets/Textures/Title/NEWGAME.png");
	}

	//====================
	// Button
	//====================
	m_StartButton->AddComponent<UIButtonComponent>();
}

//=====================================================
// Create Exit Button
//=====================================================

void TitleScene::CreateExitButton()
{
	m_ExitButton =
		m_GameObjectManager.Create<GameObject>();

	if (!m_ExitButton)
	{
		return;
	}

	m_ExitButton->SetName(
		"ExitButton");

	//====================
	// UI Rect
	//====================
	auto* rect =
		m_ExitButton->AddComponent<UIRectComponent>();

	if (rect)
	{
		const float buttonWidth =
			400.0f;

		const float buttonHeight =
			100.0f;

		// STARTボタンの少し下
		const float x =
			(
				static_cast<float>(
					Window::GetWidth()) -
				buttonWidth
				) * 0.5f;

		const float y =
			static_cast<float>(
				Window::GetHeight()) *
			0.78f;

		rect->SetPosition(
			x,
			y);

		rect->SetSize(
			buttonWidth,
			buttonHeight);
	}

	//====================
	// Image
	//====================
	auto* image =
		m_ExitButton->AddComponent<UIImageComponent>();

	if (image)
	{
		image->SetTexture(
			"Assets/Textures/Title/QUITGAME.png");
	}

	//====================
	// Button
	//====================
	m_ExitButton->AddComponent<UIButtonComponent>();
}
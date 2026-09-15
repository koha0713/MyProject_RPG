#include "InputManager.h"

#include <algorithm>

bool InputManager::Initialize(
	HWND hWnd)
{
	if (!hWnd)
	{
		return false;
	}

	m_hWnd =
		hWnd;

	//====================
	// Keyboard
	//====================

	m_CurrentKeys.fill(
		false);

	m_PreviousKeys.fill(
		false);

	//====================
	// Mouse Button
	//====================

	m_CurrentMouseButtons.fill(
		false);

	m_PreviousMouseButtons.fill(
		false);

	//====================
	// Mouse Position
	//====================

	POINT mousePoint{};

	if (::GetCursorPos(
		&mousePoint))
	{
		::ScreenToClient(
			m_hWnd,
			&mousePoint);

		m_MousePosition =
			Vector2(
				static_cast<float>(
					mousePoint.x),
				static_cast<float>(
					mousePoint.y));

		m_PreviousMousePosition =
			m_MousePosition;
	}

	m_MouseDelta =
		Vector2(
			0.0f,
			0.0f);

	m_MouseWheel =
		0.0f;

	m_PendingMouseWheel =
		0.0f;

	m_Initialized =
		true;

	return true;
}

void InputManager::Finalize()
{
	m_CurrentKeys.fill(
		false);

	m_PreviousKeys.fill(
		false);

	m_CurrentMouseButtons.fill(
		false);

	m_PreviousMouseButtons.fill(
		false);

	m_MousePosition =
		Vector2(
			0.0f,
			0.0f);

	m_PreviousMousePosition =
		Vector2(
			0.0f,
			0.0f);

	m_MouseDelta =
		Vector2(
			0.0f,
			0.0f);

	m_MouseWheel =
		0.0f;

	m_PendingMouseWheel =
		0.0f;

	m_hWnd =
		nullptr;

	m_Initialized =
		false;
}

void InputManager::Update()
{
	if (!m_Initialized ||
		!m_hWnd)
	{
		return;
	}

	//=================================================
	// Windowが非アクティブの場合
	//=================================================
	//
	// GetAsyncKeyStateはゲームWindow外の入力も取得できるため、
	// 非アクティブ時は入力を無効化する。
	//

	const bool isActive =
		(::GetForegroundWindow() ==
			m_hWnd);

	//=================================================
	// Keyboard
	//=================================================

	m_PreviousKeys =
		m_CurrentKeys;

	if (isActive)
	{
		for (size_t i = 0;
			i < KEY_COUNT;
			++i)
		{
			m_CurrentKeys[i] =
				(::GetAsyncKeyState(
					static_cast<int>(i)) &
					0x8000) != 0;
		}
	}
	else
	{
		m_CurrentKeys.fill(
			false);
	}

	//=================================================
	// Mouse Button
	//=================================================

	m_PreviousMouseButtons =
		m_CurrentMouseButtons;

	for (size_t i = 0;
		i < MOUSE_BUTTON_COUNT;
		++i)
	{
		if (!isActive)
		{
			m_CurrentMouseButtons[i] =
				false;

			continue;
		}

		const auto button =
			static_cast<MouseButton>(i);

		const int virtualKey =
			GetMouseVirtualKey(
				button);

		m_CurrentMouseButtons[i] =
			(::GetAsyncKeyState(
				virtualKey) &
				0x8000) != 0;
	}

	//=================================================
	// Mouse Position
	//=================================================

	m_PreviousMousePosition =
		m_MousePosition;

	if (isActive)
	{
		POINT mousePoint{};

		if (::GetCursorPos(
			&mousePoint))
		{
			// Screen座標からClient座標へ変換
			::ScreenToClient(
				m_hWnd,
				&mousePoint);

			m_MousePosition =
				Vector2(
					static_cast<float>(
						mousePoint.x),
					static_cast<float>(
						mousePoint.y));
		}

		m_MouseDelta =
			Vector2(
				m_MousePosition.x -
				m_PreviousMousePosition.x,

				m_MousePosition.y -
				m_PreviousMousePosition.y);
	}
	else
	{
		m_MouseDelta =
			Vector2(
				0.0f,
				0.0f);
	}

	//=================================================
	// Mouse Wheel
	//=================================================
	//
	// WindowProcで取得した入力を今フレーム用へ移す。
	//

	m_MouseWheel =
		m_PendingMouseWheel;

	m_PendingMouseWheel =
		0.0f;
}

//=====================================================
// Keyboard
//=====================================================

bool InputManager::IsKeyDown(
	KeyCode key) const
{
	const size_t index =
		static_cast<size_t>(
			key);

	if (index >= KEY_COUNT)
	{
		return false;
	}

	return m_CurrentKeys[index];
}

bool InputManager::IsKeyPressed(
	KeyCode key) const
{
	const size_t index =
		static_cast<size_t>(
			key);

	if (index >= KEY_COUNT)
	{
		return false;
	}

	return
		m_CurrentKeys[index] &&
		!m_PreviousKeys[index];
}

bool InputManager::IsKeyReleased(
	KeyCode key) const
{
	const size_t index =
		static_cast<size_t>(
			key);

	if (index >= KEY_COUNT)
	{
		return false;
	}

	return
		!m_CurrentKeys[index] &&
		m_PreviousKeys[index];
}

//=====================================================
// Mouse
//=====================================================

bool InputManager::IsMouseDown(
	MouseButton button) const
{
	const size_t index =
		static_cast<size_t>(
			button);

	if (index >=
		MOUSE_BUTTON_COUNT)
	{
		return false;
	}

	return
		m_CurrentMouseButtons[index];
}

bool InputManager::IsMousePressed(
	MouseButton button) const
{
	const size_t index =
		static_cast<size_t>(
			button);

	if (index >=
		MOUSE_BUTTON_COUNT)
	{
		return false;
	}

	return
		m_CurrentMouseButtons[index] &&
		!m_PreviousMouseButtons[index];
}

bool InputManager::IsMouseReleased(
	MouseButton button) const
{
	const size_t index =
		static_cast<size_t>(
			button);

	if (index >=
		MOUSE_BUTTON_COUNT)
	{
		return false;
	}

	return
		!m_CurrentMouseButtons[index] &&
		m_PreviousMouseButtons[index];
}

//=====================================================
// Mouse Wheel
//=====================================================

void InputManager::OnMouseWheel(
	short wheelDelta)
{
	// Windows標準では
	// WHEEL_DELTA = 120 が1段階
	m_PendingMouseWheel +=
		static_cast<float>(
			wheelDelta) /
		static_cast<float>(
			WHEEL_DELTA);
}

//=====================================================
// Mouse Button → VK
//=====================================================

int InputManager::GetMouseVirtualKey(
	MouseButton button)
{
	switch (button)
	{
	case MouseButton::Left:
		return VK_LBUTTON;

	case MouseButton::Right:
		return VK_RBUTTON;

	case MouseButton::Middle:
		return VK_MBUTTON;

	default:
		break;
	}

	return 0;
}
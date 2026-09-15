#pragma once

/**
 * @file InputManager.h
 * @brief Keyboard / Mouse入力を一元管理する
 */

#include <Windows.h>

#include <array>
#include <cstdint>

#include "CommonType.h"
#include "NonCopyable.h"

 /**
  * @brief Keyboard Key
  *
  * @details
  * Win32 Virtual-Key Codeと対応させる。
  * 必要なキーは今後追加可能。
  */
enum class KeyCode : uint16_t
{
	//====================
	// Alphabet
	//====================

	A = 'A',
	B = 'B',
	C = 'C',
	D = 'D',
	E = 'E',
	F = 'F',
	G = 'G',
	H = 'H',
	I = 'I',
	J = 'J',
	K = 'K',
	L = 'L',
	M = 'M',
	N = 'N',
	O = 'O',
	P = 'P',
	Q = 'Q',
	R = 'R',
	S = 'S',
	T = 'T',
	U = 'U',
	V = 'V',
	W = 'W',
	X = 'X',
	Y = 'Y',
	Z = 'Z',

	//====================
	// Number
	//====================

	Num0 = '0',
	Num1 = '1',
	Num2 = '2',
	Num3 = '3',
	Num4 = '4',
	Num5 = '5',
	Num6 = '6',
	Num7 = '7',
	Num8 = '8',
	Num9 = '9',

	//====================
	// Special
	//====================

	Space = VK_SPACE,
	Enter = VK_RETURN,
	Escape = VK_ESCAPE,

	Tab = VK_TAB,
	Shift = VK_SHIFT,
	Control = VK_CONTROL,

	BackSpace = VK_BACK,

	//====================
	// Arrow
	//====================

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,

	//====================
	// Function
	//====================

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F4 = VK_F4,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12
};


/**
 * @brief Mouse Button
 */
enum class MouseButton : uint8_t
{
	Left = 0,
	Right,
	Middle,

	Count
};


/**
 * @brief Keyboard / Mouse入力管理
 *
 * @details
 * 前フレームと現在フレームの入力状態を保持することで、
 *
 * Down
 * Pressed
 * Released
 *
 * を判定する。
 *
 * SceneやComponentからINPUT_MANAGERを通して利用する。
 */
class InputManager :
	public NonCopyable
{
public:

	/**
	 * @brief Singleton取得
	 */
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	//====================
	// Lifecycle
	//====================

	/**
	 * @brief 初期化
	 *
	 * @param hWnd ゲームWindow Handle
	 */
	bool Initialize(
		HWND hWnd);

	/**
	 * @brief 終了処理
	 */
	void Finalize();

	/**
	 * @brief 入力状態更新
	 *
	 * @details
	 * Scene::Update()より前に
	 * 1フレームに1回呼ぶ。
	 */
	void Update();

	//====================
	// Keyboard
	//====================

	/**
	 * @brief キーを押しているか
	 */
	bool IsKeyDown(
		KeyCode key) const;

	/**
	 * @brief キーを押した瞬間か
	 *
	 * @return 押した最初の1フレームのみtrue
	 */
	bool IsKeyPressed(
		KeyCode key) const;

	/**
	 * @brief キーを離した瞬間か
	 *
	 * @return 離した最初の1フレームのみtrue
	 */
	bool IsKeyReleased(
		KeyCode key) const;

	//====================
	// Mouse Button
	//====================

	bool IsMouseDown(
		MouseButton button) const;

	bool IsMousePressed(
		MouseButton button) const;

	bool IsMouseReleased(
		MouseButton button) const;

	//====================
	// Mouse Position
	//====================

	/**
	 * @brief Client領域上のMouse座標
	 */
	const Vector2&
		GetMousePosition() const
	{
		return m_MousePosition;
	}

	/**
	 * @brief 前フレームからのMouse移動量
	 */
	const Vector2&
		GetMouseDelta() const
	{
		return m_MouseDelta;
	}

	/**
	 * @brief Mouse Wheel量
	 *
	 * @details
	 * +1 : 上方向
	 * -1 : 下方向
	 */
	float GetMouseWheel() const
	{
		return m_MouseWheel;
	}

	//====================
	// Window Message
	//====================

	/**
	 * @brief WM_MOUSEWHEELからWheel入力を受け取る
	 *
	 * @param wheelDelta GET_WHEEL_DELTA_WPARAMの値
	 */
	void OnMouseWheel(
		short wheelDelta);

private:

	InputManager() = default;
	~InputManager() = default;

	/**
	 * @brief MouseButtonからVirtual-Key Codeを取得
	 */
	static int GetMouseVirtualKey(
		MouseButton button);

private:

	static constexpr size_t
		KEY_COUNT = 256;

	//====================
	// Keyboard
	//====================

	std::array<bool, KEY_COUNT>
		m_CurrentKeys{};

	std::array<bool, KEY_COUNT>
		m_PreviousKeys{};

	//====================
	// Mouse
	//====================

	static constexpr size_t
		MOUSE_BUTTON_COUNT =
		static_cast<size_t>(
			MouseButton::Count);

	std::array<bool, MOUSE_BUTTON_COUNT>
		m_CurrentMouseButtons{};

	std::array<bool, MOUSE_BUTTON_COUNT>
		m_PreviousMouseButtons{};

	Vector2 m_MousePosition =
		Vector2(
			0.0f,
			0.0f);

	Vector2 m_PreviousMousePosition =
		Vector2(
			0.0f,
			0.0f);

	Vector2 m_MouseDelta =
		Vector2(
			0.0f,
			0.0f);

	/**
	 * @brief 今フレームでSceneが参照するWheel量
	 */
	float m_MouseWheel = 0.0f;

	/**
	 * @brief WindowProcから蓄積されるWheel入力
	 *
	 * @details
	 * Message処理とUpdate順序による入力消失を
	 * 防止するためPendingとして分離する。
	 */
	float m_PendingMouseWheel = 0.0f;

	//====================
	// Window
	//====================

	HWND m_hWnd = nullptr;

	bool m_Initialized = false;
};


/**
 * @brief InputManager簡易アクセス
 */
#define INPUT_MANAGER \
	InputManager::GetInstance()
#pragma once

/**
 * @file UIButtonComponent.h
 * @brief Mouse操作可能なButton Component
 */

#include "Component.h"

 /**
  * @brief UI Buttonの状態
  */
enum class UIButtonState
{
	Normal,
	Hovered,
	Pressed
};

/**
 * @brief Mouse Button入力を管理するUI Component
 */
class UIButtonComponent :
	public Component
{
public:

	UIButtonComponent() = default;
	~UIButtonComponent() override = default;

	void Initialize() override;
	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override {}

	/**
	 * @brief このFrameでButtonがClickされたか
	 */
	bool IsClicked() const
	{
		return m_Clicked;
	}

	bool IsHovered() const
	{
		return
			m_State ==
			UIButtonState::Hovered ||
			m_State ==
			UIButtonState::Pressed;
	}

	bool IsPressed() const
	{
		return
			m_State ==
			UIButtonState::Pressed;
	}

	UIButtonState GetState() const
	{
		return m_State;
	}

	const char* GetComponentName() const override
	{
		return "UIButtonComponent";
	}

	/**
	 * @brief Button入力の有効/無効を設定する
	 */
	void SetEnabled(
		bool enabled)
	{
		m_Enabled = enabled;

		// 無効化された場合は
		// Buttonの内部状態も初期化する。
		if (!m_Enabled)
		{
			m_State =
				UIButtonState::Normal;

			m_PressedInside =
				false;

			m_Clicked =
				false;
		}
	}

	bool IsEnabled() const
	{
		return m_Enabled;
	}

private:

	UIButtonState m_State =
		UIButtonState::Normal;

	/**
	 * @brief Press開始時にButton上だったか
	 */
	bool m_PressedInside =
		false;

	/**
	 * @brief このFrameでClick成立したか
	 */
	bool m_Clicked =
		false;

	/**
	 * @brief Buttonが入力を受け付けるか
	 */
	bool m_Enabled =
		true;
};
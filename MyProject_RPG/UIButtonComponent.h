#pragma once

/**
 * @file UIButtonComponent.h
 * @brief Mouse‘€ì‰Â”\‚ÈButton Component
 */

#include "Component.h"

 /**
  * @brief UI Button‚Ìó‘Ô
  */
enum class UIButtonState
{
	Normal,
	Hovered,
	Pressed
};

/**
 * @brief Mouse Button“ü—Í‚ğŠÇ—‚·‚éUI Component
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
	 * @brief ‚±‚ÌFrame‚ÅButton‚ªClick‚³‚ê‚½‚©
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

private:

	UIButtonState m_State =
		UIButtonState::Normal;

	/**
	 * @brief PressŠJn‚ÉButtonã‚¾‚Á‚½‚©
	 */
	bool m_PressedInside =
		false;

	/**
	 * @brief ‚±‚ÌFrame‚ÅClick¬—§‚µ‚½‚©
	 */
	bool m_Clicked =
		false;
};
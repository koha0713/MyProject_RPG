#include "UIButtonComponent.h"

#include "GameObject.h"
#include "UIRectComponent.h"
#include "InputManager.h"

//=====================================================
// Initialize
//=====================================================

void UIButtonComponent::Initialize()
{
	m_State =
		UIButtonState::Normal;

	m_PressedInside =
		false;

	m_Clicked =
		false;
}

//=====================================================
// Finalize
//=====================================================

void UIButtonComponent::Finalize()
{
}

//=====================================================
// Update
//=====================================================

void UIButtonComponent::Update(
	uint64_t delta)
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	auto* rect =
		owner->GetComponent<
		UIRectComponent>();

	if (!rect)
	{
		return;
	}

	// Clickは1Frameだけtrueにする。
	m_Clicked =
		false;

	//=================================================
	// Mouse Position
	//=================================================

	const auto mousePosition =
		INPUT_MANAGER.GetMousePosition();

	const Vector2 mouse
	{
		static_cast<float>(
			mousePosition.x),

		static_cast<float>(
			mousePosition.y)
	};

	const bool hovered =
		rect->Contains(
			mouse);

	//=================================================
	// Mouse Press
	//=================================================

	if (INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		if (hovered)
		{
			m_State =
				UIButtonState::Pressed;

			m_PressedInside =
				true;
		}
		else
		{
			m_PressedInside =
				false;
		}
	}

	//=================================================
	// Mouse Down
	//=================================================

	if (INPUT_MANAGER.IsMouseDown(
		MouseButton::Left))
	{
		if (m_PressedInside)
		{
			m_State =
				UIButtonState::Pressed;
		}

		return;
	}

	//=================================================
	// Mouse Release
	//=================================================

	if (INPUT_MANAGER.IsMouseReleased(
		MouseButton::Left))
	{
		// Button上で押し始め、
		// Button上で離した場合のみClick成立。
		if (m_PressedInside &&
			hovered)
		{
			m_Clicked =
				true;
		}

		m_PressedInside =
			false;
	}

	//=================================================
	// Normal / Hover
	//=================================================

	if (hovered)
	{
		m_State =
			UIButtonState::Hovered;
	}
	else
	{
		m_State =
			UIButtonState::Normal;
	}
}
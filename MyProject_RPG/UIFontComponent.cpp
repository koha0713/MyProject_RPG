/**
 * @file UIFontComponent.cpp
 */

#include "UIFontComponent.h"

#include "FontRenderer.h"
#include "GameObject.h"
#include "UIRectComponent.h"

#include <imgui/imgui.h>

 //=====================================================
 // Initialize
 //=====================================================

void UIFontComponent::Initialize()
{
}

//=====================================================
// Finalize
//=====================================================

void UIFontComponent::Finalize()
{
}

//=====================================================
// Update
//=====================================================

void UIFontComponent::Update(
	uint64_t delta)
{
	(void)delta;
}

//=====================================================
// Draw
//=====================================================

void UIFontComponent::Draw()
{
	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	//=================================================
	// UIRect
	//=================================================

	auto* rect =
		owner->GetComponent<
		UIRectComponent>();

	if (!rect)
	{
		return;
	}

	//=================================================
	// Font Draw
	//=================================================

	FontRenderer::DrawText(
		m_Text,
		m_FontSize,
		rect->GetPosition(),
		rect->GetSize(),
		m_Color);
}

//=====================================================
// DebugUI
//=====================================================

void UIFontComponent::DrawDebugUI()
{
	ImGui::PushID(
		this);

	//=================================================
	// Font Size
	//=================================================

	float fontSize =
		m_FontSize;

	if (ImGui::DragFloat(
		"Font Size",
		&fontSize,
		1.0f,
		1.0f,
		200.0f))
	{
		SetFontSize(
			fontSize);
	}

	//=================================================
	// Color
	//=================================================

	float color[4] =
	{
		m_Color.x,
		m_Color.y,
		m_Color.z,
		m_Color.w
	};

	if (ImGui::ColorEdit4(
		"Font Color",
		color))
	{
		m_Color =
			Color(
				color[0],
				color[1],
				color[2],
				color[3]);
	}

	ImGui::PopID();
}
#include "UIImageComponent.h"

#include "GameObject.h"
#include "UIRectComponent.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Texture.h"

//=====================================================
// Initialize
//=====================================================

void UIImageComponent::Initialize()
{
	m_Color =
		Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
}

//=====================================================
// Finalize
//=====================================================

void UIImageComponent::Finalize()
{
	// ResourceManagerと他Componentから共有されているため、
	// 自分の参照だけを解放する。
	m_Texture.reset();
}

//=====================================================
// Update
//=====================================================

void UIImageComponent::Update(
	uint64_t delta)
{
}

//=====================================================
// Draw
//=====================================================

void UIImageComponent::Draw()
{
	GameObject* owner =
		GetOwner();

	if (!owner ||
		!m_Texture)
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

	SpriteRenderer::Draw(
		m_Texture->GetShaderResourceView(),
		rect->GetPosition(),
		rect->GetSize(),
		m_Color);
}

//=====================================================
// SetTexture
//=====================================================

bool UIImageComponent::SetTexture(
	const std::string& filePath)
{
	m_Texture =
		RESOURCE_MANAGER.LoadTexture(
			filePath);

	if (!m_Texture)
	{
		OutputDebugStringA(
			"[UIImageComponent] "
			"Failed to load texture: ");

		OutputDebugStringA(
			filePath.c_str());

		OutputDebugStringA(
			"\n");

		return false;
	}

	return true;
}
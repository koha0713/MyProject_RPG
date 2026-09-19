#pragma once

/**
 * @file UIImageComponent.h
 * @brief Screen空間へTextureを描画するUI Component
 */

#include <string>
#include <memory>

#include <d3d11.h>

#include "Component.h"
#include "CommonType.h"

class Texture;

 /**
  * @brief UI Image Component
  *
  * @details
  * UIRectComponentのPosition / Sizeを利用して、
  * TextureをScreen空間へ描画する。
  */
class UIImageComponent :
	public Component
{
public:

	UIImageComponent() = default;
	~UIImageComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//====================
	// Texture
	//====================

	/**
	 * @brief 表示するTextureを読み込む
	 *
	 * @return 成功ならtrue
	 */
	bool SetTexture(
		const std::string& filePath);

	//====================
	// Color
	//====================

	void SetColor(
		const Color& color)
	{
		m_Color =
			color;
	}

	const Color& GetColor() const
	{
		return m_Color;
	}

	const char* GetComponentName() const override
	{
		return "UIImageComponent";
	}

private:
	/**
	 * @brief ResourceManagerから共有取得するTexture
	 */
	std::shared_ptr<Texture>
		m_Texture;

	/**
	 * @brief Textureへの乗算Color
	 */
	Color m_Color =
		Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
};
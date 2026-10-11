#pragma once

/**
 * @file UIFontComponent.h
 * @brief UI文字描画Component
 */

#include <string>

#include "Component.h"
#include "CommonType.h"

 /**
  * @brief UI文字描画Component
  *
  * @details
  * 描画位置・領域はUIRectComponent、
  * FontResourceはFontRendererが管理する。
  *
  * このComponentでは
  * 表示文字列・Fontサイズ・文字色のみを保持する。
  */
class UIFontComponent :
	public Component
{
public:

	UIFontComponent() = default;
	~UIFontComponent() override = default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	//=================================================
	// Text
	//=================================================

	void SetText(
		const std::wstring& text)
	{
		m_Text =
			text;
	}

	const std::wstring& GetText() const
	{
		return m_Text;
	}

	//=================================================
	// Font
	//=================================================

	void SetFontSize(
		float fontSize)
	{
		if (fontSize <=
			0.0f)
		{
			return;
		}

		m_FontSize =
			fontSize;
	}

	float GetFontSize() const
	{
		return m_FontSize;
	}

	//=================================================
	// Color
	//=================================================

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

	//=================================================
	// Debug
	//=================================================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "UIFontComponent";
	}

private:

	/**
	 * @brief 表示文字列
	 */
	std::wstring m_Text =
		L"Text";

	/**
	 * @brief Fontサイズ
	 */
	float m_FontSize =
		32.0f;

	/**
	 * @brief 文字色
	 */
	Color m_Color =
		Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
};
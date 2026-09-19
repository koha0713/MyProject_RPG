#pragma once

/**
 * @file UIRectComponent.h
 * @brief Screen UIの矩形領域を管理するComponent
 */

#include "Component.h"
#include "CommonType.h"

 /**
  * @brief UI矩形Component
  *
  * @details
  * Positionは左上座標、
  * SizeはPixel単位で管理する。
  */
class UIRectComponent :
	public Component
{
public:

	UIRectComponent() = default;
	~UIRectComponent() override = default;

	void Initialize() override {}
	void Finalize() override {}
	void Update(uint64_t delta) override {}
	void Draw() override {}

	//====================
	// Position
	//====================

	void SetPosition(
		float x,
		float y)
	{
		m_Position =
			Vector2(x, y);
	}

	void SetPosition(
		const Vector2& position)
	{
		m_Position =
			position;
	}

	const Vector2& GetPosition() const
	{
		return m_Position;
	}

	//====================
	// Size
	//====================

	void SetSize(
		float width,
		float height)
	{
		m_Size =
			Vector2(
				width,
				height);
	}

	void SetSize(
		const Vector2& size)
	{
		m_Size =
			size;
	}

	const Vector2& GetSize() const
	{
		return m_Size;
	}

	//====================
	// Hit Test
	//====================

	/**
	 * @brief 指定Screen座標がUI矩形内か判定する
	 */
	bool Contains(
		const Vector2& point) const
	{
		return
			point.x >= m_Position.x &&
			point.x <=
			m_Position.x + m_Size.x &&
			point.y >= m_Position.y &&
			point.y <=
			m_Position.y + m_Size.y;
	}

	const char* GetComponentName() const override
	{
		return "UIRectComponent";
	}

private:

	Vector2 m_Position =
		Vector2(
			0.0f,
			0.0f);

	Vector2 m_Size =
		Vector2(
			0.0f,
			0.0f);
};
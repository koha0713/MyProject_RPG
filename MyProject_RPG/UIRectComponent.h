#pragma once

/**
 * @file UIRectComponent.h
 * @brief Screen UIの矩形領域を管理するComponent
 */

#include "Component.h"
#include "CommonType.h"
#include "DebugUI.h"

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

	void DrawDebugUI() override
	{
		//=====================================
		// Position
		//=====================================

		float position[2] =
		{
			m_Position.x,
			m_Position.y
		};

		if (ImGui::DragFloat2(
			"Position",
			position,
			1.0f))
		{
			SetPosition(
				position[0],
				position[1]);
		}

		//=====================================
		// Size
		//=====================================

		float size[2] =
		{
			m_Size.x,
			m_Size.y
		};

		if (ImGui::DragFloat2(
			"Size",
			size,
			1.0f,
			1.0f,
			2000.0f))
		{
			SetSize(
				size[0],
				size[1]);
		}
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
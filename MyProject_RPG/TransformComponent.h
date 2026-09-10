#pragma once

#include "Component.h"
#include "CommonType.h"

/**
 * @brief GameObjectのTransformを管理するComponent
 *
 * @details
 * Position / Rotation / Scaleを保持し、
 * 描画に使用するWorldMatrixを生成する。
 *
 * Rotationはラジアン単位。
 */
class TransformComponent : public Component
{
public:

	TransformComponent() = default;
	~TransformComponent() override = default;

	void Update() override;

	//====================
	// Position
	//====================

	void SetPosition(const Vector3& position)
	{
		m_Position = position;
		m_IsDirty = true;
	}

	void SetPosition(
		float x,
		float y,
		float z)
	{
		SetPosition(Vector3(x, y, z));
	}

	const Vector3& GetPosition() const
	{
		return m_Position;
	}

	void Translate(const Vector3& translation)
	{
		m_Position += translation;
		m_IsDirty = true;
	}

	//====================
	// Rotation
	//====================

	void SetRotation(const Vector3& rotation)
	{
		m_Rotation = rotation;
		m_IsDirty = true;
	}

	void SetRotation(
		float x,
		float y,
		float z)
	{
		SetRotation(Vector3(x, y, z));
	}

	const Vector3& GetRotation() const
	{
		return m_Rotation;
	}

	void Rotate(const Vector3& rotation)
	{
		m_Rotation += rotation;
		m_IsDirty = true;
	}

	//====================
	// Scale
	//====================

	void SetScale(const Vector3& scale)
	{
		m_Scale = scale;
		m_IsDirty = true;
	}

	void SetScale(float scale)
	{
		SetScale(Vector3(scale, scale, scale));
	}

	void SetScale(
		float x,
		float y,
		float z)
	{
		SetScale(Vector3(x, y, z));
	}

	const Vector3& GetScale() const
	{
		return m_Scale;
	}

	//====================
	// Matrix
	//====================

	/**
	 * @brief WorldMatrixを取得
	 */
	const Matrix4x4& GetWorldMatrix() const;


	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "TransformComponent";
	}

private:

	/**
	 * @brief WorldMatrixを再計算
	 */
	void UpdateWorldMatrix() const;

private:

	Vector3 m_Position =
		Vector3{0.0f, 0.0f, 0.0f};

	Vector3 m_Rotation =
		Vector3{0.0f, 0.0f, 0.0f};

	Vector3 m_Scale =
		Vector3{1.0f, 1.0f, 1.0f};

	// WorldMatrixは必要になった時だけ再計算する
	mutable Matrix4x4 m_WorldMatrix =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

	mutable bool m_IsDirty = true;
};
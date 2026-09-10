#include "TransformComponent.h"
#include <DebugUI.h>

void TransformComponent::Update()
{
}

/**
 * @brief WorldMatrix取得
 */
const Matrix4x4& TransformComponent::GetWorldMatrix() const
{
	// Transformに変更があった場合のみ再計算
	if (m_IsDirty)
	{
		UpdateWorldMatrix();
	}

	return m_WorldMatrix;
}

/**
 * @brief WorldMatrixを更新
 */
void TransformComponent::UpdateWorldMatrix() const
{
	//====================
	// Scale
	//====================

	const Matrix4x4 scaleMatrix =
		Matrix4x4::CreateScale(
			m_Scale);

	//====================
	// Rotation
	//====================

	// SimpleMathのCreateFromYawPitchRollは
	// Yaw(Y), Pitch(X), Roll(Z)の順で指定
	const Matrix4x4 rotationMatrix =
		Matrix4x4::CreateFromYawPitchRoll(
			m_Rotation.y,
			m_Rotation.x,
			m_Rotation.z);

	//====================
	// Translation
	//====================

	const Matrix4x4 translationMatrix =
		Matrix4x4::CreateTranslation(
			m_Position);

	//====================
	// World
	//====================

	// DirectX / SimpleMathでは
	// Scale → Rotation → Translation
	m_WorldMatrix =
		scaleMatrix *
		rotationMatrix *
		translationMatrix;

	m_IsDirty = false;
}


void TransformComponent::DrawDebugUI()
{
	float position[3] =
	{
		m_Position.x,
		m_Position.y,
		m_Position.z
	};
	if(ImGui::DragFloat3("Position", position, 0.1f))
	{
		SetPosition(position[0], position[1], position[2]);
		m_IsDirty = true;
	}

	float rotation[3] =
	{
		m_Rotation.x,
		m_Rotation.y,
		m_Rotation.z
	};
	if(ImGui::DragFloat3("Rotation", rotation, 0.1f))
	{
		SetRotation(rotation[0], rotation[1], rotation[2]);
		m_IsDirty = true;
	}

	float scale[3] =
	{
		m_Scale.x,
		m_Scale.y,
		m_Scale.z
	};
	if(ImGui::DragFloat3("Scale", scale, 0.1f))
	{
		SetScale(scale[0], scale[1], scale[2]);
		m_IsDirty = true;
	}

}
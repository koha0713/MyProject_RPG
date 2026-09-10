#include "TransformComponent.h"
#include <DebugUI.h>

void TransformComponent::Update()
{
	// Transformの更新処理
	ImGuiDrawTransformGizmo();
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

void TransformComponent::ImGuiDrawTransformGizmo()
{
	DebugUI::RegisterDebugFunction([this]()
		{
			ImGui::Begin("Transform");
			if (ImGui::SliderFloat3("Position", &m_Position.x, -10.0f, 10.0f))
			{
				m_IsDirty = true;
			}
			if (ImGui::SliderFloat3("Rotation(ラジアン)", &m_Rotation.x, -3.14159f, 3.14159f))
			{
				m_IsDirty = true;
			}
			if (ImGui::SliderFloat3("Scale", &m_Scale.x, 0.0f, 10.0f))
			{
				m_IsDirty = true;
			}
			ImGui::End();
		});
}
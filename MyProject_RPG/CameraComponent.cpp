#include "CameraComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"
#include "Window.h"
#include "DebugUI.h"

//=====================================================
// ライフサイクル
//=====================================================

void CameraComponent::Initialize()
{
}

void CameraComponent::Finalize()
{
}

void CameraComponent::Update(uint64_t delta)
{
}

void CameraComponent::Draw()
{
}

//=====================================================
// View Matrix
//=====================================================

Matrix4x4 CameraComponent::GetViewMatrix() const
{
	//====================
	// Owner確認
	//====================

	const GameObject* owner =
		GetOwner();

	if (!owner)
	{
		// Identityをstatic定数で使用すると
		// SimpleMath.cppへの依存が発生するため、
		// 明示的に単位行列を生成しておく
		return Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	//====================
	// Transform取得
	//====================

	const TransformComponent* transform =
		owner->GetComponent<TransformComponent>();

	if (!transform)
	{
		return Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	//====================
	// Camera Transform
	//====================

	const Vector3 position =
		transform->GetPosition();

	const Vector3 rotation =
		transform->GetRotation();

	//====================
	// Forward生成
	//====================
	//
	// Cameraの基準Forwardを +Z とする。
	// DirectX左手座標系を前提としている。
	//

	const Matrix4x4 rotationMatrix =
		Matrix4x4::CreateFromYawPitchRoll(
			rotation.y,
			rotation.x,
			rotation.z);

	Vector3 forward(
		0.0f,
		0.0f,
		1.0f);

	Vector3 up(
		0.0f,
		1.0f,
		0.0f);

	// 回転をForward / Upへ適用
	forward =
		Vector3::TransformNormal(
			forward,
			rotationMatrix);

	up =
		Vector3::TransformNormal(
			up,
			rotationMatrix);

	//====================
	// Target
	//====================

	const Vector3 target =
		position +
		forward;

	//====================
	// View行列生成
	//====================

	return Matrix4x4::CreateLookAt(
		position,
		target,
		up);
}

//=====================================================
// Projection Matrix
//=====================================================

Matrix4x4 CameraComponent::GetProjectionMatrix() const
{
	const float width =
		static_cast<float>(
			Window::GetWidth());

	const float height =
		static_cast<float>(
			Window::GetHeight());

	// Windowサイズが不正な場合
	if (width <= 0.0f ||
		height <= 0.0f)
	{
		return Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	const float aspect =
		width / height;

	return Matrix4x4::
		CreatePerspectiveFieldOfView(
			DirectX::XMConvertToRadians(
				m_FieldOfView),
			aspect,
			m_NearClip,
			m_FarClip);
}

//=====================================================
// Projection設定
//=====================================================

void CameraComponent::SetFieldOfView(
	float degree)
{
	// 不正な画角を防ぐ
	if (degree <= 1.0f)
	{
		degree = 1.0f;
	}
	else if (degree >= 179.0f)
	{
		degree = 179.0f;
	}

	m_FieldOfView =
		degree;
}

void CameraComponent::SetNearClip(
	float nearClip)
{
	if (nearClip <= 0.0f)
	{
		return;
	}

	if (nearClip >= m_FarClip)
	{
		return;
	}

	m_NearClip =
		nearClip;
}

void CameraComponent::SetFarClip(
	float farClip)
{
	if (farClip <= m_NearClip)
	{
		return;
	}

	m_FarClip =
		farClip;
}


void CameraComponent::DrawDebugUI()
{
	ImGui::DragFloat("FieldOfView", &m_FieldOfView, 0.1f, 1.0f, 179.0f);
	ImGui::DragFloat("NearClip", &m_NearClip, 0.01f, 0.01f, m_FarClip - 0.01f);
	ImGui::DragFloat("FarClip", &m_FarClip, 0.1f, m_NearClip + 0.01f);
}
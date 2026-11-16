#include "CameraComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"
#include "Window.h"
#include "DebugUI.h"
#include "SoundManager.h"
#include "InputManager.h"

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
	//====================
	// Owner確認
	//====================

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	//====================
	// Transform取得
	//====================

	TransformComponent* transform =
		owner->GetComponent<TransformComponent>();

	if (!transform)
	{
		return;
	}

	//====================
	// Camera操作(仮)
	//====================
	if (INPUT_MANAGER.IsKeyDown(KeyCode::W))
	{
		transform->Translate(Vector3(0.0f,0.0f,-0.1f));
	}
	if (INPUT_MANAGER.IsKeyDown(KeyCode::S))
	{
		transform->Translate(Vector3(0.0f, 0.0f, 0.1f));
	}
	if (INPUT_MANAGER.IsKeyDown(KeyCode::A))
	{
		transform->Translate(Vector3(-0.1f, 0.0f, 0.0f));
	}
	if (INPUT_MANAGER.IsKeyDown(KeyCode::D))
	{
		transform->Translate(Vector3(0.1f, 0.0f, 0.0f));
	}

	//====================
	// Camera Transform
	//====================

	const Vector3 position =
		transform->GetPosition();

	const Vector3 rotation =
		transform->GetRotation();

	//====================
	// Rotation Matrix
	//====================

	const Matrix4x4 rotationMatrix =
		Matrix4x4::CreateFromYawPitchRoll(
			rotation.y,
			rotation.x,
			rotation.z);

	//====================
	// Forward / Up
	//====================

	Vector3 forward(
		0.0f,
		0.0f,
		1.0f);

	Vector3 up(
		0.0f,
		1.0f,
		0.0f);

	forward =
		Vector3::TransformNormal(
			forward,
			rotationMatrix);

	up =
		Vector3::TransformNormal(
			up,
			rotationMatrix);

	// 念のため正規化
	forward.Normalize();
	up.Normalize();

	//====================
	// Audio Listener
	//====================

	SOUND_MANAGER.SetListener(
		position,
		forward,
		up);

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


	return Matrix4x4(
		DirectX::XMMatrixLookAtLH(
			position,
			target,
			up));
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

bool CameraComponent::SetTarget(
	const Vector3& pos,
	float distance,
	const Vector3* rotation)
{
	const Vector3 pos_n(
		pos.x,pos.y,-pos.z
	);
	if (!std::isfinite(pos_n.x) ||
		!std::isfinite(pos_n.y) ||
		!std::isfinite(pos_n.z) ||
		!std::isfinite(distance) ||
		distance <= 0.0f)
	{
		return false;
	}

	GameObject* owner = GetOwner();

	if (!owner)
	{
		return false;
	}

	auto* transform =
		owner->GetComponent<TransformComponent>();

	if (!transform)
	{
		return false;
	}

	// 省略時は現在の回転を使用する
	const Vector3 cameraRotation =
		rotation ? *rotation : transform->GetRotation();

	if (!std::isfinite(cameraRotation.x) ||
		!std::isfinite(cameraRotation.y) ||
		!std::isfinite(cameraRotation.z))
	{
		return false;
	}

	const Matrix4x4 rotationMatrix =
		Matrix4x4::CreateFromYawPitchRoll(
			cameraRotation.y,
			cameraRotation.x,
			cameraRotation.z);

	Vector3 forward =
		Vector3::TransformNormal(
			Vector3(0.0f, 0.0f, 1.0f),
			rotationMatrix);

	forward.Normalize();

	// 現在の回転を維持して、対象からdistance離す
	const Vector3 cameraPosition =
		pos_n - forward * distance;

	if (!std::isfinite(cameraPosition.x) ||
		!std::isfinite(cameraPosition.y) ||
		!std::isfinite(cameraPosition.z))
	{
		return false;
	}

	// 入力座標のY・Zを反転して注視対象にする
	const Vector3 targetPosition(
		cameraPosition.x,
		-cameraPosition.y,
		-cameraPosition.z);

	transform->SetPosition(targetPosition);

	return true;
}


void CameraComponent::DrawDebugUI()
{
	ImGui::DragFloat("FieldOfView", &m_FieldOfView, 0.1f, 1.0f, 179.0f);
	ImGui::DragFloat("NearClip", &m_NearClip, 0.01f, 0.01f, m_FarClip - 0.01f);
	ImGui::DragFloat("FarClip", &m_FarClip, 0.1f, m_NearClip + 0.01f);

	ImGui::Separator();
	ImGui::Text("SetTarget Test");

	// テスト入力値をフレーム間で保持する
	static float targetPosition[3] =
	{
		0.0f,
		1.0f,
		0.0f
	};

	static float targetDistance = 5.0f;
	static bool attempted = false;
	static bool succeeded = false;

	ImGui::DragFloat3(
		"Target Position",
		targetPosition,
		0.1f);

	ImGui::DragFloat(
		"Target Distance",
		&targetDistance,
		0.1f,
		0.01f,
		1000.0f);

	if (ImGui::Button("Apply SetTarget"))
	{
		succeeded = SetTarget(
			Vector3(
				targetPosition[0],
				targetPosition[1],
				targetPosition[2]),
			targetDistance);

		attempted = true;
	}

	if (attempted)
	{
		ImGui::Text(
			"Result: %s",
			succeeded ? "Success" : "Failed");
	}
}
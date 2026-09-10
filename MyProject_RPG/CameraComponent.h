#pragma once

#include "Component.h"
#include "CommonType.h"

/**
 * @brief 3DカメラComponent
 *
 * @details
 * TransformComponentから位置・回転を取得し、
 * View行列とProjection行列を生成する。
 *
 * Rendererはカメラそのものを所有せず、
 * CameraComponentが生成した行列のみを使用する。
 */
class CameraComponent :
	public Component
{
public:

	CameraComponent() = default;
	~CameraComponent() override = default;

	//====================
	// ライフサイクル
	//====================

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// Matrix
	//====================

	/**
	 * @brief View行列を取得
	 */
	Matrix4x4 GetViewMatrix() const;

	/**
	 * @brief Projection行列を取得
	 */
	Matrix4x4 GetProjectionMatrix() const;

	//====================
	// Projection設定
	//====================

	/**
	 * @brief 垂直画角設定
	 *
	 * @param degree degree単位
	 */
	void SetFieldOfView(float degree);

	/**
	 * @brief NearClip設定
	 */
	void SetNearClip(float nearClip);

	/**
	 * @brief FarClip設定
	 */
	void SetFarClip(float farClip);

	float GetFieldOfView() const
	{
		return m_FieldOfView;
	}

	float GetNearClip() const
	{
		return m_NearClip;
	}

	float GetFarClip() const
	{
		return m_FarClip;
	}

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "CameraComponent";
	}

private:

	//====================
	// Projection
	//====================

	// 垂直画角
	float m_FieldOfView = 60.0f;

	// Near Clip :
	float m_NearClip = 0.1f;

	// Far Clip
	float m_FarClip = 1000.0f;
};
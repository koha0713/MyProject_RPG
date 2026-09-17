#include "MousePicker.h"

#include <DirectXMath.h>
#include <cmath>

//=====================================================
// Ground Picking
//=====================================================

bool MousePicker::PickGround(
	const Vector2& mousePosition,
	float screenWidth,
	float screenHeight,
	const Matrix4x4& view,
	const Matrix4x4& projection,
	float planeY,
	Vector3& outWorldPosition)
{
	//=================================================
	// Screenサイズ確認
	//=================================================

	if (screenWidth <= 0.0f ||
		screenHeight <= 0.0f)
	{
		return false;
	}

	//=================================================
	// MouseがClient領域内にいるか確認
	//=================================================

	if (mousePosition.x < 0.0f ||
		mousePosition.y < 0.0f ||
		mousePosition.x >= screenWidth ||
		mousePosition.y >= screenHeight)
	{
		return false;
	}

	//=================================================
	// Near / Far位置をScreen座標として作成
	//=================================================
	//
	// DirectXのDepth範囲は
	//
	// Near = 0.0f
	// Far  = 1.0f
	//
	// として扱う。
	//

	const DirectX::XMVECTOR screenNear =
		DirectX::XMVectorSet(
			mousePosition.x,
			mousePosition.y,
			0.0f,
			1.0f);

	const DirectX::XMVECTOR screenFar =
		DirectX::XMVectorSet(
			mousePosition.x,
			mousePosition.y,
			1.0f,
			1.0f);

	//=================================================
	// World Matrix
	//=================================================
	//
	// Pickingでは特定ModelのLocal座標へ変換するのではなく、
	// World空間そのものへ戻したいためIdentityを使用する。
	//

	const DirectX::XMMATRIX world =
		DirectX::XMMatrixIdentity();

	//=================================================
	// Screen → World
	//=================================================

	const DirectX::XMVECTOR worldNear =
		DirectX::XMVector3Unproject(
			screenNear,
			0.0f,
			0.0f,
			screenWidth,
			screenHeight,
			0.0f,
			1.0f,
			projection,
			view,
			world);

	const DirectX::XMVECTOR worldFar =
		DirectX::XMVector3Unproject(
			screenFar,
			0.0f,
			0.0f,
			screenWidth,
			screenHeight,
			0.0f,
			1.0f,
			projection,
			view,
			world);

	//=================================================
	// XMVECTOR → Vector3
	//=================================================

	DirectX::XMFLOAT3 nearPoint{};
	DirectX::XMFLOAT3 farPoint{};

	DirectX::XMStoreFloat3(
		&nearPoint,
		worldNear);

	DirectX::XMStoreFloat3(
		&farPoint,
		worldFar);

	const Vector3 rayOrigin(
		nearPoint.x,
		nearPoint.y,
		nearPoint.z);

	Vector3 rayDirection(
		farPoint.x - nearPoint.x,
		farPoint.y - nearPoint.y,
		farPoint.z - nearPoint.z);

	//=================================================
	// Ray Direction正規化
	//=================================================

	const float directionLength =
		rayDirection.Length();

	if (directionLength <= 0.000001f)
	{
		return false;
	}

	rayDirection.Normalize();

	//=================================================
	// Rayと水平面の交差判定
	//=================================================
	//
	// Ray:
	//
	// P = Origin + Direction * t
	//
	// Y成分について、
	//
	// planeY =
	//     Origin.y +
	//     Direction.y * t
	//
	// よって、
	//
	// t =
	//     (planeY - Origin.y)
	//     / Direction.y
	//

	if (std::abs(
		rayDirection.y) <= 0.000001f)
	{
		// Rayが地面とほぼ平行。
		return false;
	}

	const float t =
		(planeY -
			rayOrigin.y) /
		rayDirection.y;

	// Cameraから後ろ側にある交点は無効。
	if (t < 0.0f)
	{
		return false;
	}

	//=================================================
	// World交点
	//=================================================

	outWorldPosition =
		rayOrigin +
		rayDirection * t;

	// 誤差で僅かにずれる可能性があるため、
	// Yは指定Plane位置へ固定する。
	outWorldPosition.y =
		planeY;

	return true;
}
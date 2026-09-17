#pragma once

/**
 * @file MousePicker.h
 * @brief Mouse座標から3D空間上の位置を取得するUtility
 */

#include "CommonType.h"

 /**
  * @brief Mouse Picking Utility
  *
  * @details
  * Client領域上のMouse座標からWorld空間へRayを生成し、
  * 指定した水平面との交点を取得する。
  *
  * InputManagerやCameraComponentには直接依存せず、
  * 必要な値だけを引数として受け取る。
  */
class MousePicker
{
public:

	/**
	 * @brief Mouse位置から水平面との交点を取得する
	 *
	 * @param mousePosition Client領域上のMouse座標
	 * @param screenWidth 描画領域の横幅
	 * @param screenHeight 描画領域の縦幅
	 * @param view View行列
	 * @param projection Projection行列
	 * @param planeY 判定対象となる水平面のY座標
	 * @param outWorldPosition 交点となったWorld座標
	 *
	 * @return 交点取得成功時true
	 */
	static bool PickGround(
		const Vector2& mousePosition,
		float screenWidth,
		float screenHeight,
		const Matrix4x4& view,
		const Matrix4x4& projection,
		float planeY,
		Vector3& outWorldPosition);
};
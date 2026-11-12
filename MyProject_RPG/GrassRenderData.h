#pragma once

#include "CommonType.h"

#include <cstdint>
#include <memory>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>

/**
 * @brief 草1枚分の頂点データ
 */
struct GrassVertex
{
	/**
	 * @brief 草株中心からのLocal座標
	 *
	 * Y=0が根元。
	 * Y=1が先端。
	 */
	Vector3 Position =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	Vector2 TexCoord =
		Vector2(
			0.0f,
			0.0f);
};

/**
 * @brief 草1株ごとのInstanceデータ
 *
 * 同じGrass Meshを描画し、
 * このデータだけ株ごとに変更する。
 */
struct GrassInstanceData
{
	/**
	 * @brief 草株の根元World座標
	 */
	Vector3 Position =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	/**
	 * @brief Y軸回転角度（radian）
	 */
	float Rotation =
		0.0f;

	/**
	 * @brief 草全体のScale
	 */
	float Scale =
		1.0f;

	/**
	 * @brief 固定乱数
	 *
	 * 風の位相、色変化などに使用予定。
	 */
	float Random =
		0.0f;

	// 16byte境界を維持しやすくするPadding
	float Padding0 =
		0.0f;

	float Padding1 =
		0.0f;
};

/**
 * @brief Terrainごとの草生成設定
 */
struct GrassSpawnSetting
{
	bool Enabled =
		false;

	/**
	 * @brief 1Cellあたりの草株数
	 */
	int Density =
		0;

	float MinScale =
		0.8f;

	float MaxScale =
		1.2f;
};

/**
 * @brief 1Chunk分の草描画データ
 */
struct GrassChunkRenderData
{
	int ChunkX =
		0;

	int ChunkY =
		0;

	std::vector<GrassInstanceData>
		Instances;

	Microsoft::WRL::ComPtr<ID3D11Buffer>
		InstanceBuffer;

	void Clear()
	{
		Instances.clear();
		InstanceBuffer.Reset();
	}

	bool HasGpuBuffer() const
	{
		return
			InstanceBuffer != nullptr &&
			!Instances.empty();
	}
};
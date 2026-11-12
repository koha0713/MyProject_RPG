#pragma once

#include "CommonType.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "MeshBuffer.h"

/**
 * @brief Water専用頂点
 *
 * 水面Geometry自体は平面だが、
 * 各頂点に水深を保持する。
 */
struct WaterVertex
{
	// World変換前の頂点位置
	Vector3 Position =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	// 水面は基本的に上向き
	Vector3 Normal =
		Vector3(
			0.0f,
			1.0f,
			0.0f);

	Vector2 TexCoord =
		Vector2(
			0.0f,
			0.0f);

	/**
	 * @brief 水面から川底までの距離
	 */
	float WaterDepth =
		0.0f;
};

/**
 * @brief 1Chunk分のWater Mesh
 */
struct TerrainWaterChunkMesh
{
	std::vector<WaterVertex>
		Vertices;

	std::vector<uint32_t>
		Indices;

	std::shared_ptr<MeshBuffer>
		Mesh;

	void Clear()
	{
		Vertices.clear();
		Indices.clear();
		Mesh.reset();
	}

	bool IsEmpty() const
	{
		return
			Vertices.empty() ||
			Indices.empty();
	}

	bool HasGpuMesh() const
	{
		return
			Mesh != nullptr &&
			Mesh->IsValid();
	}
};
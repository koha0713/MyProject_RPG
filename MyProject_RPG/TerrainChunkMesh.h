#pragma once

#include <memory>
#include <vector>
#include <cstdint>

#include "ModelData.h"
#include "TerrainVertex.h"

class MeshBuffer;

/**
 * @brief TerrainChunk 1個分のMesh情報
 *
 * @details
 * CPU側のVertex / Indexと、
 * GPU側のMeshBufferをまとめて保持する。
 */
struct TerrainChunkMesh
{
	//====================
	// CPU Mesh
	//====================

	std::vector<VertexData>
		Vertices;

	std::vector<uint32_t>
		Indices;

	//====================
	// GPU Mesh
	//====================

	std::shared_ptr<MeshBuffer>
		Mesh;

	/**
	 * @brief CPU / GPUデータを破棄
	 */
	void Clear()
	{
		Vertices.clear();
		Indices.clear();

		Mesh.reset();
	}

	/**
	 * @brief CPU Meshが空か
	 */
	bool IsEmpty() const
	{
		return
			Vertices.empty() ||
			Indices.empty();
	}

	/**
	 * @brief GPU Meshが有効か
	 */
	bool HasGpuMesh() const
	{
		return
			Mesh &&
			Mesh->IsValid();
	}
};


/**
 * @brief Terrain Ground専用Chunk Mesh
 */
struct TerrainGroundChunkMesh
{
	std::vector<TerrainVertex>
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
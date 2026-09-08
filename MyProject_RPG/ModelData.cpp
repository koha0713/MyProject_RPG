#include "ModelData.h"

/**
 * @brief CPU上のMeshDataからGPUバッファを生成
 */
bool ModelData::CreateGpuResources(
	ID3D11Device* device)
{
	if (!device)
	{
		return false;
	}

	for (auto& meshData : m_Meshes)
	{
		// CPUデータが存在しないMeshは異常
		if (meshData.Vertices.empty() ||
			meshData.Indices.empty())
		{
			return false;
		}

		auto meshBuffer =
			std::make_shared<MeshBuffer>();

		const bool result =
			meshBuffer->Create(
				device,
				meshData.Vertices.data(),
				static_cast<uint32_t>(
					sizeof(VertexData)),
				static_cast<uint32_t>(
					meshData.Vertices.size()),
				meshData.Indices.data(),
				static_cast<uint32_t>(
					meshData.Indices.size()));

		if (!result)
		{
			return false;
		}

		meshData.Mesh =
			std::move(meshBuffer);
	}

	return true;
}
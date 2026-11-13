#include "GrassRendererComponent.h"

#include "Renderer.h"
#include "MeshBuffer.h"
#include "Texture.h"

#include <algorithm>
#include <cmath>
#include <random>

//=====================================================
// Initialize
//=====================================================

void GrassRendererComponent::
Initialize()
{
	BuildGrassMesh();
}

//=====================================================
// Finalize
//=====================================================

void GrassRendererComponent::
Finalize()
{

	m_TerrainChunks =
		nullptr;
	m_GrassMesh.reset();

	m_GrassTexture.reset();

	for (GrassChunkRenderData& chunk :
		m_GrassChunks)
	{
		chunk.Clear();
	}

	m_GrassChunks.clear();
}

void GrassRendererComponent::
Update(
	uint64_t delta)
{
	const float deltaSeconds =
		static_cast<float>(
			delta) /
		1000000.0f;

	m_Time +=
		deltaSeconds;

	// 精度低下を避けるため適当に戻す
	if (m_Time >
		1000.0f)
	{
		m_Time =
			0.0f;
	}
}

void GrassRendererComponent::
Draw()
{
	if (!m_GrassMesh ||
		!m_GrassMesh->IsValid())
	{
		return;
	}

	if (!m_GrassTexture ||
		!m_GrassTexture->IsValid())
	{
		return;
	}

	if (!m_TerrainChunks)
	{
		return;
	}

	//=================================================
	// Safety
	//=================================================
	//
	// Grass ChunkとTerrain Chunkの数が
	// 一致している範囲だけ処理する。
	//=================================================

	const size_t chunkCount =
		(std::min)(
			m_GrassChunks.size(),
			m_TerrainChunks->size());

	for (size_t i = 0;
		i < chunkCount;
		++i)
	{
		GrassChunkRenderData& grassChunk =
			m_GrassChunks[i];

		if (!grassChunk.HasGpuBuffer())
		{
			continue;
		}

		const TerrainChunk& terrainChunk =
			(*m_TerrainChunks)[i];

		//=================================================
		// Terrain LOD
		//=================================================

		const TerrainChunkLOD lod =
			terrainChunk.GetLOD();

		const float lodRatio =
			GetGrassLodRatio(
				lod);

		if (lodRatio <=
			0.0f)
		{
			continue;
		}

		//=================================================
		// Instance Count
		//=================================================

		const uint32_t totalCount =
			static_cast<uint32_t>(
				grassChunk.Instances.size());

		uint32_t drawCount =
			static_cast<uint32_t>(
				static_cast<float>(
					totalCount) *
				lodRatio);

		// 草が存在するChunkなら、
		// 描画対象LODで最低1株は残す。
		if (drawCount == 0 &&
			totalCount > 0)
		{
			drawCount =
				1;
		}

		drawCount =
			(std::min)(
				drawCount,
				totalCount);

		//=================================================
		// Draw
		//=================================================

		Renderer::DrawGrassInstanced(
			*m_GrassMesh,
			grassChunk.InstanceBuffer.Get(),
			drawCount,
			m_GrassTexture.get(),
			m_Time,
			m_WindDirection,
			m_WindStrength,
			m_WindSpeed);
	}
}

bool GrassRendererComponent::
BuildGrass(
	const std::vector<TerrainChunk>& chunks)
{
	//=================================================
	// Terrain Chunk Reference
	//=================================================
	//
	// Grass描画時にTerrain側のLODを参照する。
	//=================================================

	m_TerrainChunks =
		&chunks;

	for (GrassChunkRenderData& chunk :
		m_GrassChunks)
	{
		chunk.Clear();
	}

	m_GrassChunks.clear();

	m_GrassChunks.resize(
		chunks.size());

	bool createdAny =
		false;

	for (size_t i = 0;
		i < chunks.size();
		++i)
	{
		BuildGrassChunk(
			chunks[i],
			m_GrassChunks[i]);

		if (m_GrassChunks[i].
			Instances.empty())
		{
			continue;
		}

		if (!BuildInstanceBuffer(
			m_GrassChunks[i]))
		{
			continue;
		}

		createdAny =
			true;
	}

	return createdAny;
}

//=====================================================
// Get Grass LOD Ratio
//=====================================================

float GrassRendererComponent::
GetGrassLodRatio(
	TerrainChunkLOD lod) const
{
	switch (lod)
	{
	case TerrainChunkLOD::Near:
		// 近距離は全て描画
		return 1.0f;

	case TerrainChunkLOD::Middle:
		// 中距離は半分
		return 0.6f;

	case TerrainChunkLOD::Far:
		// 遠距離はかなり減らす
		return 0.1f;

	case TerrainChunkLOD::Culled:
	default:
		// 描画しない
		return 0.0f;
	}
}

//=====================================================
// Build Grass Mesh
//=====================================================

bool GrassRendererComponent::
BuildGrassMesh()
{
	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
	{
		return false;
	}

	//=================================================
	// Base Size
	//=================================================

	constexpr float HALF_WIDTH =
		0.50f;

	constexpr float HEIGHT =
		1.3f;

	//=================================================
	// Cross Billboard
	//=================================================
	//
	// Plane A : X方向
	//
	// Plane B : Z方向
	//
	// 草1株を2枚のQuadで表現する。
	//=================================================

	const GrassVertex vertices[] =
	{
		//=============================================
		// Plane A
		//=============================================

		{
			Vector3(
				-HALF_WIDTH,
				0.0f,
				0.0f),

			Vector2(
				0.0f,
				1.0f)
		},

		{
			Vector3(
				HALF_WIDTH,
				0.0f,
				0.0f),

			Vector2(
				1.0f,
				1.0f)
		},

		{
			Vector3(
				-HALF_WIDTH,
				HEIGHT,
				0.0f),

			Vector2(
				0.0f,
				0.0f)
		},

		{
			Vector3(
				HALF_WIDTH,
				HEIGHT,
				0.0f),

			Vector2(
				1.0f,
				0.0f)
		},

		//=============================================
		// Plane B
		//=============================================

		{
			Vector3(
				0.0f,
				0.0f,
				-HALF_WIDTH),

			Vector2(
				0.0f,
				1.0f)
		},

		{
			Vector3(
				0.0f,
				0.0f,
				HALF_WIDTH),

			Vector2(
				1.0f,
				1.0f)
		},

		{
			Vector3(
				0.0f,
				HEIGHT,
				-HALF_WIDTH),

			Vector2(
				0.0f,
				0.0f)
		},

		{
			Vector3(
				0.0f,
				HEIGHT,
				HALF_WIDTH),

			Vector2(
				1.0f,
				0.0f)
		}
	};

	const uint32_t indices[] =
	{
		// Plane A
		0, 1, 2,
		2, 1, 3,

		// Plane B
		4, 5, 6,
		6, 5, 7
	};

	auto mesh =
		std::make_shared<MeshBuffer>();

	const bool created =
		mesh->Create(
			device,
			vertices,
			static_cast<uint32_t>(
				sizeof(
					GrassVertex)),
			static_cast<uint32_t>(
				std::size(
					vertices)),
			indices,
			static_cast<uint32_t>(
				std::size(
					indices)));

	if (!created)
	{
		return false;
	}

	m_GrassMesh =
		std::move(
			mesh);

	return true;
}

void GrassRendererComponent::
BuildGrassChunk(
	const TerrainChunk& chunk,
	GrassChunkRenderData& outChunk)
{
	outChunk.Clear();

	const GridPosition start =
		chunk.GetStartGrid();

	outChunk.ChunkX =
		start.X;

	outChunk.ChunkY =
		start.Y;

	const auto& cells =
		chunk.GetCells();

	for (const TerrainCellData* cell :
		cells)
	{
		if (!cell)
		{
			continue;
		}

		//=================================================
		// Terrain Spawn Setting
		//=================================================

		const GrassSpawnSetting setting =
			GetGrassSpawnSetting(
				cell->Type);

		if (!setting.Enabled ||
			setting.Density <= 0)
		{
			continue;
		}

		//=================================================
		// Generate Grass Instances
		//=================================================

		for (int i = 0;
			i < setting.Density;
			++i)
		{
			uint32_t seed =
				MakeSeed(
					cell->Position.X,
					cell->Position.Y,
					i);

			const float randomX =
				Random01(
					seed);

			const float randomZ =
				Random01(
					seed);

			const float randomRotation =
				Random01(
					seed);

			const float randomScale =
				Random01(
					seed);

			const float randomValue =
				Random01(
					seed);

			//=============================================
			// Position
			//=============================================

			const float centerX =
				static_cast<float>(
					cell->Position.X) *
				m_CellSize;

			const float centerZ =
				static_cast<float>(
					cell->Position.Y) *
				m_CellSize;

			//=================================================
			// Neighbor Grass Check
			//=================================================

			const bool grassUp =
				CanSpawnGrassAt(
					GridPosition
					{
						cell->Position.X,
						cell->Position.Y - 1
					});

			const bool grassRight =
				CanSpawnGrassAt(
					GridPosition
					{
						cell->Position.X + 1,
						cell->Position.Y
					});

			const bool grassDown =
				CanSpawnGrassAt(
					GridPosition
					{
						cell->Position.X,
						cell->Position.Y + 1
					});

			const bool grassLeft =
				CanSpawnGrassAt(
					GridPosition
					{
						cell->Position.X - 1,
						cell->Position.Y
					});

			//=================================================
			// Placement Range
			//=================================================
			//
			// 隣も草地ならCell境界まで配置する。
			// 隣が草地でない場合のみ少し内側へ寄せる。
			//=================================================

			constexpr float BORDER_MARGIN =
				0.08f;

			const float halfCell =
				m_CellSize *
				0.5f;

			// 各方向で草を配置できる範囲
			const float minX =
				grassLeft
				? -halfCell
				: -halfCell +
				m_CellSize *
				BORDER_MARGIN;

			const float maxX =
				grassRight
				? halfCell
				: halfCell -
				m_CellSize *
				BORDER_MARGIN;

			const float minZ =
				grassUp
				? -halfCell
				: -halfCell +
				m_CellSize *
				BORDER_MARGIN;

			const float maxZ =
				grassDown
				? halfCell
				: halfCell -
				m_CellSize *
				BORDER_MARGIN;

			//=================================================
			// Random Position
			//=================================================

			const float offsetX =
				minX +
				(maxX - minX) *
				randomX;

			const float offsetZ =
				minZ +
				(maxZ - minZ) *
				randomZ;

			GrassInstanceData instance;

			instance.Position =
				Vector3(
					centerX +
					offsetX,
					0.01f,
					centerZ +
					offsetZ);

			//=============================================
			// Rotation
			//=============================================

			constexpr float TWO_PI =
				6.28318530718f;

			instance.Rotation =
				randomRotation *
				TWO_PI;

			//=============================================
			// Scale
			//=============================================

			instance.Scale =
				setting.MinScale +
				(
					setting.MaxScale -
					setting.MinScale
					) *
				randomScale;

			//=============================================
			// Random
			//=============================================

			instance.Random =
				randomValue;

			outChunk.Instances.push_back(
				instance);
		}
	}

	//=================================================
	// Shuffle Instances
	//=================================================
	//
	// LODで先頭N個だけ描画しても
	// Chunk全体へ均等に草が残るよう、
	// Instance順を固定Seedでシャッフルする。
	//
	// 固定Seedなので毎回同じ配置順になる。
	//=================================================

	if (!outChunk.Instances.empty())
	{
		uint32_t shuffleSeed =
			static_cast<uint32_t>(
				outChunk.ChunkX) *
			73856093u;

		shuffleSeed ^=
			static_cast<uint32_t>(
				outChunk.ChunkY) *
			19349663u;

		std::mt19937 randomEngine(
			shuffleSeed);

		std::shuffle(
			outChunk.Instances.begin(),
			outChunk.Instances.end(),
			randomEngine);
	}
}

bool GrassRendererComponent::
BuildInstanceBuffer(
	GrassChunkRenderData& chunk)
{
	if (chunk.Instances.empty())
	{
		return true;
	}

	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
	{
		return false;
	}

	//=================================================
	// Instance Vertex Buffer
	//=================================================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				GrassInstanceData) *
			chunk.Instances.size());

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_VERTEX_BUFFER;

	bufferDesc.CPUAccessFlags =
		0;

	D3D11_SUBRESOURCE_DATA initialData{};

	initialData.pSysMem =
		chunk.Instances.data();

	const HRESULT hr =
		device->CreateBuffer(
			&bufferDesc,
			&initialData,
			chunk.InstanceBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

GrassSpawnSetting
GrassRendererComponent::
GetGrassSpawnSetting(
	TerrainType type) const
{
	switch (type)
	{
	case TerrainType::Grass:
		return GrassSpawnSetting
		{
			true,
			36,		// Density
			0.30f,	// MinScale
			0.48f	// MaxScale
		};

	case TerrainType::Forest:
		return GrassSpawnSetting
		{
			true,
			56,		// Density
			0.35f,	// MinScale
			0.55f	// MaxScale
		};

	default:
		return GrassSpawnSetting{};
	}
}

//=====================================================
// Make Seed
//=====================================================

uint32_t GrassRendererComponent::
MakeSeed(
	int gridX,
	int gridY,
	int index)
{
	uint32_t seed =
		static_cast<uint32_t>(
			gridX) *
		73856093u;

	seed ^=
		static_cast<uint32_t>(
			gridY) *
		19349663u;

	seed ^=
		static_cast<uint32_t>(
			index) *
		83492791u;

	seed ^=
		seed >> 13;

	seed *=
		1274126177u;

	return seed;
}

//=====================================================
// Random 0 - 1
//=====================================================

float GrassRendererComponent::
Random01(
	uint32_t& seed)
{
	// xorshift
	seed ^=
		seed << 13;

	seed ^=
		seed >> 17;

	seed ^=
		seed << 5;

	return
		static_cast<float>(
			seed &
			0x00FFFFFFu) /
		static_cast<float>(
			0x01000000u);
}

bool GrassRendererComponent::
CanSpawnGrass(
	TerrainType type) const
{
	const GrassSpawnSetting setting =
		GetGrassSpawnSetting(
			type);

	return setting.Enabled;
}

bool GrassRendererComponent::
CanSpawnGrassAt(
	const GridPosition& position) const
{
	if (!m_TerrainData)
	{
		return false;
	}

	const TerrainCellData* cell =
		m_TerrainData->
		FindCell(
			position);

	if (!cell)
	{
		return false;
	}

	return
		CanSpawnGrass(
			cell->Type);
}
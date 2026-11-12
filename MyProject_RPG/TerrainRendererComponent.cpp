#include "TerrainRendererComponent.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "Renderer.h"
#include "MeshBuffer.h"
#include "DebugUI.h"

//=====================================================
// Lifecycle
//=====================================================
void TerrainRendererComponent::Initialize()
{
	//=================================================
	// Ground Material
	//=================================================

	m_GroundMaterial.Diffuse =
		Vector4(
			0.38f,
			0.55f,
			0.24f,
			1.0f);

	m_GroundMaterial.Ambient =
		Vector4(
			0.15f,
			0.20f,
			0.10f,
			1.0f);

	m_GroundMaterial.Specular =
		Vector4(
			0.0f,
			0.0f,
			0.0f,
			1.0f);

	//=================================================
	// RiverBed Material
	//=================================================
	//
	// 水越しに見えることを前提に、
	// 少し明るめの土・砂色にする。
	//=================================================

	m_RiverBedMaterial.Diffuse =
		Vector4(
			0.42f,
			0.36f,
			0.24f,
			1.0f);

	m_RiverBedMaterial.Ambient =
		Vector4(
			0.18f,
			0.15f,
			0.10f,
			1.0f);

	m_RiverBedMaterial.Specular =
		Vector4(
			0.0f,
			0.0f,
			0.0f,
			1.0f);
}

void TerrainRendererComponent::Finalize()
{
	for (TerrainGroundChunkMesh& mesh :
		m_GroundMeshes)
	{
		mesh.Clear();
	}

	for (TerrainChunkMesh& mesh :
		m_RiverBedMeshes)
	{
		mesh.Clear();
	}

	for (TerrainWaterChunkMesh& mesh :
		m_WaterMeshes)
	{
		mesh.Clear();
	}

	m_GroundMeshes.clear();
	m_RiverBedMeshes.clear();
	m_WaterMeshes.clear();

	m_Chunks.clear();

	m_TerrainData =
		nullptr;
}

void TerrainRendererComponent::Update(
	uint64_t delta)
{
	//=================================================
	// Terrain LOD
	//=================================================

	UpdateChunkLOD();

	//=================================================
	// Water Time
	//=================================================

	const float deltaSeconds =
		static_cast<float>(
			delta) /
		1000000.0f;

	m_WaterTime +=
		deltaSeconds;

	//=================================================
	// float精度低下防止
	//=================================================
	//
	// 長時間起動した場合に値が巨大にならないよう
	// 適度なところで巻き戻す。
	//=================================================

	if (m_WaterTime >
		1000.0f)
	{
		m_WaterTime =
			0.0f;
	}
}

//=====================================================
// Draw
//=====================================================
void TerrainRendererComponent::Draw()
{
	const Matrix4x4 worldMatrix =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

	//=================================================
	// Ground
	//=================================================

	Renderer::SetBlendState(
		BlendState::None);

	for (size_t i = 0;
		i < m_GroundMeshes.size();
		++i)
	{
		if (i >=
			m_Chunks.size())
		{
			break;
		}

		const TerrainChunk& chunk =
			m_Chunks[i];

		if (!chunk.IsVisible())
		{
			continue;
		}

		const TerrainGroundChunkMesh& terrainMesh =
			m_GroundMeshes[i];

		if (!terrainMesh.HasGpuMesh())
		{
			continue;
		}

		Renderer::DrawTerrain(
			*terrainMesh.Mesh,
			worldMatrix,
			m_TerrainSettings,
			m_GrassTexture.get(),
			m_DirtTexture.get(),
			m_RockTexture.get(),
			m_SandTexture.get());
	}

	//=================================================
	// RiverBed
	//=================================================

	for (size_t i = 0;
		i < m_RiverBedMeshes.size();
		++i)
	{
		if (i >=
			m_Chunks.size())
		{
			break;
		}

		const TerrainChunk& chunk =
			m_Chunks[i];

		if (!chunk.IsVisible())
		{
			continue;
		}

		const TerrainChunkMesh& terrainMesh =
			m_RiverBedMeshes[i];

		if (!terrainMesh.HasGpuMesh())
		{
			continue;
		}

		Renderer::DrawMesh(
			*terrainMesh.Mesh,
			m_RiverBedMaterial,
			worldMatrix);
	}

	//=================================================
	// Water
	//=================================================

	Renderer::SetBlendState(
		BlendState::Alpha);

	for (size_t i = 0;
		i < m_WaterMeshes.size();
		++i)
	{
		if (i >=
			m_Chunks.size())
		{
			break;
		}

		const TerrainChunk& chunk =
			m_Chunks[i];

		if (!chunk.IsVisible())
		{
			continue;
		}

		const TerrainWaterChunkMesh& terrainMesh =
			m_WaterMeshes[i];

		if (!terrainMesh.HasGpuMesh())
		{
			continue;
		}

		Renderer::DrawWater(
			*terrainMesh.Mesh,
			worldMatrix,
			m_WaterSettings,
			m_WaterNormalMapA.get(),
			m_WaterNormalMapB.get(),
			m_WaterTime,
			m_CameraPosition);
	}

	//=================================================
	// State Reset
	//=================================================

	Renderer::SetBlendState(
		BlendState::None);
}

//=====================================================
// Build Terrain
//=====================================================
bool TerrainRendererComponent::BuildTerrain()
{
	if (!m_TerrainData)
	{
		OutputDebugStringA(
			"[TerrainRenderer] "
			"TerrainData is nullptr.\n");

		return false;
	}

	if (m_TerrainData->Cells.empty())
	{
		OutputDebugStringA(
			"[TerrainRenderer] "
			"TerrainData is empty.\n");

		return false;
	}

	if (m_CellSize <= 0.0f)
	{
		OutputDebugStringA(
			"[TerrainRenderer] "
			"Invalid CellSize.\n");

		return false;
	}

	//=================================================
	// Old Resources Clear
	//=================================================

	for (TerrainGroundChunkMesh& mesh :
		m_GroundMeshes)
	{
		mesh.Clear();
	}

	for (TerrainChunkMesh& mesh :
		m_RiverBedMeshes)
	{
		mesh.Clear();
	}

	for (TerrainWaterChunkMesh& mesh :
		m_WaterMeshes)
	{
		mesh.Clear();
	}

	m_GroundMeshes.clear();
	m_RiverBedMeshes.clear();
	m_WaterMeshes.clear();

	m_Chunks.clear();

	//=================================================
	// Build
	//=================================================

	if (!BuildChunks())
	{
		return false;
	}

	if (!BuildGroundMeshes())
	{
		return false;
	}

	// Water Cellの下に川底を作る
	BuildRiverBedMeshes();

	// Water Surface
	BuildWaterMeshes();

	if (!BuildGpuResources())
	{
		return false;
	}

	return true;
}

/**
 * @brief TerrainTypeからShader用Texture番号を取得
 */
float TerrainRendererComponent::
GetTerrainTextureIndex(
	TerrainType type) const
{
	switch (type)
	{
	case TerrainType::Grass:
		return 0.0f;

	case TerrainType::Dirt:
		return 1.0f;

	case TerrainType::Rock:
		return 2.0f;

	case TerrainType::Sand:
		return 3.0f;

	default:
		// Forestなど未対応Terrainは
		// 一旦Grassとして表示する
		return 0.0f;
	}
}

//=====================================================
// Build Chunks
//=====================================================

bool TerrainRendererComponent::BuildChunks()
{
	if (!m_TerrainData ||
		m_TerrainData->Cells.empty())
	{
		return false;
	}

	int maxX =
		-1;

	int maxY =
		-1;

	//=================================================
	// Terrain範囲取得
	//=================================================

	for (const TerrainCellData& cell :
		m_TerrainData->Cells)
	{
		maxX =
			(std::max)(
				maxX,
				cell.Position.X);

		maxY =
			(std::max)(
				maxY,
				cell.Position.Y);
	}

	if (maxX < 0 ||
		maxY < 0)
	{
		return false;
	}

	//=================================================
	// Chunk数
	//=================================================

	const int chunkCountX =
		(maxX +
			TerrainChunk::CHUNK_SIZE) /
		TerrainChunk::CHUNK_SIZE;

	const int chunkCountY =
		(maxY +
			TerrainChunk::CHUNK_SIZE) /
		TerrainChunk::CHUNK_SIZE;

	if (chunkCountX <= 0 ||
		chunkCountY <= 0)
	{
		return false;
	}

	m_Chunks.reserve(
		static_cast<size_t>(
			chunkCountX *
			chunkCountY));

	//=================================================
	// Chunk生成
	//=================================================

	for (int chunkY = 0;
		chunkY < chunkCountY;
		++chunkY)
	{
		for (int chunkX = 0;
			chunkX < chunkCountX;
			++chunkX)
		{
			TerrainChunk chunk;

			chunk.Initialize(
				chunkX,
				chunkY);

			m_Chunks.push_back(
				std::move(
					chunk));
		}
	}

	//=================================================
	// CellをChunkへ登録
	//=================================================

	for (const TerrainCellData& cell :
		m_TerrainData->Cells)
	{
		if (cell.Position.X < 0 ||
			cell.Position.Y < 0)
		{
			continue;
		}

		const int chunkX =
			cell.Position.X /
			TerrainChunk::CHUNK_SIZE;

		const int chunkY =
			cell.Position.Y /
			TerrainChunk::CHUNK_SIZE;

		const int index =
			chunkY *
			chunkCountX +
			chunkX;

		if (index < 0 ||
			index >=
			static_cast<int>(
				m_Chunks.size()))
		{
			continue;
		}

		m_Chunks[
			static_cast<size_t>(
				index)].
			AddCell(
				&cell);
	}

	return
		!m_Chunks.empty();
}

//=====================================================
// Build Ground Meshes
//=====================================================

bool TerrainRendererComponent::
BuildGroundMeshes()
{
	if (m_Chunks.empty())
	{
		return false;
	}

	m_GroundMeshes.clear();

	m_GroundMeshes.resize(
		m_Chunks.size());

	for (size_t i = 0;
		i < m_Chunks.size();
		++i)
	{
		BuildGroundMesh(
			m_Chunks[i],
			m_GroundMeshes[i]);
	}

	return true;
}

//=====================================================
// Build Ground Mesh
//=====================================================
void TerrainRendererComponent::
BuildGroundMesh(
	const TerrainChunk& chunk,
	TerrainGroundChunkMesh& outMesh)
{
	outMesh.Clear();

	const auto& cells =
		chunk.GetCells();

	for (const TerrainCellData* cell :
		cells)
	{
		if (!cell)
		{
			continue;
		}

		if (!IsGroundTerrain(
			cell->Type))
		{
			continue;
		}

		//=================================================
		// Terrain Type → Texture Index
		//=================================================
		//
		// Shaderへ渡すTerrain種類。
		//
		// 0.0 = Grass
		// 1.0 = Dirt
		// 2.0 = Rock
		// 3.0 = Sand
		//
		// ps_4_0_level_9_3では整数入力に制限があるため、
		// floatとして渡す。
		//=================================================

		const float terrainIndex =
			GetTerrainTextureIndex(
				cell->Type);

		//=================================================
		// Grid → World
		//=================================================
		//
		// Grid X → World X
		// Grid Y → World Z
		//
		// Grid中心が整数座標である前提。
		//=================================================

		const float centerX =
			static_cast<float>(
				cell->Position.X) *
			m_CellSize;

		const float centerZ =
			static_cast<float>(
				cell->Position.Y) *
			m_CellSize;

		const float halfSize =
			m_CellSize *
			0.5f;

		const uint32_t baseVertex =
			static_cast<uint32_t>(
				outMesh.Vertices.size());

		//=================================================
		// Vertex 0
		//=================================================

		TerrainVertex vertex0;

		vertex0.Position =
			Vector3(
				centerX - halfSize,
				0.0f,
				centerZ - halfSize);

		vertex0.Normal =
			Vector3(
				0.0f,
				1.0f,
				0.0f);

		vertex0.TexCoord =
			Vector2(
				0.0f,
				0.0f);

		vertex0.TerrainIndex =
			terrainIndex;

		//=================================================
		// Vertex 1
		//=================================================

		TerrainVertex vertex1;

		vertex1.Position =
			Vector3(
				centerX + halfSize,
				0.0f,
				centerZ - halfSize);

		vertex1.Normal =
			Vector3(
				0.0f,
				1.0f,
				0.0f);

		vertex1.TexCoord =
			Vector2(
				1.0f,
				0.0f);

		vertex1.TerrainIndex =
			terrainIndex;

		//=================================================
		// Vertex 2
		//=================================================

		TerrainVertex vertex2;

		vertex2.Position =
			Vector3(
				centerX - halfSize,
				0.0f,
				centerZ + halfSize);

		vertex2.Normal =
			Vector3(
				0.0f,
				1.0f,
				0.0f);

		vertex2.TexCoord =
			Vector2(
				0.0f,
				1.0f);

		vertex2.TerrainIndex =
			terrainIndex;

		//=================================================
		// Vertex 3
		//=================================================

		TerrainVertex vertex3;

		vertex3.Position =
			Vector3(
				centerX + halfSize,
				0.0f,
				centerZ + halfSize);

		vertex3.Normal =
			Vector3(
				0.0f,
				1.0f,
				0.0f);

		vertex3.TexCoord =
			Vector2(
				1.0f,
				1.0f);

		vertex3.TerrainIndex =
			terrainIndex;

		//=================================================
		// Vertex追加
		//=================================================

		outMesh.Vertices.push_back(
			vertex0);

		outMesh.Vertices.push_back(
			vertex1);

		outMesh.Vertices.push_back(
			vertex2);

		outMesh.Vertices.push_back(
			vertex3);

		//=================================================
		// Index
		//=================================================
		//
		// Rendererでは
		// FrontCounterClockwise = FALSE
		// + BackFaceCulling
		//
		// 上面がFrontになるWinding。
		//=================================================

		// Triangle 1
		outMesh.Indices.push_back(
			baseVertex + 0);

		outMesh.Indices.push_back(
			baseVertex + 1);

		outMesh.Indices.push_back(
			baseVertex + 2);

		// Triangle 2
		outMesh.Indices.push_back(
			baseVertex + 2);

		outMesh.Indices.push_back(
			baseVertex + 1);

		outMesh.Indices.push_back(
			baseVertex + 3);
	}
}

bool TerrainRendererComponent::
BuildWaterMeshes()
{
	if (m_Chunks.empty())
	{
		return false;
	}

	m_WaterMeshes.clear();

	m_WaterMeshes.resize(
		m_Chunks.size());

	bool hasWater =
		false;

	for (size_t i = 0;
		i < m_Chunks.size();
		++i)
	{
		BuildWaterMesh(
			m_Chunks[i],
			m_WaterMeshes[i]);

		if (!m_WaterMeshes[i].
			IsEmpty())
		{
			hasWater =
				true;
		}
	}

	return hasWater;
}

void TerrainRendererComponent::
BuildWaterMesh(
	const TerrainChunk& chunk,
	TerrainWaterChunkMesh& outMesh)
{
	outMesh.Clear();

	const auto& cells =
		chunk.GetCells();

	//=================================================
	// Settings
	//=================================================

	constexpr float WATER_HEIGHT =
		0.02f;

	/**
	 * @brief 水深情報の分割数
	 *
	 * RiverBedと同じ4分割にすることで、
	 * 川底の細かな高低差を水面側でも拾う。
	 */
	constexpr int SUBDIVISION =
		4;

	for (const TerrainCellData* cell :
		cells)
	{
		if (!cell)
		{
			continue;
		}

		if (cell->Type !=
			TerrainType::Water)
		{
			continue;
		}

		//=================================================
		// Cell Range
		//=================================================

		const float cellCenterX =
			static_cast<float>(
				cell->Position.X);

		const float cellCenterY =
			static_cast<float>(
				cell->Position.Y);

		const float cellMinX =
			cellCenterX -
			0.5f;

		const float cellMinY =
			cellCenterY -
			0.5f;

		const float step =
			1.0f /
			static_cast<float>(
				SUBDIVISION);

		//=================================================
		// Subdivision
		//=================================================

		for (int y = 0;
			y < SUBDIVISION;
			++y)
		{
			for (int x = 0;
				x < SUBDIVISION;
				++x)
			{
				const float gridX0 =
					cellMinX +
					static_cast<float>(x) *
					step;

				const float gridX1 =
					cellMinX +
					static_cast<float>(x + 1) *
					step;

				const float gridY0 =
					cellMinY +
					static_cast<float>(y) *
					step;

				const float gridY1 =
					cellMinY +
					static_cast<float>(y + 1) *
					step;

				//=========================================
				// RiverBed Height
				//=========================================

				const float riverBedHeight00 =
					CalculateRiverBedHeight(
						gridX0,
						gridY0);

				const float riverBedHeight10 =
					CalculateRiverBedHeight(
						gridX1,
						gridY0);

				const float riverBedHeight01 =
					CalculateRiverBedHeight(
						gridX0,
						gridY1);

				const float riverBedHeight11 =
					CalculateRiverBedHeight(
						gridX1,
						gridY1);

				//=========================================
				// Water Depth
				//=========================================
				//
				// 水面 - 川底 = 水深
				//
				// 負値は異常値なので0へ制限する。
				//=========================================

				const float waterDepth00 =
					(std::max)(
						0.0f,
						WATER_HEIGHT -
						riverBedHeight00);

				const float waterDepth10 =
					(std::max)(
						0.0f,
						WATER_HEIGHT -
						riverBedHeight10);

				const float waterDepth01 =
					(std::max)(
						0.0f,
						WATER_HEIGHT -
						riverBedHeight01);

				const float waterDepth11 =
					(std::max)(
						0.0f,
						WATER_HEIGHT -
						riverBedHeight11);

				//=========================================
				// World Position
				//=========================================

				const float worldX0 =
					gridX0 *
					m_CellSize;

				const float worldX1 =
					gridX1 *
					m_CellSize;

				const float worldZ0 =
					gridY0 *
					m_CellSize;

				const float worldZ1 =
					gridY1 *
					m_CellSize;

				const uint32_t baseVertex =
					static_cast<uint32_t>(
						outMesh.Vertices.size());

				//=========================================
				// Vertex 0
				//=========================================

				WaterVertex vertex0;

				vertex0.Position =
					Vector3(
						worldX0,
						WATER_HEIGHT,
						worldZ0);

				vertex0.Normal =
					Vector3(
						0.0f,
						1.0f,
						0.0f);

				vertex0.TexCoord =
					Vector2(
						gridX0,
						gridY0);

				vertex0.WaterDepth =
					waterDepth00;

				//=========================================
				// Vertex 1
				//=========================================

				WaterVertex vertex1;

				vertex1.Position =
					Vector3(
						worldX1,
						WATER_HEIGHT,
						worldZ0);

				vertex1.Normal =
					Vector3(
						0.0f,
						1.0f,
						0.0f);

				vertex1.TexCoord =
					Vector2(
						gridX1,
						gridY0);

				vertex1.WaterDepth =
					waterDepth10;

				//=========================================
				// Vertex 2
				//=========================================

				WaterVertex vertex2;

				vertex2.Position =
					Vector3(
						worldX0,
						WATER_HEIGHT,
						worldZ1);

				vertex2.Normal =
					Vector3(
						0.0f,
						1.0f,
						0.0f);

				vertex2.TexCoord =
					Vector2(
						gridX0,
						gridY1);

				vertex2.WaterDepth =
					waterDepth01;

				//=========================================
				// Vertex 3
				//=========================================

				WaterVertex vertex3;

				vertex3.Position =
					Vector3(
						worldX1,
						WATER_HEIGHT,
						worldZ1);

				vertex3.Normal =
					Vector3(
						0.0f,
						1.0f,
						0.0f);

				vertex3.TexCoord =
					Vector2(
						gridX1,
						gridY1);

				vertex3.WaterDepth =
					waterDepth11;

				outMesh.Vertices.push_back(
					vertex0);

				outMesh.Vertices.push_back(
					vertex1);

				outMesh.Vertices.push_back(
					vertex2);

				outMesh.Vertices.push_back(
					vertex3);

				//=========================================
				// Index
				//=========================================
				//
				// 現在のTerrainと同じWinding。
				//=========================================

				outMesh.Indices.push_back(
					baseVertex + 0);

				outMesh.Indices.push_back(
					baseVertex + 1);

				outMesh.Indices.push_back(
					baseVertex + 2);

				outMesh.Indices.push_back(
					baseVertex + 2);

				outMesh.Indices.push_back(
					baseVertex + 1);

				outMesh.Indices.push_back(
					baseVertex + 3);
			}
		}
	}
}

bool TerrainRendererComponent::
BuildRiverBedMeshes()
{
	if (m_Chunks.empty())
	{
		return false;
	}

	m_RiverBedMeshes.clear();

	m_RiverBedMeshes.resize(
		m_Chunks.size());

	bool createdAny =
		false;

	for (size_t i = 0;
		i < m_Chunks.size();
		++i)
	{
		BuildRiverBedMesh(
			m_Chunks[i],
			m_RiverBedMeshes[i]);

		if (!m_RiverBedMeshes[i].
			IsEmpty())
		{
			createdAny =
				true;
		}
	}

	return createdAny;
}

void TerrainRendererComponent::
BuildRiverBedMesh(
	const TerrainChunk& chunk,
	TerrainChunkMesh& outMesh)
{
	outMesh.Clear();

	const auto& cells =
		chunk.GetCells();

	//=================================================
	// 水底細分化数
	//=================================================
	//
	// 1Cellを4x4 Quadへ分割する。
	//=================================================

	constexpr int SUBDIVISION =
		4;

	for (const TerrainCellData* cell :
		cells)
	{
		if (!cell)
		{
			continue;
		}

		if (cell->Type !=
			TerrainType::Water)
		{
			continue;
		}

		//=================================================
		// Cell範囲
		//=================================================

		const float cellCenterX =
			static_cast<float>(
				cell->Position.X);

		const float cellCenterY =
			static_cast<float>(
				cell->Position.Y);

		const float cellMinX =
			cellCenterX -
			0.5f;

		const float cellMinY =
			cellCenterY -
			0.5f;

		const float step =
			1.0f /
			static_cast<float>(
				SUBDIVISION);

		//=================================================
		// Subdivision
		//=================================================

		for (int y = 0;
			y < SUBDIVISION;
			++y)
		{
			for (int x = 0;
				x < SUBDIVISION;
				++x)
			{
				const float gridX0 =
					cellMinX +
					static_cast<float>(
						x) *
					step;

				const float gridX1 =
					cellMinX +
					static_cast<float>(
						x + 1) *
					step;

				const float gridY0 =
					cellMinY +
					static_cast<float>(
						y) *
					step;

				const float gridY1 =
					cellMinY +
					static_cast<float>(
						y + 1) *
					step;

				//=========================================
				// 頂点ごとの高さ
				//=========================================

				const float height00 =
					CalculateRiverBedHeight(
						gridX0,
						gridY0);

				const float height10 =
					CalculateRiverBedHeight(
						gridX1,
						gridY0);

				const float height01 =
					CalculateRiverBedHeight(
						gridX0,
						gridY1);

				const float height11 =
					CalculateRiverBedHeight(
						gridX1,
						gridY1);

				//=========================================
				// World座標
				//=========================================

				const float worldX0 =
					gridX0 *
					m_CellSize;

				const float worldX1 =
					gridX1 *
					m_CellSize;

				const float worldZ0 =
					gridY0 *
					m_CellSize;

				const float worldZ1 =
					gridY1 *
					m_CellSize;

				const uint32_t baseVertex =
					static_cast<uint32_t>(
						outMesh.Vertices.size());

				//=================================================
				// Normal
				//=================================================

				const Vector3 normal00 =
					CalculateRiverBedNormal(
						gridX0,
						gridY0);

				const Vector3 normal10 =
					CalculateRiverBedNormal(
						gridX1,
						gridY0);

				const Vector3 normal01 =
					CalculateRiverBedNormal(
						gridX0,
						gridY1);

				const Vector3 normal11 =
					CalculateRiverBedNormal(
						gridX1,
						gridY1);

				//=========================================
				// Vertex 0
				//=========================================

				VertexData vertex0;

				vertex0.Position =
					Vector3(
						worldX0,
						height00,
						worldZ0);

				vertex0.Normal =
					normal00;

				vertex0.TexCoord =
					Vector2(
						gridX0,
						gridY0);

				//=========================================
				// Vertex 1
				//=========================================

				VertexData vertex1;

				vertex1.Position =
					Vector3(
						worldX1,
						height10,
						worldZ0);

				vertex1.Normal =
					normal10;

				vertex1.TexCoord =
					Vector2(
						gridX1,
						gridY0);

				//=========================================
				// Vertex 2
				//=========================================

				VertexData vertex2;

				vertex2.Position =
					Vector3(
						worldX0,
						height01,
						worldZ1);

				vertex2.Normal =
					normal01;

				vertex2.TexCoord =
					Vector2(
						gridX0,
						gridY1);

				//=========================================
				// Vertex 3
				//=========================================

				VertexData vertex3;

				vertex3.Position =
					Vector3(
						worldX1,
						height11,
						worldZ1);

				vertex3.Normal =
					normal11;

				vertex3.TexCoord =
					Vector2(
						gridX1,
						gridY1);

				outMesh.Vertices.push_back(
					vertex0);

				outMesh.Vertices.push_back(
					vertex1);

				outMesh.Vertices.push_back(
					vertex2);

				outMesh.Vertices.push_back(
					vertex3);

				//=========================================
				// Index
				//=========================================

				outMesh.Indices.push_back(
					baseVertex + 0);

				outMesh.Indices.push_back(
					baseVertex + 1);

				outMesh.Indices.push_back(
					baseVertex + 2);

				outMesh.Indices.push_back(
					baseVertex + 2);

				outMesh.Indices.push_back(
					baseVertex + 1);

				outMesh.Indices.push_back(
					baseVertex + 3);
			}
		}
	}
}

//=====================================================
// GPU Resources
//=====================================================
bool TerrainRendererComponent::
BuildGpuResources()
{
	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
	{
		return false;
	}

	bool createdAny =
		false;

	auto createGroundGpuMeshes =
		[device, &createdAny](
			std::vector<TerrainGroundChunkMesh>& meshes)
		{
			for (TerrainGroundChunkMesh& terrainMesh :
				meshes)
			{
				if (terrainMesh.IsEmpty())
				{
					continue;
				}

				auto mesh =
					std::make_shared<MeshBuffer>();

				const bool created =
					mesh->Create(
						device,
						terrainMesh.Vertices.data(),
						static_cast<uint32_t>(
							sizeof(
								TerrainVertex)),
						static_cast<uint32_t>(
							terrainMesh.Vertices.size()),
						terrainMesh.Indices.data(),
						static_cast<uint32_t>(
							terrainMesh.Indices.size()));

				if (!created)
				{
					continue;
				}

				terrainMesh.Mesh =
					std::move(mesh);

				createdAny =
					true;
			}
		};

	auto createGpuMeshes =
		[device, &createdAny](
			std::vector<TerrainChunkMesh>& meshes)
		{
			for (TerrainChunkMesh& terrainMesh :
				meshes)
			{
				if (terrainMesh.IsEmpty())
				{
					continue;
				}

				auto mesh =
					std::make_shared<MeshBuffer>();

				const bool created =
					mesh->Create(
						device,
						terrainMesh.Vertices.data(),
						static_cast<uint32_t>(
							sizeof(
								VertexData)),
						static_cast<uint32_t>(
							terrainMesh.Vertices.size()),
						terrainMesh.Indices.data(),
						static_cast<uint32_t>(
							terrainMesh.Indices.size()));

				if (!created)
				{
					continue;
				}

				terrainMesh.Mesh =
					std::move(
						mesh);

				createdAny =
					true;
			}
		};

	createGroundGpuMeshes(
		m_GroundMeshes);

	createGpuMeshes(
		m_RiverBedMeshes);

	auto createWaterGpuMeshes =
		[device, &createdAny](
			std::vector<TerrainWaterChunkMesh>& meshes)
		{
			for (TerrainWaterChunkMesh& waterMesh :
				meshes)
			{
				if (waterMesh.IsEmpty())
				{
					continue;
				}

				auto mesh =
					std::make_shared<MeshBuffer>();

				const bool created =
					mesh->Create(
						device,
						waterMesh.Vertices.data(),

						// Water専用Vertexのstride
						static_cast<uint32_t>(
							sizeof(WaterVertex)),

						static_cast<uint32_t>(
							waterMesh.Vertices.size()),

						waterMesh.Indices.data(),

						static_cast<uint32_t>(
							waterMesh.Indices.size()));

				if (!created)
				{
					continue;
				}

				waterMesh.Mesh =
					std::move(mesh);

				createdAny =
					true;
			}
		};

	createWaterGpuMeshes(
		m_WaterMeshes);

	return createdAny;
}

//=====================================================
// Ground Terrain
//=====================================================

bool TerrainRendererComponent::
IsGroundTerrain(
	TerrainType type)
{
	switch (type)
	{
	case TerrainType::Grass:
	case TerrainType::Dirt:
	case TerrainType::Rock:
	case TerrainType::Sand:
	case TerrainType::Forest:

		return true;

	case TerrainType::Water:

		// Waterは専用Meshで後ほど描画。
		return false;

	default:

		return false;
	}
}

//=====================================================
// Update Chunk LOD
//=====================================================
void TerrainRendererComponent::
UpdateChunkLOD()
{
	const float nearDistanceSq =
		m_NearDistance *
		m_NearDistance;

	const float middleDistanceSq =
		m_MiddleDistance *
		m_MiddleDistance;

	const float farDistanceSq =
		m_FarDistance *
		m_FarDistance;

	for (TerrainChunk& chunk :
		m_Chunks)
	{
		const GridPosition centerGrid =
			chunk.GetCenterGrid();

		//=============================================
		// Chunk中央World座標
		//=============================================

		const Vector3 chunkCenter
		{
			static_cast<float>(
				centerGrid.X) *
				m_CellSize,

			0.0f,

			static_cast<float>(
				centerGrid.Y) *
				m_CellSize
		};

		const float dx =
			chunkCenter.x -
			m_CameraPosition.x;

		const float dz =
			chunkCenter.z -
			m_CameraPosition.z;

		// Terrainなので高さ方向は無視する。
		const float distanceSq =
			dx * dx +
			dz * dz;

		//=============================================
		// LOD
		//=============================================

		if (distanceSq <=
			nearDistanceSq)
		{
			chunk.SetLOD(
				TerrainChunkLOD::Near);
		}
		else if (distanceSq <=
			middleDistanceSq)
		{
			chunk.SetLOD(
				TerrainChunkLOD::Middle);
		}
		else if (distanceSq <=
			farDistanceSq)
		{
			chunk.SetLOD(
				TerrainChunkLOD::Far);
		}
		else
		{
			chunk.SetLOD(
				TerrainChunkLOD::Culled);
		}
	}
}

//=====================================================
// Calculate RiverBed Cell Height
//=====================================================

float TerrainRendererComponent::
CalculateRiverBedCellHeight(
	const TerrainCellData& cell) const
{
	//=================================================
	// Settings
	//=================================================

	constexpr float WATER_HEIGHT =
		0.02f;

	// 岸付近の水深
	constexpr float SHALLOW_DEPTH =
		0.08f;

	// 川中央付近の最大水深
	constexpr float DEEP_DEPTH =
		1.00f;

	// 岸から何マスで最大深度へ到達するか
	constexpr int MAX_SHORE_DISTANCE =
		4;

	// 小さな地形の凹凸
	constexpr float NOISE_STRENGTH =
		0.04f;

	//=================================================
	// Shore Distance
	//=================================================

	const int shoreDistance =
		CalculateDistanceFromShore(
			cell.Position,
			MAX_SHORE_DISTANCE);

	float t =
		static_cast<float>(
			shoreDistance) /
		static_cast<float>(
			MAX_SHORE_DISTANCE);

	// 0～1に制限
	if (t < 0.0f)
	{
		t =
			0.0f;
	}
	else if (t > 1.0f)
	{
		t =
			1.0f;
	}

	//=================================================
	// SmoothStep
	//=================================================
	//
	// 岸から中央までを線形ではなく滑らかに深くする。
	//=================================================

	const float smoothT =
		t *
		t *
		(
			3.0f -
			2.0f *
			t
			);

	const float depth =
		SHALLOW_DEPTH +
		(
			DEEP_DEPTH -
			SHALLOW_DEPTH
			) *
		smoothT;

	//=================================================
	// Deterministic Noise
	//=================================================
	//
	// Grid座標だけから固定値を作るため、
	// 起動するたびに川底形状が変化しない。
	//=================================================

	uint32_t hash =
		static_cast<uint32_t>(
			cell.Position.X *
			73856093 ^
			cell.Position.Y *
			19349663);

	hash ^=
		hash >> 13;

	hash *=
		1274126177u;

	const float random01 =
		static_cast<float>(
			hash &
			0xFFFFu) /
		65535.0f;

	const float noise =
		(
			random01 *
			2.0f -
			1.0f
			) *
		NOISE_STRENGTH;

	//=================================================
	// Final Height
	//=================================================

	return
		WATER_HEIGHT -
		depth +
		noise;
}

//=====================================================
// Calculate RiverBed Vertex Height
//=====================================================

float TerrainRendererComponent::
CalculateRiverBedVertexHeight(
	int vertexGridX,
	int vertexGridY) const
{
	if (!m_TerrainData)
	{
		return 0.0f;
	}

	float totalHeight =
		0.0f;

	int count =
		0;

	//=================================================
	// 1頂点を共有する最大4Cell
	//=================================================
	//
	//             vertex
	//               +
	//
	//     [-1,-1] | [0,-1]
	//     -----------------
	//     [-1, 0] | [0, 0]
	//
	//=================================================

	const GridPosition cellOffsets[4] =
	{
		GridPosition
		{
			-1,
			-1
		},

		GridPosition
		{
			0,
			-1
		},

		GridPosition
		{
			-1,
			0
		},

		GridPosition
		{
			0,
			0
		}
	};

	for (const GridPosition& offset :
		cellOffsets)
	{
		const GridPosition cellPosition
		{
			vertexGridX +
				offset.X,

			vertexGridY +
				offset.Y
		};

		const TerrainCellData* cell =
			m_TerrainData->
			FindCell(
				cellPosition);

		if (!cell)
		{
			continue;
		}

		// Water Cellだけ平均対象
		if (cell->Type !=
			TerrainType::Water)
		{
			continue;
		}

		totalHeight +=
			CalculateRiverBedCellHeight(
				*cell);

		++count;
	}

	//=================================================
	// Water Cellが周囲に存在しない場合
	//=================================================

	if (count <= 0)
	{
		return 0.0f;
	}

	return
		totalHeight /
		static_cast<float>(
			count);
}

//=====================================================
// Calculate Distance From Shore
//=====================================================
int TerrainRendererComponent::
CalculateDistanceFromShore(
	const GridPosition& position,
	int maxDistance) const
{
	if (!m_TerrainData)
	{
		return 0;
	}

	//=================================================
	// Manhattan距離で岸を探索する
	//=================================================

	for (int distance = 1;
		distance <= maxDistance;
		++distance)
	{
		for (int offsetX = -distance;
			offsetX <= distance;
			++offsetX)
		{
			const int remaining =
				distance -
				std::abs(
					offsetX);

			// remaining == 0 の場合は
			// 同じ座標を2回調べない。
			const int signCount =
				remaining == 0
				? 1
				: 2;

			for (int signIndex = 0;
				signIndex < signCount;
				++signIndex)
			{
				const int sign =
					signIndex == 0
					? -1
					: 1;

				const int offsetY =
					remaining *
					sign;

				const GridPosition checkPosition
				{
					position.X +
						offsetX,

					position.Y +
						offsetY
				};

				const TerrainCellData* cell =
					m_TerrainData->
					FindCell(
						checkPosition);

				// Map外は岸として扱う
				if (!cell)
				{
					return distance;
				}

				// Water以外を見つけたら岸
				if (cell->Type !=
					TerrainType::Water)
				{
					return distance;
				}
			}
		}
	}

	return maxDistance;
}

float TerrainRendererComponent::
CalculateRiverBedHeight(
	float gridX,
	float gridY) const
{
	//=================================================
	// 基本設定
	//=================================================

	constexpr float WATER_HEIGHT =
		0.02f;

	// 基本となる水深
	constexpr float BASE_DEPTH =
		0.22f;

	// 凹凸の高さ
	constexpr float NOISE_HEIGHT =
		0.06f;

	// 凹凸の細かさ
	constexpr float NOISE_FREQUENCY =
		1.3f;

	//=================================================
	// Procedural Height
	//=================================================
	//
	// 2方向の波を重ねることで、
	// 単純な規則模様になりにくくする。
	//=================================================

	const float waveA =
		std::sin(
			gridX *
			NOISE_FREQUENCY +
			gridY *
			0.7f);

	const float waveB =
		std::sin(
			gridX *
			0.55f -
			gridY *
			NOISE_FREQUENCY *
			1.4f);

	const float noise =
		(
			waveA *
			0.65f +
			waveB *
			0.35f
			) *
		NOISE_HEIGHT;

	return
		WATER_HEIGHT -
		BASE_DEPTH +
		noise;
}

//=====================================================
// Calculate RiverBed Normal
//=====================================================
//=====================================================
// Calculate RiverBed Normal
//=====================================================

Vector3 TerrainRendererComponent::
CalculateRiverBedNormal(
	float gridX,
	float gridY) const
{
	//=================================================
	// Settings
	//=================================================

	/**
	 * @brief 高さを調べる間隔
	 *
	 * SUBDIVISION = 4 の場合、
	 * 1 / 4 = 0.25 が基本値。
	 */
	constexpr float SAMPLE_OFFSET =
		0.20f;

	/**
	 * @brief 水底法線の傾斜強調
	 *
	 * Geometryそのものは変形させず、
	 * Lighting用Normalだけ横方向へ強調する。
	 *
	 * 1.0 = 実際の地形と同じ
	 * 2.0 = 傾斜を約2倍強調
	 */
	constexpr float NORMAL_SLOPE_STRENGTH =
		1.25f;

	//=================================================
	// Surrounding Heights
	//=================================================

	const float heightLeft =
		CalculateRiverBedHeight(
			gridX - SAMPLE_OFFSET,
			gridY);

	const float heightRight =
		CalculateRiverBedHeight(
			gridX + SAMPLE_OFFSET,
			gridY);

	const float heightBack =
		CalculateRiverBedHeight(
			gridX,
			gridY - SAMPLE_OFFSET);

	const float heightFront =
		CalculateRiverBedHeight(
			gridX,
			gridY + SAMPLE_OFFSET);

	//=================================================
	// World Space Sampling Distance
	//=================================================

	const float worldStep =
		SAMPLE_OFFSET *
		m_CellSize *
		2.0f;

	//=================================================
	// Tangent X
	//=================================================
	//
	// 左 → 右方向の地形傾斜。
	//=================================================

	const Vector3 tangentX(
		worldStep,
		heightRight -
		heightLeft,
		0.0f);

	//=================================================
	// Tangent Z
	//=================================================
	//
	// 奥 → 手前方向の地形傾斜。
	//=================================================

	const Vector3 tangentZ(
		0.0f,
		heightFront -
		heightBack,
		worldStep);

	//=================================================
	// Cross Product
	//=================================================
	//
	// tangentZ × tangentX
	//
	// +Y方向を向くNormalを生成する。
	//=================================================

	Vector3 normal(
		tangentZ.y *
		tangentX.z -
		tangentZ.z *
		tangentX.y,

		tangentZ.z *
		tangentX.x -
		tangentZ.x *
		tangentX.z,

		tangentZ.x *
		tangentX.y -
		tangentZ.y *
		tangentX.x);

	//=================================================
	// Toon Lighting用に傾斜を強調
	//=================================================
	//
	// Y方向はそのままにして、
	// X/Z方向だけを強くする。
	//
	// Geometry自体の高さは変わらない。
	//=================================================

	normal.x *=
		NORMAL_SLOPE_STRENGTH;

	normal.z *=
		NORMAL_SLOPE_STRENGTH;

	//=================================================
	// Normalize
	//=================================================

	const float lengthSq =
		normal.x *
		normal.x +
		normal.y *
		normal.y +
		normal.z *
		normal.z;

	if (lengthSq <=
		0.000001f)
	{
		return Vector3(
			0.0f,
			1.0f,
			0.0f);
	}

	const float inverseLength =
		1.0f /
		std::sqrt(
			lengthSq);

	normal.x *=
		inverseLength;

	normal.y *=
		inverseLength;

	normal.z *=
		inverseLength;

	//=================================================
	// Safety
	//=================================================
	//
	// 下向きNormalになってしまった場合は反転する。
	//=================================================

	if (normal.y < 0.0f)
	{
		normal.x =
			-normal.x;

		normal.y =
			-normal.y;

		normal.z =
			-normal.z;
	}

	return normal;
}

//=====================================================
// Debug UI
//=====================================================

void TerrainRendererComponent::
DrawDebugUI()
{
	ImGui::Text(
		"Terrain Renderer");

	ImGui::Separator();

	ImGui::Text(
		"Chunks: %zu",
		m_Chunks.size());

	size_t totalVertices =
		0;

	size_t totalIndices =
		0;

	size_t gpuMeshes =
		0;

	for (const TerrainGroundChunkMesh& mesh :
		m_GroundMeshes)
	{
		totalVertices +=
			mesh.Vertices.size();

		totalIndices +=
			mesh.Indices.size();

		if (mesh.HasGpuMesh())
		{
			++gpuMeshes;
		}
	}

	ImGui::Text(
		"GPU Meshes: %zu",
		gpuMeshes);

	ImGui::Text(
		"Vertices: %zu",
		totalVertices);

	ImGui::Text(
		"Indices: %zu",
		totalIndices);

	ImGui::Text(
		"Cell Size: %.2f",
		m_CellSize);

	size_t waterVertices =
		0;

	size_t waterIndices =
		0;

	size_t waterGpuMeshes =
		0;

	for (const TerrainWaterChunkMesh& mesh :
		m_WaterMeshes)
	{
		waterVertices +=
			mesh.Vertices.size();

		waterIndices +=
			mesh.Indices.size();

		if (mesh.HasGpuMesh())
		{
			++waterGpuMeshes;
		}
	}

	ImGui::Separator();

	ImGui::Text(
		"Water GPU Meshes: %zu",
		waterGpuMeshes);

	ImGui::Text(
		"Water Vertices: %zu",
		waterVertices);

	ImGui::Text(
		"Water Indices: %zu",
		waterIndices);

	size_t riverBedVertices =
		0;

	size_t riverBedIndices =
		0;

	size_t riverBedGpuMeshes =
		0;

	for (const TerrainChunkMesh& mesh :
		m_RiverBedMeshes)
	{
		riverBedVertices +=
			mesh.Vertices.size();

		riverBedIndices +=
			mesh.Indices.size();

		if (mesh.HasGpuMesh())
		{
			++riverBedGpuMeshes;
		}
	}

	ImGui::Separator();

	ImGui::Text(
		"RiverBed GPU Meshes: %zu",
		riverBedGpuMeshes);

	ImGui::Text(
		"RiverBed Vertices: %zu",
		riverBedVertices);

	ImGui::Text(
		"RiverBed Indices: %zu",
		riverBedIndices);
}
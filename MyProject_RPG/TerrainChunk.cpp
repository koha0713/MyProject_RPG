#include "TerrainChunk.h"

//=====================================================
// Initialize
//=====================================================

void TerrainChunk::Initialize(
	int chunkX,
	int chunkY)
{
	m_ChunkX =
		chunkX;

	m_ChunkY =
		chunkY;

	m_Cells.clear();

	// Å‘å8x8 = 64CellB
	// Äallocation‚ð–hŽ~‚·‚éB
	m_Cells.reserve(
		CHUNK_SIZE *
		CHUNK_SIZE);
}

//=====================================================
// Clear
//=====================================================

void TerrainChunk::Clear()
{
	m_Cells.clear();
}

//=====================================================
// Add Cell
//=====================================================

void TerrainChunk::AddCell(
	const TerrainCellData* cell)
{
	if (!cell)
	{
		return;
	}

	// ”O‚Ì‚½‚ßChunk”ÍˆÍ‚ðŒŸØ‚·‚éB
	if (!Contains(
		cell->Position))
	{
		return;
	}

	m_Cells.push_back(
		cell);
}

//=====================================================
// Contains
//=====================================================

bool TerrainChunk::Contains(
	const GridPosition& position) const
{
	const GridPosition start =
		GetStartGrid();

	const int endX =
		start.X +
		CHUNK_SIZE;

	const int endY =
		start.Y +
		CHUNK_SIZE;

	return
		position.X >= start.X &&
		position.Y >= start.Y &&
		position.X < endX &&
		position.Y < endY;
}

//=====================================================
// Start Grid
//=====================================================

GridPosition TerrainChunk::GetStartGrid() const
{
	return GridPosition
	{
		m_ChunkX *
			CHUNK_SIZE,

		m_ChunkY *
			CHUNK_SIZE
	};
}

//=====================================================
// Center Grid
//=====================================================

GridPosition TerrainChunk::GetCenterGrid() const
{
	const GridPosition start =
		GetStartGrid();

	return GridPosition
	{
		start.X +
			CHUNK_SIZE / 2,

		start.Y +
			CHUNK_SIZE / 2
	};
}
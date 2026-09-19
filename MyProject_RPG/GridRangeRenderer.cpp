#include "GridRangeRenderer.h"

#include "GridMap.h"
#include "PrimitiveRenderer.h"

void GridRangeRenderer::DrawCells(
	const GridMap& gridMap,
	const std::vector<GridPosition>& cells,
	const Color& color,
	float heightOffset)
{
	if (cells.empty())
	{
		return;
	}

	const float cellSize =
		gridMap.GetCellSize();

	const float halfCell =
		cellSize * 0.5f;

	std::vector<LineVertex>
		vertices;

	// 1Cell = 2Triangle = 6Vertex
	vertices.reserve(
		cells.size() * 6);

	for (const GridPosition& grid :
		cells)
	{
		if (!gridMap.IsInside(
			grid))
		{
			continue;
		}

		const Vector3 center =
			gridMap.GridToWorld(
				grid);

		const float left =
			center.x - halfCell;

		const float right =
			center.x + halfCell;

		const float top =
			center.z - halfCell;

		const float bottom =
			center.z + halfCell;

		const float y =
			center.y + heightOffset;

		//=============================================
		// Triangle 1
		//=============================================

		vertices.push_back(
			{
				Vector3(
					left,
					y,
					top),
				color
			});

		vertices.push_back(
			{
				Vector3(
					right,
					y,
					top),
				color
			});

		vertices.push_back(
			{
				Vector3(
					left,
					y,
					bottom),
				color
			});

		//=============================================
		// Triangle 2
		//=============================================

		vertices.push_back(
			{
				Vector3(
					left,
					y,
					bottom),
				color
			});

		vertices.push_back(
			{
				Vector3(
					right,
					y,
					top),
				color
			});

		vertices.push_back(
			{
				Vector3(
					right,
					y,
					bottom),
				color
			});
	}

	PrimitiveRenderer::DrawTriangles(
		vertices);
}
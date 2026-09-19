#include "GridRenderer.h"

#include <vector>

#include "GridMap.h"
#include "PrimitiveRenderer.h"

//=====================================================
// Draw
//=====================================================

void GridRenderer::Draw(
	const GridMap& gridMap,
	const Color& color,
	float heightOffset)
{
	const int width =
		gridMap.GetWidth();

	const int height =
		gridMap.GetHeight();

	const float cellSize =
		gridMap.GetCellSize();

	if (width <= 0 ||
		height <= 0 ||
		cellSize <= 0.0f)
	{
		return;
	}

	const float halfCell =
		cellSize *
		0.5f;

	//=================================================
	// Gridã´äE
	//=================================================

	const float minX =
		-halfCell;

	const float minZ =
		-halfCell;

	const float maxX =
		static_cast<float>(
			width) *
		cellSize -
		halfCell;

	const float maxZ =
		static_cast<float>(
			height) *
		cellSize -
		halfCell;

	std::vector<LineVertex>
		vertices;

	// â°ê¸ + ècê¸
	vertices.reserve(
		static_cast<size_t>(
			(width + 1 +
				height + 1) * 2));

	//=================================================
	// Xï˚å¸Line
	//=================================================

	for (int y = 0;
		y <= height;
		++y)
	{
		const float z =
			minZ +
			static_cast<float>(
				y) *
			cellSize;

		vertices.push_back(
			{
				Vector3(
					minX,
					heightOffset,
					z),
				color
			});

		vertices.push_back(
			{
				Vector3(
					maxX,
					heightOffset,
					z),
				color
			});
	}

	//=================================================
	// Zï˚å¸Line
	//=================================================

	for (int x = 0;
		x <= width;
		++x)
	{
		const float worldX =
			minX +
			static_cast<float>(
				x) *
			cellSize;

		vertices.push_back(
			{
				Vector3(
					worldX,
					heightOffset,
					minZ),
				color
			});

		vertices.push_back(
			{
				Vector3(
					worldX,
					heightOffset,
					maxZ),
				color
			});
	}

	PrimitiveRenderer::DrawLines(
		vertices);
}
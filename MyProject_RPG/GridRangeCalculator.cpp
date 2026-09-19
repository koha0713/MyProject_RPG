#include "GridRangeCalculator.h"

#include <queue>
#include <map>
#include <utility>

#include "GridMap.h"

//=====================================================
// Move Range
//=====================================================

std::vector<GridPosition>
GridRangeCalculator::CalculateMoveRange(
	const GridMap& gridMap,
	const GridPosition& start,
	int movePoints)
{
	std::vector<GridPosition>
		result;

	if (movePoints <= 0)
	{
		return result;
	}

	struct Node
	{
		GridPosition Position;
		int Cost;
	};

	std::queue<Node>
		openQueue;

	std::map<
		std::pair<int, int>,
		int>
		visited;

	openQueue.push(
		{
			start,
			0
		});

	visited[
	{
		start.X,
			start.Y
	}] = 0;

	const GridPosition directions[] =
	{
		{ 1, 0 },
		{-1, 0 },
		{ 0, 1 },
		{ 0,-1 }
	};

	while (!openQueue.empty())
	{
		const Node current =
			openQueue.front();

		openQueue.pop();

		//=================================================
		// 最大移動力まで到達済み
		//=================================================

		if (current.Cost >=
			movePoints)
		{
			continue;
		}

		for (const GridPosition& direction :
			directions)
		{
			const GridPosition next
			{
				current.Position.X +
					direction.X,

				current.Position.Y +
					direction.Y
			};

			if (!gridMap.IsInside(
				next))
			{
				continue;
			}

			//=================================================
			// 実際に移動可能か確認
			//=================================================

			if (!gridMap.CanMoveTo(
				next))
			{
				continue;
			}

			const int nextCost =
				current.Cost + 1;

			const auto key =
				std::make_pair(
					next.X,
					next.Y);

			auto found =
				visited.find(
					key);

			// 既により低コストで到達済み
			if (found !=
				visited.end() &&
				found->second <=
				nextCost)
			{
				continue;
			}

			visited[key] =
				nextCost;

			openQueue.push(
				{
					next,
					nextCost
				});

			result.push_back(
				next);
		}
	}

	return result;
}

//=====================================================
// Attack Range
//=====================================================

std::vector<GridPosition>
GridRangeCalculator::CalculateAttackRange(
	const GridMap& gridMap,
	const GridPosition& center,
	int attackRange)
{
	std::vector<GridPosition>
		result;

	if (attackRange <= 0)
	{
		return result;
	}

	for (int y =
		-attackRange;
		y <= attackRange;
		++y)
	{
		for (int x =
			-attackRange;
			x <= attackRange;
			++x)
		{
			const int distance =
				std::abs(x) +
				std::abs(y);

			// 中心自身は含めない
			if (distance == 0)
			{
				continue;
			}

			// Manhattan距離でAttackRange以内
			if (distance >
				attackRange)
			{
				continue;
			}

			const GridPosition position
			{
				center.X + x,
				center.Y + y
			};

			if (!gridMap.IsInside(
				position))
			{
				continue;
			}

			result.push_back(
				position);
		}
	}

	return result;
}
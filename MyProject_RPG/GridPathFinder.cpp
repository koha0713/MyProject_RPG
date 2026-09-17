#include "GridPathFinder.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <queue>
#include <set>
#include <utility>

#include "GridMap.h"

namespace
{
	/**
	 * @brief GridPositionをMap / Setで使用するためのKey
	 */
	using GridKey =
		std::pair<int, int>;

	/**
	 * @brief GridPosition → GridKey
	 */
	GridKey ToKey(
		const GridPosition& position)
	{
		return
		{
			position.X,
			position.Y
		};
	}

	/**
	 * @brief GridKey → GridPosition
	 */
	GridPosition ToPosition(
		const GridKey& key)
	{
		return
		{
			key.first,
			key.second
		};
	}

	/**
	 * @brief A*のOpenListに格納するNode
	 */
	struct PathNode
	{
		GridPosition Position;

		// StartからこのNodeまでの実コスト
		int G = 0;

		// Goalまでの推定コスト
		int H = 0;

		/**
		 * @brief 総評価値
		 */
		int GetF() const
		{
			return G + H;
		}
	};

	/**
	 * @brief priority_queue用比較
	 *
	 * F値が小さいNodeを優先する。
	 */
	struct PathNodeCompare
	{
		bool operator()(
			const PathNode& lhs,
			const PathNode& rhs) const
		{
			// F値が小さい方を優先
			if (lhs.GetF() !=
				rhs.GetF())
			{
				return
					lhs.GetF() >
					rhs.GetF();
			}

			// Fが同じ場合は、
			// Goalに近い方を優先する。
			return
				lhs.H >
				rhs.H;
		}
	};
}

//=====================================================
// Path Finding
//=====================================================

std::vector<GridPosition>
GridPathFinder::FindPath(
	GridMap& gridMap,
	const GridPosition& start,
	const GridPosition& goal)
{
	//=================================================
	// Start == Goal
	//=================================================

	if (start == goal)
	{
		return
		{
			start
		};
	}

	//=================================================
	// Goal確認
	//=================================================
	//
	// Player自身がいるStartはOccupantが存在するため
	// CanMoveTo()では判定しない。
	//
	// Goalは実際に進入可能なCellである必要がある。
	//

	if (!gridMap.CanMoveTo(
		goal))
	{
		return {};
	}

	//=================================================
	// Open List
	//=================================================

	std::priority_queue<
		PathNode,
		std::vector<PathNode>,
		PathNodeCompare>
		openList;

	//=================================================
	// Cost情報
	//=================================================
	//
	// GridKey
	// ↓
	// Startからその地点までに判明している最小コスト
	//

	std::map<GridKey, int>
		gScore;

	//=================================================
	// Parent情報
	//=================================================
	//
	// GoalからStartへ経路を復元するため使用する。
	//

	std::map<GridKey, GridKey>
		cameFrom;

	//=================================================
	// Closed List
	//=================================================

	std::set<GridKey>
		closedList;

	//=================================================
	// Start登録
	//=================================================

	PathNode startNode;

	startNode.Position =
		start;

	startNode.G =
		0;

	startNode.H =
		CalculateHeuristic(
			start,
			goal);

	openList.push(
		startNode);

	gScore[
		ToKey(start)] =
		0;

		//=================================================
		// 4方向
		//=================================================

		const GridPosition directions[] =
		{
			{  0, -1 },
			{  0,  1 },
			{ -1,  0 },
			{  1,  0 }
		};

		//=================================================
		// A*
		//=================================================

		while (!openList.empty())
		{
			const PathNode current =
				openList.top();

			openList.pop();

			const GridKey currentKey =
				ToKey(
					current.Position);

			// 既に確定済みなら無視
			if (closedList.contains(
				currentKey))
			{
				continue;
			}

			//=============================================
			// Goal到達
			//=============================================

			if (current.Position ==
				goal)
			{
				std::vector<GridPosition>
					path;

				GridKey pathKey =
					currentKey;

				// Goalから逆向きに格納
				path.push_back(
					ToPosition(
						pathKey));

				while (pathKey !=
					ToKey(start))
				{
					auto parentIt =
						cameFrom.find(
							pathKey);

					if (parentIt ==
						cameFrom.end())
					{
						// 経路情報が壊れている場合
						return {};
					}

					pathKey =
						parentIt->second;

					path.push_back(
						ToPosition(
							pathKey));
				}

				// Goal → Start になっているため反転
				std::reverse(
					path.begin(),
					path.end());

				return path;
			}

			// 現在地点を確定済みにする
			closedList.insert(
				currentKey);

			//=============================================
			// 隣接Cell探索
			//=============================================

			for (const GridPosition& direction :
				directions)
			{
				const GridPosition nextPosition
				{
					current.Position.X +
						direction.X,

					current.Position.Y +
						direction.Y
				};

				const GridKey nextKey =
					ToKey(
						nextPosition);

				// 既に確定済み
				if (closedList.contains(
					nextKey))
				{
					continue;
				}

				//=========================================
				// 通行可能判定
				//=========================================

				if (!gridMap.CanMoveTo(
					nextPosition))
				{
					continue;
				}

				//=========================================
				// Cost計算
				//=========================================
				//
				// 現在は全Gridの移動コストを1とする。
				//

				const int tentativeG =
					current.G + 1;

				auto gScoreIt =
					gScore.find(
						nextKey);

				// 既により短い経路が存在する場合は無視
				if (gScoreIt !=
					gScore.end() &&
					tentativeG >=
					gScoreIt->second)
				{
					continue;
				}

				//=========================================
				// より良い経路を登録
				//=========================================

				cameFrom[
					nextKey] =
					currentKey;

					gScore[
						nextKey] =
						tentativeG;

						PathNode nextNode;

						nextNode.Position =
							nextPosition;

						nextNode.G =
							tentativeG;

						nextNode.H =
							CalculateHeuristic(
								nextPosition,
								goal);

						openList.push(
							nextNode);
			}
		}

		//=================================================
		// 経路なし
		//=================================================

		return {};
}

//=====================================================
// Heuristic
//=====================================================

int GridPathFinder::CalculateHeuristic(
	const GridPosition& a,
	const GridPosition& b)
{
	return
		std::abs(
			a.X -
			b.X) +
		std::abs(
			a.Y -
			b.Y);
}
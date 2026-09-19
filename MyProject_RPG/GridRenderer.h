#pragma once

/**
 * @file GridRenderer.h
 * @brief GridMap‚Ì‰Â‹‰»
 */

#include "CommonType.h"

class GridMap;

/**
 * @brief GridMap•`‰æ
 */
class GridRenderer
{
public:

	/**
	 * @brief Grid‹«ŠEü‚ğ•`‰æ‚·‚é
	 */
	static void Draw(
		const GridMap& gridMap,
		const Color& color,
		float heightOffset = 0.02f);
};
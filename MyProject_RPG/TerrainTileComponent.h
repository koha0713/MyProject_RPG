#pragma once

#include "Component.h"
#include "GridPosition.h"
#include "TerrainMapData.h"

/**
 * @brief 1マス分のTerrain情報を保持するComponent
 *
 * @details
 * Terrainの種類やGridPositionを保持する。
 *
 * 将来的には、
 * Cameraとの距離による描画制御や
 * Terrain固有情報の管理にも使用する。
 */
class TerrainTileComponent :
	public Component
{
public:

	TerrainTileComponent() = default;
	~TerrainTileComponent() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// Terrain
	//====================

	void SetTerrainType(
		TerrainType type)
	{
		m_TerrainType =
			type;
	}

	TerrainType GetTerrainType() const
	{
		return m_TerrainType;
	}

	//====================
	// Grid Position
	//====================

	void SetGridPosition(
		const GridPosition& position)
	{
		m_GridPosition =
			position;
	}

	const GridPosition&
		GetGridPosition() const
	{
		return m_GridPosition;
	}

	const char*
		GetComponentName() const override
	{
		return "TerrainTileComponent";
	}

private:

	TerrainType
		m_TerrainType =
		TerrainType::Grass;

	GridPosition
		m_GridPosition
	{
		0,
		0
	};
};
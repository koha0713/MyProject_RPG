#include "GridPositionComponent.h"

#include "DebugUI.h"
#include "GameObject.h"
#include "GridMap.h"
#include "TransformComponent.h"

//=====================================================
// Lifecycle
//=====================================================

void GridPositionComponent::Initialize()
{
}

void GridPositionComponent::Finalize()
{
	//=================================================
	// Grid Occupant‰ðœ
	//=================================================

	if (m_GridMap &&
		m_IsRegistered)
	{
		m_GridMap->ClearOccupant(
			m_GridPosition,
			GetOwner());
	}

	m_GridMap =
		nullptr;

	m_IsRegistered =
		false;
}

void GridPositionComponent::Update(
	uint64_t delta)
{
	(void)delta;
}

void GridPositionComponent::Draw()
{
}

//=====================================================
// Grid
//=====================================================

void GridPositionComponent::SetGridMap(
	GridMap* gridMap)
{
	//=================================================
	// ŒÃ‚¢Grid‚©‚ç“o˜^‰ðœ
	//=================================================

	if (m_GridMap &&
		m_IsRegistered)
	{
		m_GridMap->ClearOccupant(
			m_GridPosition,
			GetOwner());
	}

	m_GridMap =
		gridMap;

	m_IsRegistered =
		false;

}

bool GridPositionComponent::SetGridPosition(
	const GridPosition& position)
{
	//=================================================
	// Debug
	//=================================================

	char buffer[128]{};

	sprintf_s(
		buffer,
		"[GridPosition] Request: (%d, %d)\n",
		position.X,
		position.Y);

	OutputDebugStringA(
		buffer);

	//=================================================
	// GridMap
	//=================================================

	if (!m_GridMap)
	{
		OutputDebugStringA(
			"[GridPosition] FAILED: GridMap is null\n");

		return false;
	}

	//=================================================
	// Owner
	//=================================================

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		OutputDebugStringA(
			"[GridPosition] FAILED: Owner is null\n");

		return false;
	}

	//=================================================
	// “¯‚¶ˆÊ’u
	//=================================================

	if (m_IsRegistered &&
		position == m_GridPosition)
	{
		SyncTransform();

		return true;
	}

	//=================================================
	// CellŠm”F
	//=================================================

	if (!m_GridMap->CanMoveTo(
		position))
	{
		OutputDebugStringA(
			"[GridPosition] FAILED: CanMoveTo false\n");

		return false;
	}

	//=================================================
	// Occupant“o˜^
	//=================================================

	if (!m_GridMap->SetOccupant(
		position,
		owner))
	{
		OutputDebugStringA(
			"[GridPosition] FAILED: SetOccupant false\n");

		return false;
	}

	//=================================================
	// ŒÃ‚¢Cell‰ðœ
	//=================================================

	if (m_IsRegistered)
	{
		m_GridMap->ClearOccupant(
			m_GridPosition,
			owner);
	}

	//=================================================
	// À•WŠm’è
	//=================================================

	m_GridPosition =
		position;

	m_IsRegistered =
		true;

	sprintf_s(
		buffer,
		"[GridPosition] SUCCESS: (%d, %d)\n",
		m_GridPosition.X,
		m_GridPosition.Y);

	OutputDebugStringA(
		buffer);

	//=================================================
	// Transform“¯Šú
	//=================================================

	SyncTransform();

	return true;
}
bool GridPositionComponent::TryMove(
	const GridPosition& direction)
{
	const GridPosition nextPosition =
		m_GridPosition +
		direction;

	return SetGridPosition(
		nextPosition);
}

//=====================================================
// Transform Sync
//=====================================================

void GridPositionComponent::SyncTransform()
{
	if (!m_GridMap)
	{
		return;
	}

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	TransformComponent* transform =
		owner->GetComponent<
		TransformComponent>();

	if (!transform)
	{
		return;
	}

	const Vector3 worldPosition =
		m_GridMap->GridToWorld(
			m_GridPosition);

	transform->SetPosition(
		worldPosition);
}

//=====================================================
// Debug UI
//=====================================================

void GridPositionComponent::DrawDebugUI()
{
	ImGui::Text(
		"Grid Position: (%d, %d)",
		m_GridPosition.X,
		m_GridPosition.Y);

	ImGui::Text(
		"Registered: %s",
		m_IsRegistered
		? "True"
		: "False");

	if (m_GridMap)
	{
		ImGui::Text(
			"Grid Size: %d x %d",
			m_GridMap->GetWidth(),
			m_GridMap->GetHeight());

		ImGui::Text(
			"Cell Size: %.2f",
			m_GridMap->GetCellSize());
	}
	else
	{
		ImGui::Text(
			"GridMap: None");
	}
}
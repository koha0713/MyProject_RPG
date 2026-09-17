#include "FieldExitComponent.h"

#include "DebugUI.h"

void FieldExitComponent::Initialize()
{
}

void FieldExitComponent::Finalize()
{
}

void FieldExitComponent::Update(
	uint64_t delta)
{
	(void)delta;
}

void FieldExitComponent::Draw()
{
}

void FieldExitComponent::DrawDebugUI()
{
	ImGui::Text(
		"Target Scene: %s",
		m_TargetScene.c_str());
}
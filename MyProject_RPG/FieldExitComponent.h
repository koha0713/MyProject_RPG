#pragma once

/**
 * @file FieldExitComponent.h
 * @brief Fieldから別Sceneへ遷移する地点を表すComponent
 */

#include <string>

#include "Component.h"
#include "GridPosition.h"

 /**
  * @brief Field Exit Component
  *
  * @details
  * 特定のGrid地点に配置し、
  * Playerがその地点にいる場合にScene遷移を許可する。
  */
class FieldExitComponent :
	public Component
{
public:

	FieldExitComponent() = default;
	~FieldExitComponent() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	/**
	 * @brief 遷移先Sceneを設定
	 */
	void SetTargetScene(
		const std::string& sceneName)
	{
		m_TargetScene =
			sceneName;
	}

	const std::string&
		GetTargetScene() const
	{
		return m_TargetScene;
	}

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

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "FieldExitComponent";
	}

private:
	GridPosition m_GridPosition;
	std::string m_TargetScene;
};
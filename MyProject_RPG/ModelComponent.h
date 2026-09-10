#pragma once
#include <string>
#include "Component.h"
#include "ModelData.h"

/**
 * @brief 3Dモデルの描画Component
 */
class ModelComponent : public Component
{
public:
	//====================
	// ライフサイクル
	//====================
	ModelComponent() = default;
	~ModelComponent() = default;

	void Initialize() override;
	void Update() override;
	void Draw() override;

	//====================
	// ModelData関係
	//====================
	/**
	 * @brief モデルをファイルパスから指定
	 */
	bool SetModel(const std::string& filePath);

	/**
	 * @brief モデルを直接指定
	 */
	void SetModel(std::shared_ptr<ModelData> model)
	{
		m_Model = std::move(model);
	}

	/**
	 * @brief モデルを取得
	 */
	std::shared_ptr<ModelData> GetModel() const
	{
		return m_Model;
	}

	/**
	 * @brief モデルが設定されているか確認
	 */
	bool HasModel() const
	{
		return m_Model != nullptr;
	}

	bool SetTexture(
		size_t materialIndex,
		const std::string& filePath);

private:
	std::shared_ptr<ModelData> m_Model = nullptr; // モデルデータ

};
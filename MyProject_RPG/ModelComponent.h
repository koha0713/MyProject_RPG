#pragma once
#include <string>
#include "Component.h"
#include "ModelData.h"
#include "AnimationData.h"

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
	void Update(uint64_t delta) override;
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

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "ModelComponent";
	}

	/**
	 * @brief Animation登録
	 */
	bool SetAnimation(
		AnimationID id,
		const std::string& filePath);

	/**
	 * @brief Animation再生
	 */
	bool PlayAnimation(
		AnimationID id,
		bool loop = true);

private:
	std::shared_ptr<ModelData> m_Model = nullptr; // モデルデータ

};
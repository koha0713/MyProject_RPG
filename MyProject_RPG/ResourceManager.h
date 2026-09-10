#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "NonCopyable.h"
#include "ModelData.h"
#include "Texture.h"

/**
 * @class ResourceManager
 * @brief リソース管理クラス
 * @details 同じパスのリソースが既に読み込まれている場合、
 * 			再度読み込まずに既存のリソースを返す
 */

class ResourceManager : private NonCopyable
{
public:
	//====================
	// ライフサイクル
	//====================
	ResourceManager() = default;
	~ResourceManager() = default;

	/**
	 * @brief 初期化処理
	 */
	void Initialize();

	/**
	 * @brief 終了処理
	 */
	void Finalize();

	//====================
	// ModelData関係
	//====================
	/**
	 * @brief ModelData取得
	 * @param filePath モデルファイル名
	 * @return 取得したModelData
	 */
	std::shared_ptr<ModelData> LoadModel(
		const std::string& filePath);

	/**
	 * @brief ModelDataを解放
	 */
	void UnloadModel(
		const std::string& filePath);

	 /**
	  * @brief ModelDataを全て解放
	  */
	void ClearModels();

	//====================
	// Texture関係
	//====================

	/**
	 * @brief Texture取得
	 *
	 * @details
	 * 同じパスのTextureが既にロード済みなら
	 * 既存Textureを返す。
	 *
	 * @param filePath Textureファイルパス
	 *
	 * @return Texture
	 */
	std::shared_ptr<Texture> LoadTexture(
		const std::string& filePath);

	/**
	 * @brief TextureをResourceManagerから削除
	 */
	void UnloadTexture(
		const std::string& filePath);

	/**
	 * @brief Textureを全て削除
	 */
	void ClearTextures();

private:
	//====================
	// メンバ変数
	//====================
	/**
	 * @brief モデルデータのマップ
	 */
	std::unordered_map<
		std::string,
		std::shared_ptr<ModelData>
	> m_ModelDataMap;	// モデルデータのマップ

	/**
	 * @brief Textureキャッシュ
	 */
	std::unordered_map<
		std::string,
		std::shared_ptr<Texture>
	> m_TextureMap;
	
public:
	/**
	 * @brief ResourceManagerのインスタンスを取得
	 * @return ResourceManagerのインスタンス
	 */
	static ResourceManager& GetInstance()
	{
		static ResourceManager instance;
		return instance;
	}

};

#define RESOURCE_MANAGER ResourceManager::GetInstance()
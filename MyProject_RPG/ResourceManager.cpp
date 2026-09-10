#include "ResourceManager.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include <filesystem>

void ResourceManager::Initialize()
{
	// 初期化処理は特に必要なし
}

void ResourceManager::Finalize()
{
	// ModelDataがTextureへのshared_ptrを
	// 保持しているためModelから先に解放する
	ClearModels();

	// Textureキャッシュ解放
	ClearTextures();
}

std::shared_ptr<ModelData> ResourceManager::LoadModel(
	const std::string& filePath)
{
	// 既に読み込まれている場合は、既存のリソースを返す
	auto it = m_ModelDataMap.find(filePath);
	if (it != m_ModelDataMap.end())
	{
		return it->second;
	}

	// モデルデータを読み込む(新規の場合)
	auto modelData =
		ModelLoader::LoadModel(
			filePath,
			1.0f,
			true,
			false);

	if (!modelData)
	{
		// Assimp読み込み失敗
		return nullptr;
	}

	//====================
	// GPU Mesh生成
	//====================

	if (!modelData->CreateGpuResources(
		Renderer::GetDevice()))
	{
		return nullptr;
	}

	//====================
	// Material Texture生成
	//====================
	for (auto& material :
		modelData->GetMaterials())
	{
		// Texture指定がないMaterialは
		// DiffuseColorだけで描画する
		if (material.TexturePath.empty())
		{
			continue;
		}

		auto texture =
			LoadTexture(
				material.TexturePath);

		if (!texture)
		{
			// Textureだけ読み込めなくても
			// モデル全体のロード失敗にはしない。
			//
			// Renderer側でDiffuseColorへ
			// フォールバックする。
			continue;
		}

		material.Texture =
			std::move(texture);
	}

	// 読み込んだモデルデータをマップに登録
	m_ModelDataMap.emplace(filePath, modelData);

	return modelData;
}

void ResourceManager::UnloadModel(
	const std::string& filePath)
{
	// モデルデータをマップから削除
	m_ModelDataMap.erase(filePath);
}

void ResourceManager::ClearModels()
{
	// モデルデータを全て解放
	m_ModelDataMap.clear();
}

std::shared_ptr<Texture>
ResourceManager::LoadTexture(
	const std::string& filePath)
{
	if (filePath.empty())
	{
		return nullptr;
	}

	//====================
	// パス正規化
	//====================

	const std::string normalizedPath =
		std::filesystem::path(filePath)
		.lexically_normal()
		.string();

	//====================
	// キャッシュ確認
	//====================

	auto it =
		m_TextureMap.find(
			normalizedPath);

	if (it !=
		m_TextureMap.end())
	{
		return it->second;
	}

	//====================
	// 新規Texture生成
	//====================

	auto texture =
		std::make_shared<Texture>();

	if (!texture->Load(
		Renderer::GetDevice(),
		normalizedPath))
	{
		return nullptr;
	}

	//====================
	// キャッシュ登録
	//====================

	m_TextureMap.emplace(
		normalizedPath,
		texture);

	return texture;
}

void ResourceManager::UnloadTexture(
	const std::string& filePath)
{
	const std::string normalizedPath =
		std::filesystem::path(filePath)
		.lexically_normal()
		.string();

	m_TextureMap.erase(
		normalizedPath);
}

void ResourceManager::ClearTextures()
{
	m_TextureMap.clear();
}
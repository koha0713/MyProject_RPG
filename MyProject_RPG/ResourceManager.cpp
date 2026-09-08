#include "ResourceManager.h"
#include "ModelLoader.h"
#include "Renderer.h"

void ResourceManager::Initialize()
{
	// 初期化処理は特に必要なし
}

void ResourceManager::Finalize()
{
	// モデルデータを全て解放
	ClearModels();
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
			false,
			false);

	if (!modelData)
	{
		// Assimpによる読み込み失敗
		return nullptr;
	}

	//====================
	// GPUリソース生成
	//====================

	if (!modelData->CreateGpuResources(
		Renderer::GetDevice()))
	{
		// VertexBuffer / IndexBuffer生成失敗
		return nullptr;
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
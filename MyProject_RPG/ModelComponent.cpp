#include "ModelComponent.h"

#include "ResourceManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "DebugUI.h"

void ModelComponent::Initialize()
{
	// モデルデータの初期化処理
	// ここでは特に何もしないが、必要に応じて処理を追加する
}

void ModelComponent::Update()
{
	// モデルデータの更新処理
	// ここでは特に何もしないが、必要に応じて処理を追加する
}

void ModelComponent::Draw()
{
	if(!m_Model)
	{
		// モデルが設定されていない場合は描画しない
		return;
	}

	auto* transform = GetOwner()->GetComponent<TransformComponent>();
	
	if(!transform)
	{
		// TransformComponentが設定されていない場合は描画しない
		return;
	}

	const Matrix4x4& worldMatrix = 
		transform->GetWorldMatrix();

	//=====================
	// モデルの描画処理
	//=====================
	const auto& meshes = m_Model->GetMeshes();

	const auto& materials = m_Model->GetMaterials();

	for(const auto& meshData : meshes)
	{
		if(!meshData.Mesh)
		{
			// メッシュが設定されていない場合は描画しない
			continue;
		}

		if(meshData.MaterialIndex >= materials.size())
		{
			// マテリアルのインデックスが範囲外の場合は描画しない
			continue;
		}

		const auto& materialData = 
			materials[meshData.MaterialIndex];

		Renderer::DrawMesh(
			*meshData.Mesh,
			materialData,
			worldMatrix);
	}
}

bool ModelComponent::SetModel(const std::string& filePath)
{
	// ResourceManagerからモデルデータを取得
	auto modelData = 
		RESOURCE_MANAGER.LoadModel(filePath);
	if (!modelData)
	{
		// モデルデータの読み込みに失敗した場合はfalseを返す
		m_Model.reset();
		return false;
	}

	// モデルデータを設定
	m_Model = std::move(modelData);

	return true;
}

bool ModelComponent::SetTexture(
	size_t materialIndex,
	const std::string& filePath)
{
	// Modelが設定されていない場合は失敗
	if (!m_Model)
	{
		return false;
	}

	auto& materials =
		m_Model->GetMaterials();

	// Material番号が範囲外の場合は失敗
	if (materialIndex >= materials.size())
	{
		return false;
	}

	// ResourceManagerからTextureを取得
	auto texture =
		RESOURCE_MANAGER.LoadTexture(
			filePath);

	if (!texture)
	{
		return false;
	}

	// 指定MaterialへTextureを設定
	materials[materialIndex].Texture =
		std::move(texture);

	return true;
}

void ModelComponent::DrawDebugUI()
{
	ImGui::Text("ModelComponent");
}
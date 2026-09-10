#include "ModelComponent.h"

#include "ResourceManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "DebugUI.h"
#include "AnimatorComponent.h"

void ModelComponent::Initialize()
{
	// モデルデータの初期化処理
	// ここでは特に何もしないが、必要に応じて処理を追加する
}

void ModelComponent::Update(uint64_t delta)
{
	// モデルデータの更新処理
	// ここでは特に何もしないが、必要に応じて処理を追加する
	(void)delta;
}

void ModelComponent::Draw()
{
	if (!m_Model)
	{
		return;
	}

	auto* transform =
		GetOwner()->GetComponent<TransformComponent>();

	if (!transform)
	{
		return;
	}

	const Matrix4x4& worldMatrix =
		transform->GetWorldMatrix();

	//====================
	// Animator
	//====================

	const AnimatorComponent* animator =
		GetOwner()->GetComponent<AnimatorComponent>();

	std::span<const Matrix4x4>
		boneMatrices;

	const std::vector<Matrix4x4>*
		globalNodeMatrices =
		nullptr;

	if (animator)
	{
		const auto& finalMatrices =
			animator->GetFinalBoneMatrices();

		if (!finalMatrices.empty())
		{
			boneMatrices =
				std::span<const Matrix4x4>(
					finalMatrices.data(),
					finalMatrices.size());
		}

		globalNodeMatrices =
			&animator->GetGlobalBoneMatrices();
	}

	//====================
	// Model
	//====================

	const auto& meshes =
		m_Model->GetMeshes();

	const auto& materials =
		m_Model->GetMaterials();

	const auto& skeleton =
		m_Model->GetSkeleton();

	for (const auto& meshData : meshes)
	{
		if (!meshData.Mesh)
		{
			continue;
		}

		if (meshData.MaterialIndex >=
			materials.size())
		{
			continue;
		}

		const auto& materialData =
			materials[
				meshData.MaterialIndex];

		//=================================================
		// Skinned Mesh
		//=================================================

		if (meshData.HasSkinning)
		{
			Renderer::DrawMesh(
				*meshData.Mesh,
				materialData,
				worldMatrix,
				boneMatrices);

			continue;
		}

		//=================================================
		// Node Transformで動くMesh
		//=================================================

		Matrix4x4 meshWorld =
			worldMatrix;

		if (globalNodeMatrices &&
			meshData.NodeIndex >= 0)
		{
			const size_t nodeIndex =
				static_cast<size_t>(
					meshData.NodeIndex);

			if (nodeIndex <
				globalNodeMatrices->size() &&
				nodeIndex <
				skeleton.Bones.size())
			{
				const Matrix4x4&
					currentNodeGlobal =
					(*globalNodeMatrices)[
						nodeIndex];

				// Node空間をModel Worldへ接続
				meshWorld =
					currentNodeGlobal *
					worldMatrix;
			}
		}

		Renderer::DrawMesh(
			*meshData.Mesh,
			materialData,
			meshWorld,
			{});
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

	//=================================================
	// FBX内Animationの登録
	//=================================================

	const auto& animations =
		m_Model->GetAnimations();

	if (!animations.empty())
	{
		auto* owner =
			GetOwner();

		if (!owner)
		{
			return true;
		}

		AnimatorComponent* animator =
			owner->
			GetComponent<AnimatorComponent>();

		// Animationを含むModelの場合のみ
		// AnimatorComponentを自動追加する
		if (!animator)
		{
			animator =
				owner->
				AddComponent<AnimatorComponent>();
		}

		if (animator)
		{
			animator->
				SetAnimations(
					animations);
		}
	}

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

bool ModelComponent::SetAnimation(
	AnimationID id,
	const std::string& filePath)
{
	if (!GetOwner())
	{
		return false;
	}

	//====================
	// Animator取得
	//====================

	auto* animator =
		GetOwner()->
		GetComponent<AnimatorComponent>();

	// Animationを初めて設定するときは
	// AnimatorComponentを自動追加する
	if (!animator)
	{
		animator =
			GetOwner()->
			AddComponent<AnimatorComponent>();
	}

	if (!animator)
	{
		return false;
	}

	return animator->
		SetAnimation(
			id,
			filePath);
}

bool ModelComponent::PlayAnimation(
	AnimationID id,
	bool loop)
{
	if (!GetOwner())
	{
		return false;
	}

	auto* animator =
		GetOwner()->
		GetComponent<AnimatorComponent>();

	if (!animator)
	{
		return false;
	}

	return animator->
		PlayAnimation(
			id,
			loop);
}

void ModelComponent::DrawDebugUI()
{
	ImGui::Text("ModelComponent");
}
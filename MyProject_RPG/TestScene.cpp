#include "TestScene.h"
#include "DebugUI.h"
#include "Renderer.h"

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include <ModelLoader.h>

TestScene::TestScene()
{

}

void TestScene::Initialize()
{
	// GameObjectの初期化
	{
		//====================
		// Player生成
		//====================
		{
			auto* player =
				m_gameObjectManager.Create<GameObject>();

			if (!player)
			{
				return;
			}

			player->SetName("Player");
			player->SetTag(Tag::Player);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					player->AddComponent<TransformComponent>();

				if (transform)
				{
					// 仮カメラが原点を向いているため、
					// まずはモデルを原点に配置して確認する
					transform->SetPosition(
						0.0f,
						0.0f,
						0.0f);

					transform->SetRotation(
						0.0f,
						0.0f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						1.0f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					player->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Warrior/Warrior.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Warrior/Warrior_Texture.png");
					// Material[1]へTextureを手動設定
					const bool texture1Result =
						model->SetTexture(
							1,
							"Assets/Models/Warrior/Warrior_Sword_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle);
				}
			}

		}
		//====================
		// Camera生成
		//====================
		{
			auto* cameraObject =
				m_gameObjectManager.Create<GameObject>();

			if (cameraObject)
			{
				cameraObject->SetName(
					"MainCamera");

				//====================
				// Transform
				//====================

				auto* cameraTransform =
					cameraObject->
					AddComponent<TransformComponent>();

				if (cameraTransform)
				{
					cameraTransform->SetPosition(
						0.0f,
						2.0f,
						-5.0f);

					// 原点方向を向く初期値
					cameraTransform->SetRotation(
						0.0f,
						0.0f,
						0.0f);
				}

				//====================
				// Camera
				//====================

				m_MainCamera =
					cameraObject->
					AddComponent<CameraComponent>();

				if (m_MainCamera)
				{
					m_MainCamera->
						SetFieldOfView(
							60.0f);

					m_MainCamera->
						SetNearClip(
							0.1f);

					m_MainCamera->
						SetFarClip(
							1000.0f);
				}
			}
		}

		
	}

	m_gameObjectManager.Initialize();

}

void TestScene::Finalize()
{
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void TestScene::Update(uint64_t delta)
{
	// GameObjectの更新
	m_gameObjectManager.Update(delta);
}

void TestScene::Draw(uint64_t delta)
{
	//====================
	// Camera設定
	//====================

	if (m_MainCamera)
	{
		Renderer::SetCamera(
			m_MainCamera->
			GetViewMatrix(),

			m_MainCamera->
			GetProjectionMatrix());
	}

	// GameObjectの描画
	m_gameObjectManager.Draw();

	// デバッグUIの描画
	DebugUI::RegisterDebugFunction([this]()
		{
			m_gameObjectManager.DrawDebugUI();
		});

}
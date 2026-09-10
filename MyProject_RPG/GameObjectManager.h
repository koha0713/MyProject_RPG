#pragma once
#include <vector>
#include "GameObject.h"

/**
 * @class GameObjectManagerクラス
 * @brief GameObjectの管理クラス
 */
class GameObjectManager : private NonCopyable
{
public:

	GameObjectManager() = default;
	~GameObjectManager() = default;

	//====================
	// ライフサイクル
	//====================
	/**
	 * @brief 初期化処理
	 */
	void Initialize();

	/**
	 * @brief 終了処理
	 */
	void Finalize();

	/**
	 * @brief 更新処理
	 */
	void Update(uint64_t delta);

	/**
	 * @brief 描画処理
	 */
	void Draw();

	/**
	 * @brief 全GameObjectの破棄
	 */
	void Clear();

	//====================
	// GameObjectの生成・破棄
	//====================
	template<class T = GameObject, class... Args>
	T* Create(Args&&... args)
	{
		static_assert(std::is_base_of_v<GameObject, T>);

		auto object = std::make_unique<T>(std::forward<Args>(args)...);

		T* ptr = object.get();

		object->Initialize();

		m_GameObjects.emplace_back(std::move(object));

		return ptr;
	}


	//====================
	// GameObjectの検索
	//====================
	/**
	 * @brief タグでGameObjectを検索
	 */
	std::vector<GameObject*> FindByTag(Tag tag) const;

	/**
	 * @brief 名前でGameObjectを検索
	 */
	GameObject* FindByName(const std::string& name) const;

	/**
	 * @brief 全GameObjectの取得
	 */
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const
	{
		return m_GameObjects;
	}

	/**
	 * @brief GameObject / Component用DebugUI
	 */
	void DrawDebugUI();

private:
	/**
	 * @brief 破棄されたGameObjectを削除
	 */
	void RemoveDestroyedObjects();

private:
	//====================
	// GameObjectの管理
	//====================
	std::vector<std::unique_ptr<GameObject>> m_GameObjects;

	size_t m_SelectedGameObjectIndex = 0;
	size_t m_SelectedComponentIndex = 0;
};
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <type_traits>
#include "NonCopyable.h"
#include "Component.h"

/**
 * @brief GameObjectのTag
 */
enum class Tag
{
	None,
	Player,
	Enemy,
	NPC,
	Stage
};

/**
 * @brief GameObjectクラス
 */
class GameObject : private NonCopyable
{
public:

	GameObject() = default;

	virtual ~GameObject() = default;

	//====================
	// ライフサイクル
	//====================
	/**
	 * @brief 初期化処理
	 */
	virtual void Initialize()
	{
		for (auto& component : m_Components)
		{
			component->Initialize();
		}
	}

	/**
	 * @brief 終了処理
	 */
	virtual void Finalize()
	{
		for (auto& component : m_Components)
		{
			component->Finalize();
		}
	}

	/**
	 * @brief 更新処理
	 */
	virtual void Update()
	{
		// 非アクティブなら更新しない
		if (!m_IsActive)
			return;

		for (auto& component : m_Components)
		{
			component->Update();
		}
	}

	/**
	 * @brief 描画処理
	 */
	virtual void Draw()
	{
		// 非アクティブなら描画しない
		if (!m_IsActive)
			return;

		for (auto& component : m_Components)
		{
			component->Draw();
		}
	}

	//====================
	// コンポーネント関係
	//====================
	/**
	 * @brief Component登録
	 */
	template<class T, class... Args>
	T* AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of_v<Component, T>);
		auto component =
			std::make_unique<T>(std::forward<Args>(args)...);
		component->SetOwner(this);
		T* ptr = component.get();
		m_Components.emplace_back(std::move(component));

		return ptr;
	}

	/**
	 * @brief Component取得
	 */
	template<class T>
	T* GetComponent() const
	{
		for (auto& component : m_Components)
		{
			if (auto ptr = dynamic_cast<T*>(component.get()))
			{
				return ptr;
			}
		}
		return nullptr;
	}

	//====================
	// 名前関係
	//====================
	/**
	 * @brief 名前設定
	 * @param name セットする名前
	 */
	void SetName(const std::string& name)
	{
		m_Name = name;
	}

	/**
	 * @brief 名前取得
	 * @return const std::string& Objectの名前
	 */
	const std::string& GetName() const
	{
		return m_Name;
	}

	//====================
	// タグ関係
	//====================
	/**
	 * @brief タグ設定
	 * @param tag セットするtag名
	 */
	void SetTag(const Tag& tag)
	{
		m_Tag = tag;
	}

	/**
	 * @brief タグ取得
	 * @return Tag& ObjectのTag名
	 */
	const Tag& GetTag() const
	{
		return m_Tag;
	}

	//====================
	// アクティブ関係
	//====================
	/**
	 * @brief アクティブセット
	 * @param active セットするbool値
	 */
	void SetActive(bool active)
	{
		m_IsActive = active;
	}

	/**
	 * @brief アクティブ取得
	 * @return　bool アクティブ状態か
	 */
	bool IsActive() const
	{
		return m_IsActive;
	}


	//====================
	// デストロイ関係
	//====================
	/**
	 * @brief GameObjectを削除する
	 */
	void Destroy()
	{
		m_IsDestroy = true;
	}

	/**
	 * @brief 削除するか取得
	 * @return bool GameObjectを削除するか
	 */
	bool IsDestroy() const
	{
		return m_IsDestroy;
	}

	/**
	 * @brief 所有しているComponent一覧を取得
	 *
	 * @details
	 * 所有権はGameObjectが保持するため、
	 * 外部からvector自体は変更させない。
	 */
	const std::vector<std::unique_ptr<Component>>&
		GetComponents() const
	{
		return m_Components;
	}

private:
	std::string m_Name;
	Tag m_Tag;
	bool m_IsActive = true;		// アクティブ状態か
	bool m_IsDestroy = false;	// 削除するか

	std::vector<std::unique_ptr<Component>> m_Components;

};
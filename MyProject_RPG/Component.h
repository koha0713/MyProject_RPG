#pragma once
#include "NonCopyable.h"
#include <cstdint>

class GameObject;

/**
 * @brief 全てのComponentの基底クラス
 */
class Component : private NonCopyable
{
public:
	Component() = default;
	virtual ~Component() = default;

	//====================
	// ライフサイクル
	//====================
	virtual void Initialize() {}

	virtual void Finalize() {}

	virtual void Update(uint64_t delta) {}

	virtual void Draw() {}

	//====================
	// Owner
	//====================
	GameObject* GetOwner() const
	{
		return m_Owner;
	}

	//====================
	// Debug UI
	//====================
	/**
	 * @brief Component固有のデバッグUIを描画する
	 *
	 * @details
	 * 必要なComponentだけoverrideする。
	 * GameObjectManager側はComponentの具体型を知らずに
	 * この関数を呼び出せる。
	 */
	virtual void DrawDebugUI()
	{
	}

	//====================
	// Getter関数
	//====================
	virtual const char* GetComponentName() const
	{
		return "Component";
	}

private:
	friend class GameObject;

	void SetOwner(GameObject* owner)
	{
		m_Owner = owner;
	}


private:
	GameObject* m_Owner = nullptr;

};
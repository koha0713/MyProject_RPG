#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "IScene.h"
#include "NonCopyable.h"

// 前方宣言
class IScene;

/**
 * @brief SceneManagerクラス
 */
class SceneManager : NonCopyable {
	
	static inline std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes{};
	static inline std::string m_currentSceneName{};

public:
	//====================
	// ライフサイクル
	//====================
	static void Initialize();
	static void Finalize();
	static void Update(uint64_t delta);
	static void Draw(uint64_t delta);

	//====================
	// シーンマネージャー関連
	//====================
	static void SetCurrentScene(std::string);
	static IScene* GetCurrentScene()
	{
		return m_scenes[m_currentSceneName].get();
	}


};


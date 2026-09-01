#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <cassert>
#include "IScene.h"

/**
 * @brief シーンクラスを名前で登録・生成するファクトリクラス
 * 
 * @details このクラスはシングルトンとして動作、文字列名に対応する
 * IScene派生クラスのインスタンスを生成するための関数を登録・管理する。
 * Create()関数で文字列からシーンインスタンスを動的に生成する。
 */
class SceneClassFactory {
public:
	/**
	 * @brief IScene派生クラスのインスタンスを生成する関数型
	 */
	using SceneCreatorFunc = std::function<std::unique_ptr<IScene>()>;

	/**
	 * @brief シングルトンインスタンスを取得
	 * @return SceneClassFactoryの唯一インスタンス
	 */
	static SceneClassFactory& GetInstance()
	{
		static SceneClassFactory instance;
		return instance;
	}

	/**
	 * @brief クラス名と生成関数を登録する
	 * @param name クラス名(create()で指定するキー)
	 * @param func クラスインスタンスを生成する関数(例：std::make_unique)
	 */
	void RegisterClass(const std::string& name, SceneCreatorFunc func)
	{
		m_registry[name] = func;
	}

	/**
	 * @brief 登録されたクラス名からシーンインスタンスを生成する
	 * @param name 生成したいクラス名(RegisterClassで登録されたキー)
	 * @return std::unique_ptr<IScene> 該当クラスのユニークポインタ(見つからなければnullptr)
	 */
	std::unique_ptr<IScene> Create(const std::string& name) 
	{
		auto it = m_registry.find(name);
		if (it != m_registry.end()) {
			return it->second();
		}
		assert(false && "SceneClassFactory : クラス名が登録されていません");
		return nullptr;
	}

private:
	/**
	 * @brief クラス名と生成関数のマッピングテーブル
	 */
	std::unordered_map<std::string, SceneCreatorFunc> m_registry;

};


/**
 * @brief クラスをSceneClassFactoryに自動登録するマクロ
 * @details IScene派生クラスのソースファイルにこのマクロを記述することで
 * SceneClassFactoryに登録できるので、忘れず書くこと！
 * 
 * 以下、使用例：
 * @code
 * class TitleScene : public IScene { ... };
 * REGISTER_CLASS(TitleScene);
 * @endcode
 * 
 * @param CLASSNAME 登録対象のクラス名
 */
#define REGISTER_CLASS(CLASSNAME) \
    namespace { \
        struct CLASSNAME##Registrar { \
            CLASSNAME##Registrar() { \
                SceneClassFactory::GetInstance().RegisterClass(#CLASSNAME, []() { \
                    return std::make_unique<CLASSNAME>(); \
                }); \
            } \
        }; \
        static CLASSNAME##Registrar global_##CLASSNAME##_registrar; \
    }

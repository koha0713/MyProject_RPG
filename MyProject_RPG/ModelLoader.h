#pragma once
#include <string>
#include <memory>
#include "ModelData.h"
/**
 * @brief モデルデータを読み込むクラス
 * @details Assimpを使用してモデルデータを読み込む
 */
class ModelLoader
{
public:
	/**
	 * @brief モデルデータを読み込む
	 * @param filePath モデルファイル名
	 * @param scaleBase モデルのスケール基準値
	 * @param flip モデルのY軸反転フラグ
	 * @param simple モデルの簡易読み込みフラグ
	 * @return 読み込んだモデルデータ
	 */
	static std::shared_ptr<ModelData> LoadModel(
		const std::string& filePath,
		float scaleBase,
		bool flip,
		bool simple);
};
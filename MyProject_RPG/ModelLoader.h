#pragma once
#include <string>
#include <memory>
#include "ModelData.h"
#include "AnimationData.h"
/**
 * @brief モデルデータを読み込むクラス
 * @details Assimpを使用してモデルデータを読み込む
 */
class ModelLoader
{
public:
	/**
	 * @brief モデルデータ全体を読み込む
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

	/**
	 * @brief AnimationだけをFBXから読み込む
	 * @details
	 *	Animation用FBXからMeshやTextureを生成せず、
	 *	AnimationClipのみ取得する。
	 * @param filePath Animationを含むFBX
	 * @param outAnimations 読み込んだAnimationClip
	 * @return 1つ以上Animationを取得できればtrue
	 */
	static bool LoadAnimations(
		const std::string& filePath,
		std::vector<AnimationClip>& outAnimations);
};
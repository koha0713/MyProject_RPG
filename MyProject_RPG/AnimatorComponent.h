#pragma once

/**
 * @file AnimatorComponent.h
 * @brief Skeletal Animationの再生状態を管理するComponent
 */

#include <memory>
#include <unordered_map>
#include <vector>

#include "Component.h"
#include "AnimationData.h"
#include "CommonType.h"

 /**
  * @brief Animation再生を管理するComponent
  *
  * @details
  * ModelDataは共有Resourceとして保持し、
  * 現在Animation・再生時間・速度などの
  * GameObject固有状態はこちらで管理する。
  */
class AnimatorComponent :
	public Component
{
public:

	AnimatorComponent() = default;
	~AnimatorComponent() override = default;

	//====================
	// Component
	//====================

	void Initialize() override;
	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "AnimatorComponent";
	}

	//====================
	// Animation登録
	//====================

	/**
	 * @brief AnimationをIDへ登録
	 *
	 * @details
	 * filePath内に複数Animationが存在する場合は、
	 * AnimationIDに対応する名前を検索する。
	 */
	bool SetAnimation(
		AnimationID id,
		const std::string& filePath);

	//====================
	// 再生
	//====================

	/**
	 * @brief Animation再生
	 */
	bool PlayAnimation(
		AnimationID id,
		bool loop = true);

	/**
	 * @brief Animation停止
	 */
	void StopAnimation();

	/**
	 * @brief 一時停止
	 */
	void SetPaused(bool paused)
	{
		m_Paused = paused;
	}

	/**
	 * @brief 再生速度
	 */
	void SetSpeed(float speed)
	{
		m_Speed = speed;
	}

	//====================
	// Getter
	//====================

	bool IsPlaying() const
	{
		return m_Playing;
	}

	AnimationID GetCurrentAnimation() const
	{
		return m_CurrentAnimation;
	}

	double GetCurrentTime() const
	{
		return m_CurrentTime;
	}

	const AnimationClip* GetCurrentClip() const;

	/**
	 * @brief Skinning用Bone行列を取得
	 */
	const std::vector<Matrix4x4>&
		GetFinalBoneMatrices() const
	{
		return m_FinalBoneMatrices;
	}

	/**
	 * @brief 既に読み込まれているAnimationClip群を登録する
	 *
	 * @details
	 * ModelDataに内包されているAnimationを、
	 * AnimationIDに対応するClipへ紐付ける。
	 *
	 * ファイルの再読み込みは行わない。
	 */
	bool SetAnimations(
		const std::vector<AnimationClip>& clips);

	/**
	 * @brief 各BoneのGlobal Transformを取得
	 *
	 * @details
	 * Rigid Meshを特定Boneへ追従させる場合に使用する。
	 */
	const std::vector<Matrix4x4>&
		GetGlobalBoneMatrices() const
	{
		return m_GlobalBoneMatrices;
	}

private:

	/**
	 * @brief AnimationIDに対応するClipを探す
	 */
	const AnimationClip* FindClip(
		AnimationID id,
		const std::vector<AnimationClip>& clips) const;

	/**
	 * @brief IDから検索用名称を取得
	 */
	static const char* GetAnimationName(
		AnimationID id);

	/**
	 * @brief Animation時間を進める
	 *
	 * @param deltaSeconds 経過秒数
	 */
	void UpdateAnimationTime(
		double deltaSeconds);

	/**
	 * @brief 現在時刻からSkeleton姿勢を計算
	 */
	void UpdateBonePose();

	/**
	 * @brief Bone名からAnimationChannelを取得
	 */
	const BoneAnimation* FindBoneAnimation(
		const AnimationClip& clip,
		const std::string& boneName) const;

	/**
	 * @brief Position補間
	 */
	Vector3 SamplePosition(
		const BoneAnimation& animation,
		double time,
		const Vector3& defaultValue) const;

	/**
	 * @brief Rotation補間
	 */
	Quaternion SampleRotation(
		const BoneAnimation& animation,
		double time,
		const Quaternion& defaultValue) const;

	/**
	 * @brief Scale補間
	 */
	Vector3 SampleScale(
		const BoneAnimation& animation,
		double time,
		const Vector3& defaultValue) const;



private:

	//====================
	// Animation Resource
	//====================

	std::unordered_map<
		AnimationID,
		AnimationClip>
		m_Animations;

	//====================
	// Bone Pose
	//====================
	/**
	 * @brief 各Node/BoneのGlobal行列
	 */
	std::vector<Matrix4x4>
		m_GlobalBoneMatrices;

	/**
	 * @brief Vertex Skinningに使用する最終Bone行列
	 */
	std::vector<Matrix4x4>
		m_FinalBoneMatrices;

	//====================
	// Playback
	//====================

	AnimationID m_CurrentAnimation =
		AnimationID::None;

	double m_CurrentTime = 0.0;

	float m_Speed = 1.0f;

	bool m_Playing = false;
	bool m_Paused = false;
	bool m_Loop = true;
};
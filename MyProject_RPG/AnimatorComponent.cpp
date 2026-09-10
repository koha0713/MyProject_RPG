#include "AnimatorComponent.h"

#include <algorithm>
#include <cctype>

#include "ModelLoader.h"
#include "DebugUI.h"
#include "GameObject.h"
#include "ModelComponent.h"
#include <cmath>

namespace
{
	/**
	 * @brief Vector3線形補間
	 */
	Vector3 LerpVector3(
		const Vector3& a,
		const Vector3& b,
		float t)
	{
		return Vector3(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t);
	}

	/**
	 * @brief Quaternion球面線形補間
	 */
	Quaternion SlerpQuaternion(
		const Quaternion& a,
		const Quaternion& b,
		float t)
	{
		using namespace DirectX;

		const XMVECTOR qa =
			XMLoadFloat4(&a);

		const XMVECTOR qb =
			XMLoadFloat4(&b);

		const XMVECTOR resultVector =
			XMQuaternionNormalize(
				XMQuaternionSlerp(
					qa,
					qb,
					t));

		Quaternion result;

		XMStoreFloat4(
			&result,
			resultVector);

		return result;
	}

	/**
	 * @brief Transform Matrixを分解
	 */
	void DecomposeTransform(
		const Matrix4x4& matrix,
		Vector3& scale,
		Quaternion& rotation,
		Vector3& translation)
	{
		using namespace DirectX;

		XMVECTOR xmScale;
		XMVECTOR xmRotation;
		XMVECTOR xmTranslation;

		const XMMATRIX xmMatrix =
			XMLoadFloat4x4(
				&matrix);

		if (!XMMatrixDecompose(
			&xmScale,
			&xmRotation,
			&xmTranslation,
			xmMatrix))
		{
			// 分解失敗時の安全な初期値
			scale =
				Vector3(
					1.0f,
					1.0f,
					1.0f);

			rotation =
				Quaternion(
					0.0f,
					0.0f,
					0.0f,
					1.0f);

			translation =
				Vector3(
					0.0f,
					0.0f,
					0.0f);

			return;
		}

		XMStoreFloat3(
			&scale,
			xmScale);

		XMStoreFloat4(
			&rotation,
			xmRotation);

		XMStoreFloat3(
			&translation,
			xmTranslation);
	}
}

//=====================================================
// Component
//=====================================================

void AnimatorComponent::Initialize()
{
}

void AnimatorComponent::Finalize()
{
	m_Animations.clear();

	m_CurrentAnimation =
		AnimationID::None;

	m_CurrentTime = 0.0;

	m_Playing = false;
	m_Paused = false;
}

void AnimatorComponent::Update(
	uint64_t delta)
{
	if (!m_Playing ||
		m_Paused)
	{
		return;
	}

	//=================================================
	// delta → 秒
	//=================================================

	const double deltaSeconds =
		static_cast<double>(delta) /
		1'000'000.0;

	UpdateAnimationTime(
		deltaSeconds);

	UpdateBonePose();
}

void AnimatorComponent::Draw()
{
	// Animator自身は描画しない
}

//=====================================================
// Animation登録
//=====================================================

bool AnimatorComponent::SetAnimation(
	AnimationID id,
	const std::string& filePath)
{
	if (id == AnimationID::None ||
		filePath.empty())
	{
		return false;
	}

	//====================
	// Animation読込
	//====================

	std::vector<AnimationClip> clips;

	if (!ModelLoader::LoadAnimations(
		filePath,
		clips))
	{
		return false;
	}

	//====================
	// IDに対応するClip検索
	//====================

	const AnimationClip* clip =
		FindClip(
			id,
			clips);

	if (!clip)
	{
		return false;
	}

	// ClipをAnimator固有の登録Tableへコピー
	m_Animations[id] =
		*clip;

	return true;
}

//=====================================================
// Play
//=====================================================

bool AnimatorComponent::PlayAnimation(
	AnimationID id,
	bool loop)
{
	const auto it =
		m_Animations.find(
			id);

	if (it ==
		m_Animations.end())
	{
		return false;
	}

	m_CurrentAnimation =
		id;

	m_CurrentTime =
		0.0;

	m_Loop =
		loop;

	m_Playing =
		true;

	m_Paused =
		false;

	return true;
}

void AnimatorComponent::StopAnimation()
{
	m_CurrentAnimation =
		AnimationID::None;

	m_CurrentTime =
		0.0;

	m_Playing =
		false;

	m_Paused =
		false;
}

//=====================================================
// Current Clip
//=====================================================

const AnimationClip*
AnimatorComponent::GetCurrentClip() const
{
	const auto it =
		m_Animations.find(
			m_CurrentAnimation);

	if (it ==
		m_Animations.end())
	{
		return nullptr;
	}

	return &it->second;
}

//=====================================================
// Animation検索
//=====================================================

const AnimationClip*
AnimatorComponent::FindClip(
	AnimationID id,
	const std::vector<AnimationClip>& clips) const
{
	const std::string targetName =
		GetAnimationName(id);

	if (targetName.empty())
	{
		return nullptr;
	}

	//====================
	// 大文字小文字を無視して検索
	//====================

	auto toLower =
		[](std::string text)
		{
			std::transform(
				text.begin(),
				text.end(),
				text.begin(),
				[](unsigned char c)
				{
					return static_cast<char>(
						std::tolower(c));
				});

			return text;
		};

	const std::string targetLower =
		toLower(
			targetName);

	for (const auto& clip : clips)
	{
		const std::string clipLower =
			toLower(
				clip.Name);

		// 例:
		// "CharacterArmature|Walk"
		//             ↓
		// "walk"を含む
		if (clipLower.find(
			targetLower) !=
			std::string::npos)
		{
			return &clip;
		}
	}

	return nullptr;
}

//=====================================================
// AnimationID名称
//=====================================================

const char*
AnimatorComponent::GetAnimationName(
	AnimationID id)
{
	switch (id)
	{
	case AnimationID::Idle:
		return "Idle";

	case AnimationID::IdleAttacking:
		return "Idle_Attacking";

	case AnimationID::IdleWeapon:
		return "Idle_Weapon";

	case AnimationID::Walk:
		return "Walk";

	case AnimationID::Run:
		return "Run";

	case AnimationID::RunWeapon:
		return "Run_Weapon";

	case AnimationID::Attack:
		return "Sword_Attack";

	case AnimationID::AttackFast:
		return "Sword_AttackFast";

	case AnimationID::Punch:
		return "Punch";

	case AnimationID::PickUp:
		return "PickUp";

	case AnimationID::Roll:
		return "Roll";

	case AnimationID::Damage:
		// 元FBX側のスペルに合わせる
		return "RecieveHit";

	case AnimationID::Damage2:
		return "RecieveHit_2";

	case AnimationID::Death:
		return "Death";

	case AnimationID::Mixamo:
		return "mixamo.com";

	default:
		break;
	}

	return "";
}

Vector3 AnimatorComponent::SamplePosition(
	const BoneAnimation& animation,
	double time,
	const Vector3& defaultValue) const
{
	const auto& keys =
		animation.PositionKeys;

	if (keys.empty())
	{
		return defaultValue;
	}

	if (keys.size() == 1)
	{
		return keys[0].Value;
	}

	if (time <= keys.front().Time)
	{
		return keys.front().Value;
	}

	if (time >= keys.back().Time)
	{
		return keys.back().Value;
	}

	for (size_t i = 0;
		i + 1 < keys.size();
		++i)
	{
		const auto& current =
			keys[i];

		const auto& next =
			keys[i + 1];

		if (time < next.Time)
		{
			const double duration =
				next.Time -
				current.Time;

			if (duration <= 0.0)
			{
				return current.Value;
			}

			const float t =
				static_cast<float>(
					(time - current.Time) /
					duration);

			return LerpVector3(
				current.Value,
				next.Value,
				t);
		}
	}

	return keys.back().Value;
}

Vector3 AnimatorComponent::SampleScale(
	const BoneAnimation& animation,
	double time,
	const Vector3& defaultValue) const
{
	const auto& keys =
		animation.ScaleKeys;

	if (keys.empty())
	{
		return defaultValue;
	}

	if (keys.size() == 1)
	{
		return keys[0].Value;
	}

	if (time <= keys.front().Time)
	{
		return keys.front().Value;
	}

	if (time >= keys.back().Time)
	{
		return keys.back().Value;
	}

	for (size_t i = 0;
		i + 1 < keys.size();
		++i)
	{
		const auto& current =
			keys[i];

		const auto& next =
			keys[i + 1];

		if (time < next.Time)
		{
			const double duration =
				next.Time -
				current.Time;

			if (duration <= 0.0)
			{
				return current.Value;
			}

			const float t =
				static_cast<float>(
					(time - current.Time) /
					duration);

			return LerpVector3(
				current.Value,
				next.Value,
				t);
		}
	}

	return keys.back().Value;
}

Quaternion AnimatorComponent::SampleRotation(
	const BoneAnimation& animation,
	double time,
	const Quaternion& defaultValue) const
{
	const auto& keys =
		animation.RotationKeys;

	if (keys.empty())
	{
		return defaultValue;
	}

	if (keys.size() == 1)
	{
		return keys[0].Value;
	}

	if (time <= keys.front().Time)
	{
		return keys.front().Value;
	}

	if (time >= keys.back().Time)
	{
		return keys.back().Value;
	}

	for (size_t i = 0;
		i + 1 < keys.size();
		++i)
	{
		const auto& current =
			keys[i];

		const auto& next =
			keys[i + 1];

		if (time < next.Time)
		{
			const double duration =
				next.Time -
				current.Time;

			if (duration <= 0.0)
			{
				return current.Value;
			}

			const float t =
				static_cast<float>(
					(time - current.Time) /
					duration);

			return SlerpQuaternion(
				current.Value,
				next.Value,
				t);
		}
	}

	return keys.back().Value;
}

const BoneAnimation*
AnimatorComponent::FindBoneAnimation(
	const AnimationClip& clip,
	const std::string& boneName) const
{
	for (const auto& animation :
		clip.BoneAnimations)
	{
		if (animation.BoneName ==
			boneName)
		{
			return &animation;
		}
	}

	return nullptr;
}

void AnimatorComponent::UpdateAnimationTime(
	double deltaSeconds)
{
	if (!m_Playing ||
		m_Paused)
	{
		return;
	}

	const AnimationClip* clip =
		GetCurrentClip();

	if (!clip)
	{
		return;
	}

	if (clip->Duration <= 0.0)
	{
		return;
	}

	// FBXによって0の場合があるためフォールバック
	const double ticksPerSecond =
		clip->TicksPerSecond > 0.0
		? clip->TicksPerSecond
		: 30.0;

	m_CurrentTime +=
		deltaSeconds *
		ticksPerSecond *
		static_cast<double>(
			m_Speed);

	//====================
	// Loop
	//====================

	if (m_Loop)
	{
		m_CurrentTime =
			std::fmod(
				m_CurrentTime,
				clip->Duration);

		// 負のSpeedにも一応対応
		if (m_CurrentTime < 0.0)
		{
			m_CurrentTime +=
				clip->Duration;
		}
	}
	else
	{
		if (m_CurrentTime >=
			clip->Duration)
		{
			m_CurrentTime =
				clip->Duration;

			m_Playing =
				false;
		}
		else if (m_CurrentTime < 0.0)
		{
			m_CurrentTime = 0.0;

			m_Playing =
				false;
		}
	}
}

void AnimatorComponent::UpdateBonePose()
{
	//====================
	// ModelComponent
	//====================

	GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return;
	}

	ModelComponent* modelComponent =
		owner->
		GetComponent<ModelComponent>();

	if (!modelComponent)
	{
		return;
	}

	const auto model =
		modelComponent->
		GetModel();

	if (!model)
	{
		return;
	}

	const AnimationClip* clip =
		GetCurrentClip();

	if (!clip)
	{
		return;
	}

	const SkeletonData& skeleton =
		model->GetSkeleton();

	const size_t boneCount =
		skeleton.Bones.size();

	if (boneCount == 0)
	{
		return;
	}

	//====================
	// Matrix領域確保
	//====================

	m_GlobalBoneMatrices.resize(
		boneCount);

	m_FinalBoneMatrices.resize(
		boneCount);

	//=================================================
	// Bone計算
	//=================================================
	//
	// BuildSkeleton()では親→子の順にBonesへ追加しているため、
	// 0から順番に処理すれば親GlobalMatrixが既に計算済みになる。
	//

	for (size_t boneIndex = 0;
		boneIndex < boneCount;
		++boneIndex)
	{
		const BoneData& bone =
			skeleton.Bones[
				boneIndex];

		//====================
		// Bind Pose
		//====================

		Vector3 defaultScale;
		Quaternion defaultRotation;
		Vector3 defaultPosition;

		DecomposeTransform(
			bone.LocalTransform,
			defaultScale,
			defaultRotation,
			defaultPosition);

		// デフォルトではFBXのBind Pose
		Matrix4x4 localMatrix =
			bone.LocalTransform;

		//====================
		// Animation Channel
		//====================

		const BoneAnimation*
			boneAnimation =
			FindBoneAnimation(
				*clip,
				bone.Name);

		if (boneAnimation)
		{
			const Vector3 position =
				SamplePosition(
					*boneAnimation,
					m_CurrentTime,
					defaultPosition);

			const Quaternion rotation =
				SampleRotation(
					*boneAnimation,
					m_CurrentTime,
					defaultRotation);

			const Vector3 scale =
				SampleScale(
					*boneAnimation,
					m_CurrentTime,
					defaultScale);

			//====================
			// Local Transform
			//====================
			//
			// DirectX / SimpleMathでは
			// Row Vector形式として
			// Scale → Rotation → Translation
			// の順で合成する。
			//

			localMatrix =
				Matrix4x4::CreateScale(
					scale) *
				Matrix4x4::CreateFromQuaternion(
					rotation) *
				Matrix4x4::CreateTranslation(
					position);
		}

		//====================
		// Global Transform
		//====================

		if (bone.ParentIndex < 0)
		{
			m_GlobalBoneMatrices[
				boneIndex] =
				localMatrix;
		}
		else
		{
			const size_t parentIndex =
				static_cast<size_t>(
					bone.ParentIndex);

			m_GlobalBoneMatrices[
				boneIndex] =
				localMatrix *
					m_GlobalBoneMatrices[
						parentIndex];
		}

		//====================
		// Final Bone Matrix
		//====================
		// Assimp(column-vector)で一般的な
		// GlobalInverse
		// * Global
		// * Offset
		// を、現在のDirectX row-vector表現へ
		// 変換した順序。
		//

		m_FinalBoneMatrices[
			boneIndex] =
			bone.OffsetMatrix *
				m_GlobalBoneMatrices[
					boneIndex] *
				skeleton.
						GlobalInverseTransform;
	}
}

bool AnimatorComponent::SetAnimations(
	const std::vector<AnimationClip>& clips)
{
	if (clips.empty())
	{
		return false;
	}

	bool registered = false;

	constexpr AnimationID animationIDs[] =
	{
		AnimationID::Idle,
		AnimationID::IdleAttacking,
		AnimationID::IdleWeapon,

		AnimationID::Walk,
		AnimationID::Run,
		AnimationID::RunWeapon,

		AnimationID::Attack,
		AnimationID::AttackFast,
		AnimationID::Punch,
		AnimationID::PickUp,
		AnimationID::Roll,

		AnimationID::Damage,
		AnimationID::Damage2,
		AnimationID::Death,

		AnimationID::Mixamo
	};

	for (const AnimationID id :
	animationIDs)
	{
		const AnimationClip* clip =
			FindClip(
				id,
				clips);

		if (!clip)
		{
			// Modelによって存在しないAnimationが
			// あっても問題ない。
			continue;
		}

		m_Animations[id] =
			*clip;

		registered = true;
	}

	return registered;
}


void AnimatorComponent::DrawDebugUI()
{
	ImGui::Text(
		"Registered Animations: %zu",
		m_Animations.size());

	ImGui::Separator();

	//====================
	// Current Animation
	//====================

	ImGui::Text(
		"Current: %s",
		GetAnimationName(
			m_CurrentAnimation));

	ImGui::Text(
		"Time: %.3f",
		m_CurrentTime);

	ImGui::Checkbox(
		"Paused",
		&m_Paused);

	ImGui::Checkbox(
		"Loop",
		&m_Loop);

	ImGui::DragFloat(
		"Speed",
		&m_Speed,
		0.05f,
		0.0f,
		5.0f);

	ImGui::Separator();

	//====================
	// Registered Clips
	//====================

	for (const auto& [id, clip] :
		m_Animations)
	{
		ImGui::PushID(
			static_cast<int>(id));

		ImGui::Text(
			"%s",
			clip.Name.c_str());

		ImGui::SameLine();

		if (ImGui::Button("Play"))
		{
			PlayAnimation(
				id);
		}

		ImGui::PopID();
	}

	const AnimationClip* clip =
		GetCurrentClip();

	if (clip)
	{
		const double ticksPerSecond =
			clip->TicksPerSecond > 0.0
			? clip->TicksPerSecond
			: 30.0;

		const double currentSeconds =
			m_CurrentTime /
			ticksPerSecond;

		const double durationSeconds =
			clip->Duration /
			ticksPerSecond;

		ImGui::Text(
			"Time(Tick): %.3f / %.3f",
			m_CurrentTime,
			clip->Duration);

		ImGui::Text(
			"Time(Sec): %.3f / %.3f",
			currentSeconds,
			durationSeconds);

		ImGui::Text(
			"Bone Matrices: %zu",
			m_FinalBoneMatrices.size());
	}
}
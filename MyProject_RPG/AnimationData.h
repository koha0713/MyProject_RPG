#pragma once

/**
 * @file AnimationData.h
 * @brief Animationに使用するデータ構造
 */

#include <string>
#include <vector>

#include "CommonType.h"

 /**
  * @brief GPU Skinningで使用できる最大Bone数
  *
  * @details
  * 128 * 64byte = 8192byteなので、
  * D3D11 ConstantBuffer上限64KBに十分収まる。
  */
constexpr size_t MAX_BONES = 128;

 /**
  * @brief ゲーム側からAnimationを識別するID
  *
  * @details
  * FBX内のAnimation名とは分離する。
  *
 * AnimationIDはゲーム側で使用する識別子であり、
 * 実際のFBX Animation名との対応は
 * AnimatorComponent側で管理する。
  */
enum class AnimationID
{
	None = 0,

	//====================
	// Idle
	//====================

	// 通常Idle
	Idle,

	// 戦闘時Idle
	IdleAttacking,

	// 武器構えIdle
	IdleWeapon,

	//====================
	// Locomotion
	//====================

	Walk,
	Run,
	RunWeapon,

	//====================
	// Attack
	//====================

	// 基本攻撃
	Attack,

	// 2つ目の攻撃
	Attack2,

	// 3つ目の攻撃
	Attack3,

	// 高速攻撃
	// 弓などではAttack後の後動作にも使用する。
	AttackFast,

	Punch,

	//====================
	// Action
	//====================

	PickUp,
	Roll,

	//====================
	// Damage / Death
	//====================

	// 通常時Damage
	// 現状では基本的に使用しない。
	Damage,

	// 戦闘時Damage
	Damage2,

	Death,

	//====================
	// Legacy
	//====================

	Mixamo
};

/**
 * @brief Vector3用KeyFrame
 */
struct VectorKeyFrame
{
	// Animation開始からのTick時間
	double Time = 0.0;

	// KeyFrame値
	Vector3 Value =
		Vector3(
			0.0f,
			0.0f,
			0.0f);
};

/**
 * @brief Quaternion用KeyFrame
 */
struct QuaternionKeyFrame
{
	// Animation開始からのTick時間
	double Time = 0.0;

	// 回転値
	//
	// Quaternion::Identityは
	// SimpleMath.cppへの依存になるため使用しない。
	Quaternion Value =
		Quaternion(
			0.0f,
			0.0f,
			0.0f,
			1.0f);
};

/**
 * @brief 1Node/Bone分のAnimationデータ
 *
 * @details
 * AssimpのaiNodeAnim 1つに対応する。
 */
struct BoneAnimation
{
	// 対象Node/Bone名
	std::string BoneName;

	// 移動Key
	std::vector<VectorKeyFrame>
		PositionKeys;

	// 回転Key
	std::vector<QuaternionKeyFrame>
		RotationKeys;

	// Scale Key
	std::vector<VectorKeyFrame>
		ScaleKeys;
};

/**
 * @brief 1つのAnimationClip
 *
 * @details
 * Walk / Run / Attackなど、
 * 1つの再生可能なAnimationを表す。
 */
struct AnimationClip
{
	// FBX内に記録されているAnimation名
	std::string Name;

	// Animation全体の長さ
	// Assimp上ではTick単位
	double Duration = 0.0;

	// 1秒間のTick数
	double TicksPerSecond = 0.0;

	// Bone/NodeごとのAnimation
	std::vector<BoneAnimation>
		BoneAnimations;
};
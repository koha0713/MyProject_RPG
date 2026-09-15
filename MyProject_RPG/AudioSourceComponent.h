#pragma once

/**
 * @file AudioSourceComponent.h
 * @brief GameObjectにSound再生機能を追加するComponent
 */

#include <string>

#include "Component.h"
#include "SoundCategory.h"
#include "SoundHandle.h"
#include "CommonType.h"

 /**
  * @brief Audio Source Component
  *
  * @details
  * 2D / 3D Soundの再生を担当する。
  *
  * 3D Soundの場合はOwnerのTransformComponentから
  * World座標を取得し、SoundManagerのEmitter位置へ反映する。
  *
  * SoundManager:
  *   実際のAudio再生・Voice管理
  *
  * AudioSourceComponent:
  *   GameObjectとSoundを関連付ける
  */
class AudioSourceComponent :
	public Component
{
public:

	AudioSourceComponent() = default;
	~AudioSourceComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// Playback
	//====================

	/**
	 * @brief Soundを再生する
	 *
	 * @details
	 * すでに再生中の場合は一度停止してから再生する。
	 */
	void Play();

	/**
	 * @brief SoundをOneShot再生する
	 *
	 * @details
	 * 現在管理しているLoop Soundなどには影響せず、
	 * 一時的なSoundを重ねて再生する。
	 *
	 * 再生終了後のVoice破棄はSoundManagerが担当する。
	 *
	 * @param filePath 再生するSound
	 * @param volume   OneShot個別Volume
	 */
	void PlayOneShot(
		const std::wstring& filePath,
		float volume = 1.0f);

	/**
	 * @brief 現在設定されているSoundPathをOneShot再生する
	 *
	 * @param volume OneShot個別Volume
	 */
	void PlayOneShot(
		float volume = 1.0f);

	/**
	 * @brief Soundを停止する
	 */
	void Stop();

	/**
	 * @brief 現在再生中か
	 */
	bool IsPlaying() const;

	//====================
	// Sound設定
	//====================

	void SetSoundPath(
		const std::wstring& filePath)
	{
		m_SoundPath =
			filePath;
	}

	const std::wstring&
		GetSoundPath() const
	{
		return m_SoundPath;
	}

	void SetLoop(
		bool loop)
	{
		m_Loop =
			loop;
	}

	bool IsLoop() const
	{
		return m_Loop;
	}

	/**
	 * @brief 2D / 3D Soundを切り替える
	 *
	 * @note
	 * 再生中のSoundには即時反映されない。
	 * 次回Play時から反映される。
	 */
	void Set3D(
		bool is3D)
	{
		m_Is3D =
			is3D;
	}

	bool Is3D() const
	{
		return m_Is3D;
	}

	/**
	 * @brief 個別Volume設定
	 */
	void SetVolume(
		float volume);

	float GetVolume() const
	{
		return m_Volume;
	}

	void SetCategory(
		SoundCategory category)
	{
		m_Category =
			category;
	}

	SoundCategory GetCategory() const
	{
		return m_Category;
	}

	/**
	 * @brief 3D Soundの距離スケール設定
	 *
	 * @details
	 * 現在はPlay3D()のDistanceScalerとして使用する。
	 */
	void SetDistanceScaler(
		float distanceScaler);

	float GetDistanceScaler() const
	{
		return m_DistanceScaler;
	}

	/**
	 * @brief Initialize時に自動再生するか
	 *
	 * @details
	 * BGM、環境音、焚き火などの
	 * Loop Soundで利用できる。
	 */
	void SetPlayOnStart(
		bool playOnStart)
	{
		m_PlayOnStart =
			playOnStart;
	}

	bool IsPlayOnStart() const
	{
		return m_PlayOnStart;
	}

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "AudioSourceComponent";
	}

private:

	/**
	 * @brief OwnerのWorld位置を取得する
	 */
	bool GetOwnerPosition(
		Vector3& position) const;

private:

	//====================
	// Sound Resource
	//====================

	std::wstring m_SoundPath;

	//====================
	// Playback
	//====================

	SoundHandle m_SoundHandle;

	bool m_Loop =
		false;

	bool m_Is3D =
		true;

	bool m_PlayOnStart =
		false;

	float m_Volume =
		1.0f;

	SoundCategory m_Category =
		SoundCategory::SE;

	/**
	 * @brief 3D Soundの距離減衰スケール
	 */
	float m_DistanceScaler =
		20.0f;
};
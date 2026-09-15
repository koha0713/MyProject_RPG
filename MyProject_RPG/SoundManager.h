#pragma once

/**
 * @file SoundManager.h
 * @brief XAudio2によるSound管理
 */

#include <Windows.h>
#include <xaudio2.h>
#include <x3daudio.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

#include "NonCopyable.h"
#include "SoundData.h"
#include "SoundHandle.h"
#include "SoundCategory.h"
#include <CommonType.h>

using Microsoft::WRL::ComPtr;


/**
 * @brief Sound Manager
 *
 * @details
 * XAudio2の初期化、音声Resource、
 * SourceVoiceの再生状態を一元管理する。
 *
 * 将来的にX3DAudioを追加し、
 * 3D Soundへ拡張する。
 */
class SoundManager :
	public NonCopyable
{
public:

	static SoundManager&
		GetInstance()
	{
		static SoundManager instance;
		return instance;
	}

	//====================
	// Lifecycle
	//====================

	bool Initialize();

	void Finalize();

	/**
	 * @brief 毎フレーム更新
	 *
	 * @details
	 * 再生終了したSourceVoiceを破棄する。
	 */
	void Update();

	//====================
	// Resource
	//====================

	/**
	 * @brief Soundをロードする
	 *
	 * @details
	 * 同じPathはCacheから返す。
	 */
	std::shared_ptr<SoundData>
		LoadSound(
			const std::wstring& filePath);

	//====================
	// Playback
	//====================

	/**
	 * @brief 2D Sound再生
	 *
	 * @param filePath WAVファイル
	 * @param loop     Loop再生
	 * @param volume   0.0 ～ 1.0
	 */
	SoundHandle Play2D(
		const std::wstring& filePath,
		bool loop = false,
		float volume = 1.0f,
		SoundCategory category =
		SoundCategory::SE);

	/**
	 * @brief 3D Soundを再生
	 *
	 * @param filePath 音声ファイル
	 * @param position World座標
	 * @param loop Loop再生するか
	 * @param volume 個別Volume
	 * @param category Soundカテゴリ
	 * @param maxDistance 音が十分減衰する距離
	 */
	SoundHandle Play3D(
		const std::wstring& filePath,
		const Vector3& position,
		bool loop = false,
		float volume = 1.0f,
		SoundCategory category =
		SoundCategory::SE,
		float maxDistance = 20.0f);

	/**
	 * @brief Sound停止
	 */
	void Stop(
		SoundHandle handle);

	/**
	 * @brief 再生中か
	 */
	bool IsPlaying(
		SoundHandle handle) const;

	//====================
	// Volume
	//====================

	/**
	 * @brief Sound個別Volume変更
	 */
	void SetVolume(
		SoundHandle handle,
		float volume);

	/**
	 * @brief Category Volume変更
	 */
	void SetCategoryVolume(
		SoundCategory category,
		float volume);

	/**
	 * @brief Category Volume取得
	 */
	float GetCategoryVolume(
		SoundCategory category) const;

	/**
	 * @brief Master Volume変更
	 */
	void SetMasterVolume(
		float volume);

	float GetMasterVolume() const
	{
		return m_MasterVolume;
	}

	/**
	 * @brief 3D Audio Listenerを設定
	 *
	 * @param position ListenerのWorld座標
	 * @param forward 正規化された前方向
	 * @param up 正規化された上方向
	 */
	void SetListener(
		const Vector3& position,
		const Vector3& forward,
		const Vector3& up);

	/**
	 * @brief 3D Soundの位置を変更
	 */
	void SetEmitterPosition(
		SoundHandle handle,
		const Vector3& position);

private:

	SoundManager() = default;
	~SoundManager() = default;

	/**
	 * @brief 再生中Sound Instance
	 */
	struct PlayingSound
	{
		SoundHandle Handle;

		IXAudio2SourceVoice*
			Voice = nullptr;

		std::shared_ptr<SoundData>
			Data;

		/**
		 * @brief Loop再生か
		 */
		bool Loop = false;

		/**
		 * @brief Sound用途
		 */
		SoundCategory Category =
			SoundCategory::SE;

		/**
		 * @brief このSound自身のVolume
		 *
		 * @details
		 * 実際にSourceVoiceへ設定するVolumeは
		 *
		 * BaseVolume * CategoryVolume
		 *
		 * となる。
		 */
		float BaseVolume =
			1.0f;

		/**
		 * @brief 3D Soundか
		 */
		bool Is3D = false;

		/**
		 * @brief 3D SoundのWorld座標
		 */
		Vector3 Position =
			Vector3(
				0.0f,
				0.0f,
				0.0f);

		/**
		 * @brief 距離減衰の基準距離
		 */
		float MaxDistance =
			20.0f;

		/**
		 * @brief X3DAudio Emitter
		 */
		X3DAUDIO_EMITTER
			Emitter{};
	};

	/**
	 * @brief 1つの3D SoundへSpatial計算を適用
	 */
	void Update3DSound(
		PlayingSound& sound);

	/**
	 * @brief 指定Handleを検索
	 */
	PlayingSound*
		FindPlayingSound(
			SoundHandle handle);

	const PlayingSound*
		FindPlayingSound(
			SoundHandle handle) const;

	/**
	 * @brief SourceVoice破棄
	 */
	static void DestroyVoice(
		PlayingSound& sound);

	/**
	 * @brief SourceVoiceへ最終Volumeを適用
	 */
	void ApplyVolume(
		PlayingSound& sound);

private:

	//====================
	// XAudio2
	//====================

	ComPtr<IXAudio2>
		m_XAudio2;

	IXAudio2MasteringVoice*
		m_MasteringVoice =
		nullptr;

	//====================
	// Resource Cache
	//====================

	std::unordered_map<
		std::wstring,
		std::shared_ptr<SoundData>>
		m_SoundCache;

	//====================
	// Playing Sounds
	//====================

	std::vector<PlayingSound>
		m_PlayingSounds;

	uint64_t m_NextHandleId =
		1;

	float m_MasterVolume =
		1.0f;

	bool m_Initialized =
		false;

	static constexpr size_t
		SOUND_CATEGORY_COUNT =
		static_cast<size_t>(
			SoundCategory::Count);

	std::array<float, SOUND_CATEGORY_COUNT>
		m_CategoryVolumes
	{
		1.0f,	// BGM
		1.0f,	// SE
		1.0f	// Ambient
	};

	//====================
	// X3DAudio
	//====================

	/**
	 * @brief X3DAudio Instance
	 */
	X3DAUDIO_HANDLE
		m_X3DInstance{};

	/**
	 * @brief Listener
	 */
	X3DAUDIO_LISTENER
		m_Listener{};

	/**
	 * @brief MasteringVoiceのSpeaker構成
	 */
	DWORD
		m_ChannelMask = 0;

	/**
	 * @brief 出力Channel数
	 */
	UINT32
		m_DestinationChannelCount = 0;
};


/**
 * @brief SoundManager簡易アクセス
 */
#define SOUND_MANAGER \
	SoundManager::GetInstance()
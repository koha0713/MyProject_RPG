#include "SoundManager.h"

#include <algorithm>

#include "WavLoader.h"

bool SoundManager::Initialize()
{
	if (m_Initialized)
	{
		return true;
	}

	//=================================================
	// XAudio2作成
	//=================================================

	HRESULT hr =
		XAudio2Create(
			m_XAudio2.GetAddressOf(),
			0,
			XAUDIO2_DEFAULT_PROCESSOR);

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// MasteringVoice作成
	//=================================================

	hr =
		m_XAudio2->CreateMasteringVoice(
			&m_MasteringVoice);

	if (FAILED(hr))
	{
		m_XAudio2.Reset();

		return false;
	}

	m_MasterVolume =
		1.0f;

	m_MasteringVoice->SetVolume(
		m_MasterVolume);

	m_Initialized =
		true;

	m_CategoryVolumes.fill(
		1.0f);

	return true;
}

void SoundManager::Finalize()
{
	//=================================================
	// Playing Sound破棄
	//=================================================

	for (PlayingSound& sound :
		m_PlayingSounds)
	{
		DestroyVoice(
			sound);
	}

	m_PlayingSounds.clear();

	//=================================================
	// Resource Cache
	//=================================================

	m_SoundCache.clear();

	//=================================================
	// MasteringVoice
	//=================================================

	if (m_MasteringVoice)
	{
		m_MasteringVoice->DestroyVoice();

		m_MasteringVoice =
			nullptr;
	}

	//=================================================
	// XAudio2
	//=================================================

	m_XAudio2.Reset();

	m_NextHandleId =
		1;

	m_MasterVolume =
		1.0f;

	m_Initialized =
		false;
}

void SoundManager::Update()
{
	if (!m_Initialized)
	{
		return;
	}

	//=================================================
	// 再生終了したVoiceを削除
	//=================================================
	//
	// BuffersQueued == 0 なら
	// SubmitしたAudio Bufferの再生が終了している。
	//
	// Loop SoundはBuffersQueuedが残り続けるため
	// 自動削除されない。
	//

	for (auto iterator =
		m_PlayingSounds.begin();
		iterator !=
		m_PlayingSounds.end();)
	{
		PlayingSound& sound =
			*iterator;

		if (!sound.Voice)
		{
			iterator =
				m_PlayingSounds.erase(
					iterator);

			continue;
		}

		XAUDIO2_VOICE_STATE state{};

		sound.Voice->GetState(
			&state);

		if (!sound.Loop &&
			state.BuffersQueued == 0)
		{
			DestroyVoice(
				sound);

			iterator =
				m_PlayingSounds.erase(
					iterator);

			continue;
		}

		++iterator;
	}
}

std::shared_ptr<SoundData>
SoundManager::LoadSound(
	const std::wstring& filePath)
{
	//=================================================
	// Cache検索
	//=================================================

	auto found =
		m_SoundCache.find(
			filePath);

	if (found !=
		m_SoundCache.end())
	{
		return found->second;
	}

	//=================================================
	// File読み込み
	//=================================================

	std::shared_ptr<SoundData>
		soundData =
		WavLoader::Load(
			filePath);

	if (!soundData)
	{
		return nullptr;
	}

	m_SoundCache.emplace(
		filePath,
		soundData);

	return soundData;
}

SoundHandle SoundManager::Play2D(
	const std::wstring& filePath,
	bool loop,
	float volume,
	SoundCategory category)
{
	SoundHandle invalidHandle{};

	if (!m_Initialized ||
		!m_XAudio2)
	{
		return invalidHandle;
	}

	std::shared_ptr<SoundData>
		soundData =
		LoadSound(
			filePath);

	if (!soundData)
	{
		return invalidHandle;
	}

	//=================================================
	// SourceVoice作成
	//=================================================

	IXAudio2SourceVoice*
		sourceVoice =
		nullptr;

	HRESULT hr =
		m_XAudio2->CreateSourceVoice(
			&sourceVoice,
			&soundData->Format);

	if (FAILED(hr) ||
		!sourceVoice)
	{
		return invalidHandle;
	}

	//=================================================
	// Audio Buffer
	//=================================================

	XAUDIO2_BUFFER buffer{};

	buffer.AudioBytes =
		static_cast<UINT32>(
			soundData->AudioData.size());

	buffer.pAudioData =
		soundData->AudioData.data();

	buffer.Flags =
		XAUDIO2_END_OF_STREAM;

	if (loop)
	{
		buffer.LoopCount =
			XAUDIO2_LOOP_INFINITE;
	}

	hr =
		sourceVoice->SubmitSourceBuffer(
			&buffer);

	if (FAILED(hr))
	{
		sourceVoice->DestroyVoice();

		return invalidHandle;
	}

	//=================================================
	// Volume
	//=================================================



	//=================================================
	// Start
	//=================================================

	hr =
		sourceVoice->Start();

	if (FAILED(hr))
	{
		sourceVoice->DestroyVoice();

		return invalidHandle;
	}

	//=================================================
	// Handle生成
	//=================================================

	SoundHandle handle;

	handle.Id =
		m_NextHandleId++;

	PlayingSound playingSound;

	playingSound.Handle =
		handle;

	playingSound.Voice =
		sourceVoice;

	playingSound.Data =
		soundData;

	playingSound.Loop =
		loop;

	playingSound.Category =
		category;

	playingSound.BaseVolume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	// Categoryを含めたVolumeを適用
	ApplyVolume(
		playingSound);

	m_PlayingSounds.emplace_back(
		std::move(
			playingSound));

	return handle;
}

void SoundManager::Stop(
	SoundHandle handle)
{
	PlayingSound* sound =
		FindPlayingSound(
			handle);

	if (!sound ||
		!sound->Voice)
	{
		OutputDebugStringA("Stop Fail\n");
		return;
	}

	sound->Voice->Stop();

	sound->Voice->FlushSourceBuffers();

	DestroyVoice(
		*sound);

	std::erase_if(
		m_PlayingSounds,
		[handle](
			const PlayingSound& playingSound)
		{
			return
				playingSound.Handle ==
				handle;
		});
}

void SoundManager::SetVolume(
	SoundHandle handle,
	float volume)
{
	PlayingSound* sound =
		FindPlayingSound(
			handle);

	if (!sound ||
		!sound->Voice)
	{
		return;
	}

	// 個別Volumeとして保存
	sound->BaseVolume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	// Category Volumeを含めて再計算
	ApplyVolume(
		*sound);
}

void SoundManager::SetCategoryVolume(
	SoundCategory category,
	float volume)
{
	const size_t categoryIndex =
		static_cast<size_t>(
			category);

	if (categoryIndex >=
		m_CategoryVolumes.size())
	{
		return;
	}

	volume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	m_CategoryVolumes[
		categoryIndex] =
		volume;

		//=================================================
		// 再生中Soundへ即時反映
		//=================================================

		for (PlayingSound& sound :
			m_PlayingSounds)
		{
			if (sound.Category !=
				category)
			{
				continue;
			}

			ApplyVolume(
				sound);
		}
}

float SoundManager::GetCategoryVolume(
	SoundCategory category) const
{
	const size_t categoryIndex =
		static_cast<size_t>(
			category);

	if (categoryIndex >=
		m_CategoryVolumes.size())
	{
		return 0.0f;
	}

	return
		m_CategoryVolumes[
			categoryIndex];
}

bool SoundManager::IsPlaying(
	SoundHandle handle) const
{
	const PlayingSound* sound =
		FindPlayingSound(
			handle);

	if (!sound ||
		!sound->Voice)
	{
		return false;
	}

	XAUDIO2_VOICE_STATE state{};

	sound->Voice->GetState(
		&state);

	return
		state.BuffersQueued > 0;
}

void SoundManager::SetMasterVolume(
	float volume)
{
	if (!m_MasteringVoice)
	{
		return;
	}

	m_MasterVolume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	m_MasteringVoice->SetVolume(
		m_MasterVolume);
}


SoundManager::PlayingSound*
SoundManager::FindPlayingSound(
	SoundHandle handle)
{
	for (PlayingSound& sound :
		m_PlayingSounds)
	{
		if (sound.Handle ==
			handle)
		{
			return &sound;
		}
	}

	return nullptr;
}

const SoundManager::PlayingSound*
SoundManager::FindPlayingSound(
	SoundHandle handle) const
{
	for (const PlayingSound& sound :
		m_PlayingSounds)
	{
		if (sound.Handle ==
			handle)
		{
			return &sound;
		}
	}

	return nullptr;
}

void SoundManager::DestroyVoice(
	PlayingSound& sound)
{
	if (!sound.Voice)
	{
		return;
	}

	sound.Voice->Stop();

	sound.Voice->DestroyVoice();

	sound.Voice =
		nullptr;

	sound.Data.reset();
}

void SoundManager::ApplyVolume(
	PlayingSound& sound)
{
	if (!sound.Voice)
	{
		return;
	}

	const size_t categoryIndex =
		static_cast<size_t>(
			sound.Category);

	if (categoryIndex >=
		m_CategoryVolumes.size())
	{
		return;
	}

	//=================================================
	// Final Volume
	//=================================================
	//
	// SourceVoiceでは
	//
	// 個別Volume × CategoryVolume
	//
	// のみ計算する。
	//
	// MasterVolumeはMasteringVoice側で処理されるため、
	// ここで掛けると二重適用になる。
	//

	const float finalVolume =
		sound.BaseVolume *
		m_CategoryVolumes[
			categoryIndex];

	sound.Voice->SetVolume(
		finalVolume);
}
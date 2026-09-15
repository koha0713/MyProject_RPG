#include "AudioSourceComponent.h"

#include <algorithm>

#include "DebugUI.h"
#include "GameObject.h"
#include "SoundManager.h"
#include "TransformComponent.h"

//=====================================================
// Lifecycle
//=====================================================

void AudioSourceComponent::Initialize()
{
	//=================================================
	// Auto Play
	//=================================================
	//
	// 環境音やBGMなど、
	// GameObject生成時から鳴らしたいSoundに使用する。
	//

	if (m_PlayOnStart &&
		!m_SoundPath.empty())
	{
		Play();
	}
}

void AudioSourceComponent::Finalize()
{
	// Component破棄後にSourceVoiceだけが
	// 残らないよう必ず停止する。
	Stop();
}

void AudioSourceComponent::Update(
	uint64_t delta)
{
	(void)delta;

	//=================================================
	// 3D SoundのみTransformへ追従
	//=================================================

	if (!m_Is3D)
	{
		return;
	}

	if (!IsPlaying())
	{
		return;
	}

	Vector3 position(
		0.0f,
		0.0f,
		0.0f);

	if (!GetOwnerPosition(
		position))
	{
		return;
	}

	// OwnerのTransform位置を
	// X3DAudio Emitterへ反映する。
	SOUND_MANAGER.SetEmitterPosition(
		m_SoundHandle,
		position);
}

void AudioSourceComponent::Draw()
{
}

//=====================================================
// Playback
//=====================================================

void AudioSourceComponent::Play()
{
	if (m_SoundPath.empty())
	{
		OutputDebugStringA(
			"[AudioSourceComponent] SoundPath is empty.\n");

		return;
	}

	//=================================================
	// 多重再生防止
	//=================================================
	//
	// 1つのAudioSourceComponentは
	// 1つの再生Instanceを管理する。
	//

	if (m_SoundHandle.IsValid())
	{
		Stop();
	}

	//=================================================
	// 3D Sound
	//=================================================

	if (m_Is3D)
	{
		Vector3 position(
			0.0f,
			0.0f,
			0.0f);

		if (!GetOwnerPosition(
			position))
		{
			OutputDebugStringA(
				"[AudioSourceComponent] "
				"TransformComponent not found.\n");

			return;
		}

		m_SoundHandle =
			SOUND_MANAGER.Play3D(
				m_SoundPath,
				position,
				m_Loop,
				m_Volume,
				m_Category,
				m_DistanceScaler);
	}

	//=================================================
	// 2D Sound
	//=================================================

	else
	{
		m_SoundHandle =
			SOUND_MANAGER.Play2D(
				m_SoundPath,
				m_Loop,
				m_Volume,
				m_Category);
	}

	//=================================================
	// Result
	//=================================================

	if (!m_SoundHandle.IsValid())
	{
		OutputDebugStringA(
			"[AudioSourceComponent] Play failed.\n");
	}
}

void AudioSourceComponent::PlayOneShot(
	const std::wstring& filePath,
	float volume)
{
	if (filePath.empty())
	{
		OutputDebugStringA(
			"[AudioSourceComponent] "
			"OneShot SoundPath is empty.\n");

		return;
	}

	//=================================================
	// Volume
	//=================================================

	volume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	//=================================================
	// 3D OneShot
	//=================================================

	if (m_Is3D)
	{
		Vector3 position(
			0.0f,
			0.0f,
			0.0f);

		if (!GetOwnerPosition(
			position))
		{
			OutputDebugStringA(
				"[AudioSourceComponent] "
				"TransformComponent not found.\n");

			return;
		}

		// OneShotなのでLoopはfalse。
		//
		// Handleは保持しない。
		// 再生終了後はSoundManager::Update()が
		// Voiceを自動的に破棄する。
		SOUND_MANAGER.Play3D(
			filePath,
			position,
			false,
			volume,
			m_Category,
			m_DistanceScaler);

		return;
	}

	//=================================================
	// 2D OneShot
	//=================================================

	SOUND_MANAGER.Play2D(
		filePath,
		false,
		volume,
		m_Category);
}

void AudioSourceComponent::PlayOneShot(
	float volume)
{
	if (m_SoundPath.empty())
	{
		OutputDebugStringA(
			"[AudioSourceComponent] "
			"SoundPath is empty.\n");

		return;
	}

	PlayOneShot(
		m_SoundPath,
		volume);
}

void AudioSourceComponent::Stop()
{
	if (!m_SoundHandle.IsValid())
	{
		return;
	}

	SOUND_MANAGER.Stop(
		m_SoundHandle);

	m_SoundHandle = {};
}

bool AudioSourceComponent::IsPlaying() const
{
	if (!m_SoundHandle.IsValid())
	{
		return false;
	}

	return
		SOUND_MANAGER.IsPlaying(
			m_SoundHandle);
}

//=====================================================
// Settings
//=====================================================

void AudioSourceComponent::SetVolume(
	float volume)
{
	m_Volume =
		std::clamp(
			volume,
			0.0f,
			1.0f);

	// 再生中なら即座に反映する。
	if (m_SoundHandle.IsValid())
	{
		SOUND_MANAGER.SetVolume(
			m_SoundHandle,
			m_Volume);
	}
}

void AudioSourceComponent::SetDistanceScaler(
	float distanceScaler)
{
	if (distanceScaler <= 0.0f)
	{
		distanceScaler =
			0.01f;
	}

	m_DistanceScaler =
		distanceScaler;
}

//=====================================================
// Transform
//=====================================================

bool AudioSourceComponent::GetOwnerPosition(
	Vector3& position) const
{
	const GameObject* owner =
		GetOwner();

	if (!owner)
	{
		return false;
	}

	const TransformComponent* transform =
		owner->GetComponent<
		TransformComponent>();

	if (!transform)
	{
		return false;
	}

	position =
		transform->GetPosition();

	return true;
}

//=====================================================
// Debug UI
//=====================================================
void AudioSourceComponent::DrawDebugUI()
{
	//====================
	// Playback State
	//====================

	const bool isPlaying =
		IsPlaying();

	ImGui::Text(
		"State: %s",
		isPlaying
		? "Playing"
		: "Stopped");

	ImGui::Text(
		"Mode: %s",
		m_Is3D
		? "3D"
		: "2D");

	ImGui::Separator();

	//====================
	// Playback Settings
	//====================

	ImGui::Checkbox(
		"Loop",
		&m_Loop);

	ImGui::Checkbox(
		"3D Sound",
		&m_Is3D);

	ImGui::Checkbox(
		"Play On Start",
		&m_PlayOnStart);

	//====================
	// Volume
	//====================

	float volume =
		m_Volume;

	if (ImGui::DragFloat(
		"Volume",
		&volume,
		0.01f,
		0.0f,
		1.0f))
	{
		SetVolume(
			volume);
	}

	ImGui::Separator();

	//====================
	// Category
	//====================

	const char* categoryNames[] =
	{
		"BGM",
		"SE",
		"Ambient"
	};

	int categoryIndex =
		static_cast<int>(
			m_Category);

	if (ImGui::Combo(
		"Category",
		&categoryIndex,
		categoryNames,
		static_cast<int>(
			SoundCategory::Count)))
	{
		m_Category =
			static_cast<SoundCategory>(
				categoryIndex);
	}

	//====================
	// 3D Settings
	//====================

	if (m_Is3D)
	{
		float distanceScaler =
			m_DistanceScaler;

		if (ImGui::DragFloat(
			"Distance Scaler",
			&distanceScaler,
			0.1f,
			0.01f,
			1000.0f))
		{
			SetDistanceScaler(
				distanceScaler);
		}
	}

	ImGui::Separator();

	//====================
	// Playback Controls
	//====================

	if (!isPlaying)
	{
		if (ImGui::Button(
			"Play"))
		{
			Play();
		}
	}
	else
	{
		if (ImGui::Button(
			"Stop"))
		{
			Stop();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(
		"Restart"))
	{
		Stop();
		Play();
	}

	ImGui::Separator();

	//====================
	// OneShot Test
	//====================

	if (ImGui::Button(
		"Play OneShot"))
	{
		PlayOneShot();
	}
}

/*
* 使用例:
auto* audioSource =
	enemy->AddComponent<
		AudioSourceComponent>();

audioSource->SetSoundPath(
	L"Assets/Sound/EnemyGrowl.wav");

audioSource->Set3D(
	true);

audioSource->SetLoop(
	true);

audioSource->SetVolume(
	1.0f);

audioSource->SetCategory(
	SoundCategory::SE);

audioSource->SetDistanceScaler(
	20.0f);

audioSource->Play();
*/
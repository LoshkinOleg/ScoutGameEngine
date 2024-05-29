#pragma once

#include <set>
#include <mutex>

#include <Scout/PreprocessorMacro.h>
#include <Scout/IAudioEngine.h>
#include <Scout/Portaudio/TypedefAndEnum.h>
#include <Scout/Portaudio/Sound.h>

namespace Scout
{
	/*
		TODO:
		- check handle validity before trying to use them
		- allow user to specify device explicitly
		- implement audio input (mic)
		- handle non F32 bit-depths
		- look into:
			- PaStreamParameters::hostApiSpecificStreamInfo
			- PaStreamParameters::suggestedLatency
			- paClipOff
			- PaStreamCallback
	*/
	class AudioEngine_Portaudio final: public IAudioEngine
	{
	public:
		AudioEngine_Portaudio() = delete;
		SCOUT_NO_COPY(AudioEngine_Portaudio);
		SCOUT_NO_MOVE(AudioEngine_Portaudio);

		AudioEngine_Portaudio(const Bitdepth_Portaudio quant, const Samplerate_Portaudio sampleRate, const SpeakerSetup_Portaudio speakersSetup, const std::chrono::milliseconds desiredLatency, const MixingPolicy policy);
		~AudioEngine_Portaudio();

		SoundHandle MakeSound(
			const std::vector<float>& data, const std::uint64_t nrOfChannels,
			const bool interleaved = true) override;

		void PlaySound(const SoundHandle sound) override;
		void PlayOneShot(const SoundHandle sound) override;
		void StopSound(const SoundHandle sound) override;
		void StopAll();

		void SetSoundLooped(const SoundHandle sound, const bool newVal) override;

		void PauseSound(const SoundHandle sound) override;
		void UnpauseSound(const SoundHandle sound) override;
		void SetSoundLooping(const SoundHandle sound, const bool newLooping);

		bool IsSoundPlaying(const SoundHandle sound) const override;
		bool IsSoundPaused(const SoundHandle sound) const override;
		bool IsSoundLooped(const SoundHandle sound) const override;

		Bitdepth GetBitdepth() const override;
		Samplerate GetSamplerate() const override;
		std::uint64_t GetBytesPerFrame() const override;
		std::uint64_t GetNrOfChannels() const override;
		std::uint64_t GetFramesPerBuffer() const override;
		std::uint64_t GetBufferSizeInBytes() const override;
		std::chrono::milliseconds GetBufferLatency() const override;
		std::chrono::milliseconds GetBufferDuration() const override;

		void Update() override;

		SfxHandle RegisterEffectForSound(const SoundSpecificEffectCallback fxCallback, const SoundHandle sound) override;
		SfxHandle RegisterEffectForDisplay(const AudioDisplayEffectCallback fxCallback) override;

		// Warning: for guaranteed accuracy, this access should be protected with the mutex lock.
		const std::vector<float>& GetRawBuffer() const override;

	private:
		static int ServicePortaudio_(const void* input, void* output,
									 unsigned long frameCount,
									 const PaStreamCallbackTimeInfo* timeInfo,
									 PaStreamCallbackFlags statusFlags,
									 void* userData);

	private:
		Bitdepth_Portaudio quant_ = Bitdepth_Portaudio::F32;
		Samplerate_Portaudio sampleRate_ = Samplerate_Portaudio::Hz_48k;
		SpeakerSetup_Portaudio speakerSetup_ = SpeakerSetup_Portaudio::MONO;
		MixingPolicy mixingPolicy_ = MixingPolicy::SUM_AND_CLAMP;
		std::uint32_t framesPerBuffer_ = 0;

		PaStream* pStream_ = nullptr;
		PaDeviceIndex selectedDevice_ = paNoDevice;
		std::vector<float> buffer_{};
		bool update_ = true;
		std::mutex bufferMutex_;

		// std::vector<MonoSound_Portaudio> soundsMono_{};
		// std::vector<StereoSound_Portaudio> soundsStereo_{}; // TODO: merge all sounds into a common structure to remove need to differentiate between kinds of sounds.
		std::vector<MultichannelSound_Portaudio> sounds_{};
		std::set<SoundHandle> playing_{}; // Note: data of a set is not contiguous

		std::vector<AudioDisplayEffectCallback> displayEffects_{}; // Applied to the sum buffer after all sounds have been mixed.
	};
}
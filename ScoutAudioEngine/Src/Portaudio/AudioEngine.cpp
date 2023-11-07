#include <Scout/Portaudio/AudioEngine.h>

#include <iostream>
#include <cstring>
#include <mutex>
#include <algorithm>
#include <cassert>

#include <Scout/Math.h>
#include <Scout/AudioMixing.h>

namespace Scout
{
	AudioEngine_Portaudio::AudioEngine_Portaudio(
		const Bitdepth_Portaudio quant, const Samplerate_Portaudio sampleRate,
		const SpeakerSetup_Portaudio speakersSetup, const std::chrono::milliseconds desiredLatency, const MixingPolicy policy):
		quant_(quant), sampleRate_(sampleRate), speakerSetup_(speakersSetup), mixingPolicy_(policy)
	{
		const double sampleDuration = 1.0 / (double)((std::uint32_t)sampleRate_);
		const double latencyDuration = (double)desiredLatency.count() * 0.001;
		framesPerBuffer_ = (std::uint32_t)NearestUpperPowOfTwo((std::uint64_t)(latencyDuration / sampleDuration));

		int nrOfChannels;
		switch(speakerSetup_)
		{
			case Scout::SpeakerSetup_Portaudio::MONO:
			{
				nrOfChannels = 1;
			}break;

			case Scout::SpeakerSetup_Portaudio::DUAL_MONO:
			{
				nrOfChannels = 2;
			}break;

			case Scout::SpeakerSetup_Portaudio::STEREO:
			{
				nrOfChannels = 2;
			}break;

			default:
			{
				throw std::runtime_error("Unexpected speakerSetup_ .");
			}break;
		}

		auto err = Pa_Initialize();
		if(err != paNoError) throw std::runtime_error(std::string("Failed to initialize PortAudio: ") + Pa_GetErrorText(err));

		selectedDevice_ = Pa_GetDefaultOutputDevice();
		if(selectedDevice_ == paNoDevice) throw std::runtime_error(std::string("PortAudio failed to retireve a default playback device."));

		PaStreamParameters outputParams{
			selectedDevice_,
			nrOfChannels,
			(std::uint32_t)quant_,
			latencyDuration,
			NULL
		};

		err = Pa_OpenStream(
			&pStream_,
			NULL,
			&outputParams,
			(double)sampleRate,
			(unsigned int)framesPerBuffer_,
			paNoFlag,
			&ServicePortaudio_,
			this); // Bug on linux: Try replacing this with some ad-hoc struct to see if problem persists?
		if(err != paNoError) throw std::runtime_error(std::string("Failed to open a stream to default playback device: ") + Pa_GetErrorText(err));

		err = Pa_StartStream(pStream_);
		if(err != paNoError) throw std::runtime_error(std::string("Failed to start stream to default playback device: ") + Pa_GetErrorText(err));

		buffer_.resize((std::uint64_t)(framesPerBuffer_ * (std::uint32_t)nrOfChannels), 0.0f);
	}

	AudioEngine_Portaudio::~AudioEngine_Portaudio()
	{
		auto err = Pa_StopStream(pStream_);
		if(err != paNoError) std::cerr << std::string("Error stopping stream to playback device: ") + Pa_GetErrorText(err) << std::endl;
		err = Pa_CloseStream(pStream_);
		if(err != paNoError) std::cerr << std::string("Error closing stream to playback device: ") + Pa_GetErrorText(err) << std::endl;
		err = Pa_Terminate();
		if(err != paNoError) std::cerr << std::string("Error shutting down PortAudio: ") + Pa_GetErrorText(err) << std::endl;
	}

	SoundHandle AudioEngine_Portaudio::MakeSound(const std::vector<float>& data, const std::uint64_t nrOfChannels, const bool interleaved)
	{
		// TODO: take into consideration sample rate: resample source data to internal sample rate?
		// TODO: create multichannel sound classes for handling more than 2 channels

		if (nrOfChannels > 1)
		{
			if (nrOfChannels == 2)
			{
				if (data.size() % 2 != 0) throw std::runtime_error("Interleaved audio signal's size is not even.");

				if (interleaved)
				{
					soundsStereo_.push_back(std::vector<float>(data.begin(), data.begin() + data.size()));
					return SoundHandle::INVALID_ID - (soundsStereo_.size() - 1); // Stereo handles are stored at the end of the handles range to differenciate them from mono sounds.
				}
				else
				{
					const std::vector<float> signalLeft = std::vector<float>(data.begin(), data.begin() + data.size() / 2);
					const std::vector<float> signalRight = std::vector<float>(data.begin() + data.size() / 2, data.end());
					soundsStereo_.push_back(StereoSound_Portaudio(signalLeft, signalRight));
					return SoundHandle::INVALID_ID - (soundsStereo_.size() - 1); // Stereo handles are stored at the end of the handles range to differenciate them from mono sounds.
				}
			}
			else
			{
				throw std::runtime_error("Trying to create sound with an unexpected number of channels.");
			}

			// Old code that was just converting any multichannel audio to mono.
			// if (interleaved)
			// {
			// 	// Discards one channel entirely.
			// 	std::vector<float> signal = data;
			// 	UninterleaveSignal(signal, nrOfChannels);
			// 	soundsMono_.push_back(std::vector<float>(signal.begin(), signal.begin() + data.size() / nrOfChannels));
			// 	return soundsMono_.size() - 1;
			// }
			// else
			// {
			// 	soundsMono_.push_back(std::vector<float>(data.begin(), data.begin() + data.size() / nrOfChannels));
			// 	return soundsMono_.size() - 1;
			// }
		}

		soundsMono_.push_back(std::vector<float>(data.begin(), data.end()));
		return soundsMono_.size() - 1;
	}

	void AudioEngine_Portaudio::PlaySound(const SoundHandle sound)
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			soundsMono_[sound].Play();
		}
		else
		{
			soundsStereo_[SoundHandle::INVALID_ID - sound].Play();
		}
		playing_.insert(sound);
	}

	void AudioEngine_Portaudio::PlayOneShot(const SoundHandle sound)
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			soundsMono_[sound].Play();
			soundsMono_[sound].loop = false;
		}
		else
		{
			soundsStereo_[SoundHandle::INVALID_ID - sound].Play();
			soundsStereo_[SoundHandle::INVALID_ID - sound].loop = false;
		}
		playing_.insert(sound);
	}

	void AudioEngine_Portaudio::StopSound(const SoundHandle sound)
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			soundsMono_[sound].Stop();
		}
		else
		{
			soundsStereo_[SoundHandle::INVALID_ID - sound].Stop();
		}
		playing_.erase(sound);
	}

	void AudioEngine_Portaudio::StopAll()
	{
		for(auto& sound : soundsMono_)
		{
			sound.Stop();
		}
		for (auto& sound : soundsStereo_)
		{
			sound.Stop();
		}
		playing_.clear();
	}

	void AudioEngine_Portaudio::PauseSound(const SoundHandle sound)
	{
		playing_.erase(sound);
	}

	void AudioEngine_Portaudio::UnpauseSound(const SoundHandle sound)
	{
		playing_.insert(sound);
	}

	void AudioEngine_Portaudio::SetSoundLooping(const SoundHandle sound, const bool newLooping)
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			soundsMono_[sound].loop = newLooping;
		}
		else
		{
			soundsStereo_[SoundHandle::INVALID_ID - sound].loop = newLooping;
		}
	}

	bool AudioEngine_Portaudio::IsSoundPlaying(const SoundHandle sound) const
	{
		// Sound is considered to be playing if it's currentBegin index is not pointing to END_OF_DATA.

		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			return soundsMono_[sound].currentBegin != END_OF_DATA;
		}
		else
		{
			return soundsStereo_[SoundHandle::INVALID_ID - sound].currentBegin != END_OF_DATA;
		}
	}

	bool AudioEngine_Portaudio::AudioEngine_Portaudio::IsSoundPaused(const SoundHandle sound) const
	{
		// Sound is considered to be paused if it's currently "playing" but not is not part of the playing_ set .
		return IsSoundPlaying(sound) && (playing_.find(sound) == playing_.end());
	}

	bool AudioEngine_Portaudio::IsSoundLooped(const SoundHandle sound) const
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			return soundsMono_[sound].loop;
		}
		else
		{
			return soundsStereo_[SoundHandle::INVALID_ID - sound].loop;
		}
	}

	Bitdepth AudioEngine_Portaudio::GetBitdepth() const
	{
		return ToAbstractEnum_Portaudio(quant_);
	}

	Samplerate AudioEngine_Portaudio::GetSamplerate() const
	{
		return ToAbstractEnum_Portaudio(sampleRate_);
	}

	std::uint64_t AudioEngine_Portaudio::GetBytesPerFrame() const
	{
		switch(quant_)
		{
			case Scout::Bitdepth_Portaudio::F32: return 4 * GetNrOfChannels();

			default:
			{
				throw std::runtime_error("Cannot compute nr of bytes per frame due to inability to deduce size indicated by quant_ .");
			}break;
		}
	}

	std::uint64_t AudioEngine_Portaudio::GetNrOfChannels() const
	{
		switch(speakerSetup_)
		{
			case SpeakerSetup_Portaudio::MONO: return 1;
			case SpeakerSetup_Portaudio::DUAL_MONO: return 2;
			case SpeakerSetup_Portaudio::STEREO: return 2;

			default:
			{
				throw std::runtime_error("Unexpected speakerSetup_ .");
			}break;
		}
	}

	std::uint64_t AudioEngine_Portaudio::GetFramesPerBuffer() const
	{
		return framesPerBuffer_;
	}

	std::uint64_t AudioEngine_Portaudio::GetBufferSizeInBytes() const
	{
		return framesPerBuffer_ * GetBytesPerFrame();
	}

	std::chrono::milliseconds AudioEngine_Portaudio::GetBufferLatency() const
	{
		std::uint64_t latencyInSeconds = 0;
		const auto* info = Pa_GetStreamInfo(pStream_);
		if (info)
		{
			latencyInSeconds = (std::uint64_t)info->outputLatency;
		}
		else
		{
			throw std::runtime_error("AudioEngine_Portaudio::GetBufferLatency: Failed to retireve portaudio output latency.");
		}
		return std::chrono::milliseconds(latencyInSeconds * 1000);
	}

	void AudioEngine_Portaudio::Update()
	{
		std::scoped_lock lck(bufferMutex_);
		if (!update_) return;

		static std::vector<float> outputBuff(GetBufferSizeInBytes() / sizeof(float), 0.0f);

		switch(speakerSetup_)
		{
			case Scout::SpeakerSetup_Portaudio::MONO: // Note: might have broken it when code stereo support.
			{
				std::fill(outputBuff.begin(), outputBuff.end(), 0.0f);
				const std::uint32_t framesPerBuff = (std::uint32_t)GetFramesPerBuffer();

				static std::vector<float> sumBuff(framesPerBuff, 0.0f);
				static std::vector<float> workingBuff(framesPerBuff, 0.0f);
				std::fill(sumBuff.begin(), sumBuff.end(), 0.0f);
				std::fill(workingBuff.begin(), workingBuff.end(), 0.0f);

				for(auto& handle : playing_)
				{
					soundsMono_[handle].Service(workingBuff);
					soundsMono_[handle].AdvanceBy(framesPerBuff);
					MixSignalsInPlace(sumBuff, workingBuff, mixingPolicy_);
					if (soundsMono_[handle].currentBegin == END_OF_DATA) StopSound(handle);
				}

				assert(outputBuff.size() == sumBuff.size());
				std::copy(sumBuff.begin(), sumBuff.end(), outputBuff.begin());
			}break;

			case Scout::SpeakerSetup_Portaudio::DUAL_MONO: // Note: might have broken it when code stereo support.
			{
				std::fill(outputBuff.begin(), outputBuff.end(), 0.0f);
				const std::uint32_t framesPerBuff = (std::uint32_t)GetFramesPerBuffer();

				static std::vector<float> sumBuff(framesPerBuff, 0.0f);
				static std::vector<float> workingBuff(framesPerBuff, 0.0f);
				std::fill(sumBuff.begin(), sumBuff.end(), 0.0f);
				std::fill(workingBuff.begin(), workingBuff.end(), 0.0f);

				for(auto& handle : playing_)
				{
					soundsMono_[handle].Service(workingBuff);
					soundsMono_[handle].AdvanceBy(framesPerBuff);
					MixSignalsInPlace(sumBuff, workingBuff, mixingPolicy_);
					if (soundsMono_[handle].currentBegin == END_OF_DATA) StopSound(handle);
				}

				assert(outputBuff.size() == 2 * sumBuff.size());
				std::copy(sumBuff.begin(), sumBuff.end(), outputBuff.begin());
				std::copy(sumBuff.begin(), sumBuff.end(), outputBuff.begin() + sumBuff.size());
				InterleaveSignal(outputBuff, 2);
			}break;

			case Scout::SpeakerSetup_Portaudio::STEREO:
			{
				const std::uint32_t framesPerBuff = (std::uint32_t)GetFramesPerBuffer();

				{ // Mono
					std::fill(outputBuff.begin(), outputBuff.end(), 0.0f);
					static std::vector<float> sumBuffMono(framesPerBuff, 0.0f);
					static std::vector<float> workingBuffMono(framesPerBuff, 0.0f);
					std::fill(sumBuffMono.begin(), sumBuffMono.end(), 0.0f);
					std::fill(workingBuffMono.begin(), workingBuffMono.end(), 0.0f);

					for (auto& handle : playing_)
					{
						// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
						if (handle < SoundHandle::INVALID_ID / 2)
						{
							soundsMono_[handle].Service(workingBuffMono);
							soundsMono_[handle].AdvanceBy(framesPerBuff);
							MixSignalsInPlace(sumBuffMono, workingBuffMono, mixingPolicy_);
							if (soundsMono_[handle].currentBegin == END_OF_DATA) StopSound(handle);
						}
					}

					if (outputBuff.size() != sumBuffMono.size() * 2) throw std::runtime_error("Output buffer size doesn't match the size of the sum buffer.");
					std::copy(sumBuffMono.begin(), sumBuffMono.end(), outputBuff.begin());
					std::copy(sumBuffMono.begin(), sumBuffMono.end(), outputBuff.begin() + sumBuffMono.size());
					InterleaveSignal(outputBuff, 2);
					std::copy(outputBuff.begin(), outputBuff.end(), buffer_.begin());
				}

				{ // Stereo
					std::fill(outputBuff.begin(), outputBuff.end(), 0.0f);
					static std::vector<float> sumBuffStereo(framesPerBuff * 2, 0.0f);
					static std::vector<float> workingBuffStereo(framesPerBuff * 2, 0.0f);
					std::fill(sumBuffStereo.begin(), sumBuffStereo.end(), 0.0f);
					std::fill(workingBuffStereo.begin(), workingBuffStereo.end(), 0.0f);

					const auto playingCpy = playing_; // Needed to be able to StopSound() inside the for loop. We're changing the container we're interating on inside.
					for (auto& handle : playingCpy)
					{
						// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
						if (handle > SoundHandle::INVALID_ID / 2)
						{
							soundsStereo_[SoundHandle::INVALID_ID - handle].Service(workingBuffStereo);
							soundsStereo_[SoundHandle::INVALID_ID - handle].AdvanceBy(framesPerBuff);
							MixSignalsInPlace(sumBuffStereo, workingBuffStereo, mixingPolicy_);
							if (soundsStereo_[SoundHandle::INVALID_ID - handle].currentBegin == END_OF_DATA) StopSound(handle);
						}
					}

					if (outputBuff.size() != sumBuffStereo.size()) throw std::runtime_error("Output buffer size doesn't match the size of the sum buffer.");
					std::copy(sumBuffStereo.begin(), sumBuffStereo.end(), outputBuff.begin());
				}
			}break;

			default:
			{
				throw std::runtime_error("Specified speaker configuration isn't handled by this implementation.");
			}break;
		}

		// Mix mono sounds and stereo ones.
		MixSignalsInPlace(buffer_, outputBuff, mixingPolicy_); // Note: might have broken this for MONO and DUAL_MONO when code stereo support. Used to just copy the outputBuff to buffer_.
		// NormalizeSignal(buffer_);

		// Audio display effects. Note: should this maybe be nr of channels dependant?...
		for (size_t i = 0; i < displayEffects_.size(); i++)
		{
			displayEffects_[i](buffer_);
		}

		update_ = false;
	}

	SfxHandle AudioEngine_Portaudio::RegisterEffectForSound(const SoundSpecificEffectCallback fxCallback, const SoundHandle sound)
	{
		throw std::runtime_error("Implement this.");

		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound.id < SoundHandle::INVALID_ID / 2)
		{
			return SfxHandle::INVALID_ID;
		}
		else
		{
			return SfxHandle::INVALID_ID;
		}
	}

	SfxHandle AudioEngine_Portaudio::RegisterEffectForDisplay(const AudioDisplayEffectCallback fxCallback)
	{
		// TODO: ensure that the fx passed matches the selected speakers layout.

		displayEffects_.push_back(fxCallback);
		return displayEffects_.size() - 1;
	}

	int AudioEngine_Portaudio::ServicePortaudio_(const void* input, void* output,
								 unsigned long frameCount,
								 const PaStreamCallbackTimeInfo* timeInfo,
								 PaStreamCallbackFlags statusFlags,
								 void* userData)
	{
		AudioEngine_Portaudio& self = *static_cast<AudioEngine_Portaudio*>(userData);
		std::scoped_lock lck(self.bufferMutex_);

		try
		{
			// BUG: trying to access self.buffer_ on linux results in a segmentation fault.
			std::memcpy(output, self.buffer_.data(), frameCount * self.GetNrOfChannels() * sizeof(float));
		}
		catch(const std::exception& e)
		{
			throw std::runtime_error(std::string("AudioEngine_Portaudio::ServicePortaudio_: Encountered error servicing audio: ") + e.what());
		}
		
		self.update_ = true;
		return paContinue;
	}

	void AudioEngine_Portaudio::SetSoundLooped(const SoundHandle sound, const bool newVal)
	{
		// Ids at the start of the integers are for mono, ids at the end of the integers range are for stereo.
		if (sound < SoundHandle::INVALID_ID / 2)
		{
			soundsMono_[sound].loop = newVal;
		}
		else
		{
			soundsStereo_[SoundHandle::INVALID_ID - sound].loop = newVal;
		}
	}
}
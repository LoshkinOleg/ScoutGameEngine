#include <Scout/Portaudio/Sound.h>
#include <Scout/Math.h>

#include <cstring>

namespace Scout
{
	// MonoSound_Portaudio

	MonoSound_Portaudio::MonoSound_Portaudio(const std::vector<float>& data):
		data(data)
	{}
	void MonoSound_Portaudio::AdvanceBy(const std::uint32_t frames)
	{
		if(currentBeginFrame == END_OF_DATA) return;

		const std::uint32_t max = (std::uint32_t)data.size();
		const std::uint32_t nextBegin = currentBeginFrame + frames;

		if (loop)
		{
			if (nextBegin < max) // if not wrapping around audio data
			{
				currentBeginFrame = nextBegin;
			}
			else
			{
				currentBeginFrame = nextBegin - max;
			}
		}
		else
		{
			if (nextBegin <= max) // if not wrapping around audio data
			{
				currentBeginFrame = nextBegin;
			}
			else
			{
				currentBeginFrame = END_OF_DATA;
			}
		}
	}
	void MonoSound_Portaudio::GoToFrame(const std::uint32_t frame)
	{
		if(frame >= data.size()) throw std::runtime_error("Trying to go to a frame beyond sound data.");

		currentBeginFrame = frame;
	}
	void MonoSound_Portaudio::Service(std::vector<float>& outBuff) const
	{
		const auto buffSize = (std::uint32_t)outBuff.size();
		const auto max = (std::uint32_t)data.size();
		
		std::uint32_t currentEnd = currentBeginFrame + buffSize;
		if (currentEnd >= max) // if wrapping around audio data
		{
			currentEnd = currentEnd - max;
		}

		if (currentEnd > currentBeginFrame) // not wrapping around buffer of audio data
		{
			std::copy(data.begin() + currentBeginFrame, data.begin() + currentEnd, outBuff.begin());
		}
		else
		{
			const auto firstPartDelta = max - currentBeginFrame;
			const auto secondPartDelta = currentEnd;
			std::copy(data.begin() + currentBeginFrame, data.end(), outBuff.begin());
			std::copy(data.begin(), data.begin() + secondPartDelta, outBuff.begin() + firstPartDelta);
		}

		for (size_t i = 0; i < fx.size(); i++)
		{
			fx[i](outBuff);
		}
	}

	void MonoSound_Portaudio::Play()
	{
		currentBeginFrame = 0;
	}
	void MonoSound_Portaudio::Stop()
	{
		currentBeginFrame = END_OF_DATA;
	}

	// StereoSound_Portaudio

	StereoSound_Portaudio::StereoSound_Portaudio(const std::vector<float>& dataInterleaved) :
		dataInterleaved(dataInterleaved)
	{}

	StereoSound_Portaudio::StereoSound_Portaudio(const std::vector<float>& dataLeft, const std::vector<float>& dataRight)
	{
		if (dataLeft.size() != dataRight.size()) throw std::runtime_error("Specifying two signals for stereo of differing sizes.");
		dataInterleaved.resize(dataLeft.size() * 2);
		std::copy(dataLeft.begin(), dataLeft.end(), dataInterleaved.begin());
		std::copy(dataRight.begin(), dataRight.end(), dataInterleaved.begin() + dataInterleaved.size() / 2);
		InterleaveSignal(dataInterleaved, 2);
	}

	void StereoSound_Portaudio::AdvanceBy(const std::uint32_t frames)
	{
		// Working in frames here.

		if (currentBeginFrame == END_OF_DATA) return;

		if ((std::uint32_t)dataInterleaved.size() % 2 != 0) throw std::runtime_error("Interleaved data's size is not even.");
		const std::uint32_t max = (std::uint32_t)dataInterleaved.size() / 2;
		const std::uint32_t nextBegin = currentBeginFrame + frames;

		if (loop)
		{
			if (nextBegin < max) // if not wrapping around audio data
			{
				currentBeginFrame = nextBegin;
			}
			else
			{
				currentBeginFrame = (nextBegin - max);
			}
		}
		else
		{
			if (nextBegin <= max) // if not wrapping around audio data
			{
				currentBeginFrame = nextBegin;
			}
			else
			{
				currentBeginFrame = END_OF_DATA;
			}
		}
	}
	void StereoSound_Portaudio::GoToFrame(const std::uint32_t frame)
	{
		if ((std::uint32_t)dataInterleaved.size() % 2 != 0) throw std::runtime_error("Interleaved data's size is not even.");
		if (frame >= dataInterleaved.size() / 2) throw std::runtime_error("Trying to go to a frame beyond sound data.");

		currentBeginFrame = frame;
	}
	void StereoSound_Portaudio::Service(std::vector<float>& outBuff) const
	{
		// Working in frames here.

		const auto buffSize = (std::uint32_t)outBuff.size();
		const auto max = (std::uint32_t)dataInterleaved.size() / 2; // div 2 to remain in stereo frames

		std::uint32_t currentEnd = currentBeginFrame + buffSize / 2; // div 2 since outBuff is sized for 2 channels and we want to remain in frames instead of samples.
		if (currentEnd >= max) // if wrapping around audio data
		{
			currentEnd = currentEnd - max;
		}

		if (currentEnd > currentBeginFrame) // not wrapping around buffer of audio data
		{
			std::copy(dataInterleaved.begin() + currentBeginFrame * 2, dataInterleaved.begin() + currentEnd * 2, outBuff.begin()); // *2 since we're working with stereo frames
		}
		else
		{
			const auto firstPartDelta = max - currentBeginFrame;
			const auto secondPartDelta = currentEnd;
			std::copy(dataInterleaved.begin() + currentBeginFrame * 2, dataInterleaved.end(), outBuff.begin()); // *2 since we're working with stereo frames
			std::copy(dataInterleaved.begin(), dataInterleaved.begin() + secondPartDelta * 2, outBuff.begin() + firstPartDelta * 2);
		}

		for (size_t i = 0; i < fx.size(); i++)
		{
			fx[i](outBuff);
		}
	}

	void StereoSound_Portaudio::Play()
	{
		currentBeginFrame = 0;
	}
	void StereoSound_Portaudio::Stop()
	{
		currentBeginFrame = END_OF_DATA;
	}

	// MultichannelSound_Portaudio

	MultichannelSound_Portaudio::MultichannelSound_Portaudio(const std::vector<float>& dataInterleaved, const size_t nrOfChannels, const size_t targetNrOfChannels, const size_t engineBufferSizeInSamples)
		:dataInterleaved(dataInterleaved), nrOfChannels(nrOfChannels)
	{
		if (dataInterleaved.size() < engineBufferSizeInSamples)
		{
			// Audio data is less than a single buffer of audio. Pad the audio data with 0's.
			this->dataInterleaved.resize(engineBufferSizeInSamples);
			std::fill(this->dataInterleaved.begin(), this->dataInterleaved.end(), 0.0f);
			std::copy(dataInterleaved.begin(), dataInterleaved.end(), this->dataInterleaved.begin());
		}
		if (nrOfChannels != targetNrOfChannels)
		{
			ChangeNrOfChannelsInSignal(this->dataInterleaved, nrOfChannels, targetNrOfChannels, true);
			this->nrOfChannels = targetNrOfChannels;
		}
	}

	void MultichannelSound_Portaudio::AdvanceBy(const std::uint32_t frames)
	{
		if (currentBeginFrame == END_OF_DATA) return;

		if ((std::uint32_t)dataInterleaved.size() % nrOfChannels != 0) throw std::runtime_error("Interleaved data's size is not coherent with the number of channels.");
		const std::uint32_t maxFrame = (std::uint32_t)dataInterleaved.size() / nrOfChannels;
		const std::uint32_t nextBeginFrame = currentBeginFrame + frames;

		if (loop)
		{
			if (nextBeginFrame < maxFrame) // if not wrapping around audio data
			{
				currentBeginFrame = nextBeginFrame;
			}
			else
			{
				currentBeginFrame = (nextBeginFrame - maxFrame);
			}
		}
		else
		{
			if (nextBeginFrame <= maxFrame) // if not wrapping around audio data
			{
				currentBeginFrame = nextBeginFrame;
			}
			else
			{
				currentBeginFrame = END_OF_DATA;
			}
		}
	}

	void MultichannelSound_Portaudio::GoToFrame(const std::uint32_t frame)
	{
		if ((std::uint32_t)dataInterleaved.size() % nrOfChannels != 0) throw std::runtime_error("Interleaved data's size is not coherent with the number of channels.");
		if (frame >= dataInterleaved.size() / nrOfChannels) throw std::runtime_error("Trying to go to a frame beyond sound data.");

		currentBeginFrame = frame;
	}

	void MultichannelSound_Portaudio::Service(std::vector<float>& outBuff) const
	{
		const auto buffSize = (std::uint32_t)outBuff.size();
		const auto maxFrame = (std::uint32_t)dataInterleaved.size() / nrOfChannels;

		std::uint32_t currentEndFrame = currentBeginFrame + buffSize / nrOfChannels;
		if (currentEndFrame >= maxFrame) // if wrapping around audio data
		{
			currentEndFrame = currentEndFrame - maxFrame;
		}

		if (currentEndFrame > currentBeginFrame) // not wrapping around buffer of audio data
		{
			std::copy(dataInterleaved.begin() + currentBeginFrame * nrOfChannels, dataInterleaved.begin() + currentEndFrame * nrOfChannels, outBuff.begin());
		}
		else
		{
			const auto firstPartDelta = maxFrame - currentBeginFrame;
			const auto secondPartDelta = currentEndFrame;
			std::copy(dataInterleaved.begin() + currentBeginFrame * nrOfChannels, dataInterleaved.end(), outBuff.begin());
			std::copy(dataInterleaved.begin(), dataInterleaved.begin() + secondPartDelta * nrOfChannels, outBuff.begin() + firstPartDelta * nrOfChannels);
		}

		for (size_t i = 0; i < fx.size(); i++)
		{
			fx[i](outBuff);
		}
	}

	void MultichannelSound_Portaudio::Play()
	{
		currentBeginFrame = 0;
	}

	void MultichannelSound_Portaudio::Stop()
	{
		currentBeginFrame = END_OF_DATA;
	}
}
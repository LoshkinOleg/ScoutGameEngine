#include <Scout/Portaudio/Sound.h>
#include <Scout/Math.h>

#include <cstring>

namespace Scout
{
	MonoSound_Portaudio::MonoSound_Portaudio(const std::vector<float>& data):
		data(data)
	{}
	void MonoSound_Portaudio::AdvanceBy(const std::uint32_t frames)
	{
		if(currentBegin == END_OF_DATA) return;

		const std::uint32_t max = (std::uint32_t)data.size();
		const std::uint32_t nextBegin = currentBegin + frames;

		if (loop)
		{
			if (nextBegin < max) // if not wrapping around audio data
			{
				currentBegin = nextBegin;
			}
			else
			{
				currentBegin = nextBegin - max;
			}
		}
		else
		{
			if (nextBegin <= max) // if not wrapping around audio data
			{
				currentBegin = nextBegin;
			}
			else
			{
				currentBegin = END_OF_DATA;
			}
		}
	}
	void MonoSound_Portaudio::GoToFrame(const std::uint32_t frame)
	{
		if(frame >= data.size()) throw std::runtime_error("Trying to go to a frame beyond sound data.");

		currentBegin = frame;
	}
	void MonoSound_Portaudio::Service(std::vector<float>& outBuff) const
	{
		const auto buffSize = (std::uint32_t)outBuff.size();
		const auto max = (std::uint32_t)data.size();
		
		std::uint32_t currentEnd = currentBegin + buffSize;
		if (currentEnd >= max) // if wrapping around audio data
		{
			currentEnd = currentEnd - max;
		}

		if (currentEnd > currentBegin) // not wrapping around buffer of audio data
		{
			std::copy(data.begin() + currentBegin, data.begin() + currentEnd, outBuff.begin());
		}
		else
		{
			const auto firstPartDelta = max - currentBegin;
			const auto secondPartDelta = currentEnd;
			std::copy(data.begin() + currentBegin, data.end(), outBuff.begin());
			std::copy(data.begin(), data.begin() + secondPartDelta, outBuff.begin() + firstPartDelta);
		}
	}

	void MonoSound_Portaudio::Play()
	{
		currentBegin = 0;
	}
	void MonoSound_Portaudio::Stop()
	{
		currentBegin = END_OF_DATA;
	}



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

		if (currentBegin == END_OF_DATA) return;

		if ((std::uint32_t)dataInterleaved.size() % 2 != 0) throw std::runtime_error("Interleaved data's size is not even.");
		const std::uint32_t max = (std::uint32_t)dataInterleaved.size() / 2;
		const std::uint32_t nextBegin = currentBegin + frames;

		if (loop)
		{
			if (nextBegin < max) // if not wrapping around audio data
			{
				currentBegin = nextBegin;
			}
			else
			{
				currentBegin = (nextBegin - max);
			}
		}
		else
		{
			if (nextBegin <= max) // if not wrapping around audio data
			{
				currentBegin = nextBegin;
			}
			else
			{
				currentBegin = END_OF_DATA;
			}
		}
	}
	void StereoSound_Portaudio::GoToFrame(const std::uint32_t frame)
	{
		if ((std::uint32_t)dataInterleaved.size() % 2 != 0) throw std::runtime_error("Interleaved data's size is not even.");
		if (frame >= dataInterleaved.size() / 2) throw std::runtime_error("Trying to go to a frame beyond sound data.");

		currentBegin = frame;
	}
	void StereoSound_Portaudio::Service(std::vector<float>& outBuff) const
	{
		// Working in frames here.

		const auto buffSize = (std::uint32_t)outBuff.size();
		const auto max = (std::uint32_t)dataInterleaved.size() / 2; // div 2 to remain in stereo frames

		std::uint32_t currentEnd = currentBegin + buffSize / 2; // div 2 since outBuff is sized for 2 channels and we want to remain in frames instead of samples.
		if (currentEnd >= max) // if wrapping around audio data
		{
			currentEnd = currentEnd - max;
		}

		if (currentEnd > currentBegin) // not wrapping around buffer of audio data
		{
			std::copy(dataInterleaved.begin() + currentBegin * 2, dataInterleaved.begin() + currentEnd * 2, outBuff.begin()); // *2 since we're working with stereo frames
		}
		else
		{
			const auto firstPartDelta = max - currentBegin;
			const auto secondPartDelta = currentEnd;
			std::copy(dataInterleaved.begin() + currentBegin * 2, dataInterleaved.end(), outBuff.begin()); // *2 since we're working with stereo frames
			std::copy(dataInterleaved.begin(), dataInterleaved.begin() + secondPartDelta * 2, outBuff.begin() + firstPartDelta * 2);
		}
	}

	void StereoSound_Portaudio::Play()
	{
		currentBegin = 0;
	}
	void StereoSound_Portaudio::Stop()
	{
		currentBegin = END_OF_DATA;
	}
}
#pragma once

#include <vector>

#include <Scout/Portaudio/TypedefAndEnum.h>

namespace Scout
{
	struct MonoSound_Portaudio
	{
		MonoSound_Portaudio(const std::vector<float>& data);

		void AdvanceBy(const std::uint32_t frames);
		void GoToFrame(const std::uint32_t frame);

		void Service(std::vector<float>& outBuff) const;

		void Play();
		void Stop();

		std::vector<float> data{};
		std::vector<SoundSpecificEffectCallback> fx_{};
		std::uint32_t currentBegin = END_OF_DATA; // index of first current frame
		bool loop = false;
	};

	struct StereoSound_Portaudio
	{
		StereoSound_Portaudio(const std::vector<float>& dataLeft, const std::vector<float>& dataRight);
		StereoSound_Portaudio(const std::vector<float>& dataInterleaved);

		void AdvanceBy(const std::uint32_t frames);
		void GoToFrame(const std::uint32_t frame);

		void Service(std::vector<float>& outBuff) const;

		void Play();
		void Stop();

		std::vector<float> dataInterleaved{};
		std::vector<SoundSpecificEffectCallback> fx_{};
		std::uint32_t currentBegin = END_OF_DATA; // index of first current FRAME (not sample!)
		bool loop = false;
	};
}
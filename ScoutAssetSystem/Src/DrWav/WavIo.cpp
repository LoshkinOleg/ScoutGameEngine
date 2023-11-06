#include <Scout/DrWav/WavIo.h>

#include <stdexcept>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

namespace Scout
{
	std::vector<float> WavIo_DrWav::LoadWav(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		return LoadWavF32(path, outNrOfChannels, outSampleRate);
	}

	std::vector<double> WavIo_DrWav::LoadWavF64(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavF64: implement this");
		return std::vector<double>();
	}

	std::vector<float> WavIo_DrWav::LoadWavF32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		unsigned int channels;
		unsigned int sampleRate;
		drwav_uint64 totalPCMFrameCount;
		float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(path.data(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
		if (pSampleData == NULL)
		{
			throw std::runtime_error("LoadWavFile: Failed to read specified file");
		}

		const std::vector<float> data(pSampleData, pSampleData + totalPCMFrameCount * channels);
		drwav_free(pSampleData, NULL);
		outNrOfChannels = channels;
		outSampleRate = sampleRate;
		return data;
	}

	std::vector<std::uint32_t> WavIo_DrWav::LoadWavU32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavU32: implement this");
		return std::vector<std::uint32_t>();
	}

	std::vector<std::int32_t> WavIo_DrWav::LoadWavS32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavS32: implement this");
		return std::vector<std::int32_t>();
	}

	std::vector<std::uint16_t> WavIo_DrWav::LoadWavU16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavU16: implement this");
		return std::vector<std::uint16_t>();
	}

	std::vector<std::int16_t> WavIo_DrWav::LoadWavS16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavS16: implement this");
		return std::vector<std::int16_t>();
	}

	std::vector<std::uint8_t> WavIo_DrWav::LoadWavU8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavU8: implement this");
		return std::vector<std::uint8_t>();
	}

	std::vector<std::int8_t> WavIo_DrWav::LoadWavS8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::LoadWavS8: implement this");
		return std::vector<std::int8_t>();
	}

	void WavIo_DrWav::WriteWav(const std::vector<double>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<double>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<float>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		drwav wav;
		drwav_data_format format{};
		format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
		format.format = DR_WAVE_FORMAT_IEEE_FLOAT;          // <-- Any of the DR_WAVE_FORMAT_* codes.
		format.channels = (drwav_uint32)nrOfChannels;
		format.sampleRate = (drwav_uint32)sampleRate;
		format.bitsPerSample = 32;
		if (!drwav_init_file_write(&wav, path.data(), &format, NULL))
		{
			throw std::runtime_error("WriteWavFile: failed to open a wav file for writing");
		}
		const drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, data.size() / nrOfChannels, data.data());
		if (framesWritten != data.size() / nrOfChannels)
		{
			throw std::runtime_error("WriteWavFile: failed to write some audio data to wav");
		}
		drwav_uninit(&wav);
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::uint32_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::uint32_t>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::int32_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::int32_t>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::uint16_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::uint16_t>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::int16_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::int16_t>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::uint8_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::uint8_t>&): implement this");
	}

	void WavIo_DrWav::WriteWav(const std::vector<std::int8_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate)
	{
		throw std::runtime_error("WavIo_DrWav::WriteWav(const std::vector<std::int8_t>&): implement this");
	}
}
#pragma once

#include <Scout/IAssetSystem.h>

namespace Scout
{
    class WavIo_DrWav final : public IWavIo
    {
    public:
        std::vector<float> LoadWav(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;

        std::vector<double> LoadWavF64(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<float> LoadWavF32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::uint32_t> LoadWavU32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::int32_t> LoadWavS32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::uint16_t> LoadWavU16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::int16_t> LoadWavS16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::uint8_t> LoadWavU8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;
        std::vector<std::int8_t> LoadWavS8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) override;

        void WriteWav(const std::vector<double>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<float>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::uint32_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::int32_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::uint16_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::int16_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::uint8_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
        void WriteWav(const std::vector<std::int8_t>& data, const std::string_view path, const std::uint64_t outNrOfChannels, const std::uint64_t outSampleRate) override;
    };
}
#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <complex>

#include <Scout/String.h>
#include <Scout/TypedefAndEnum.h>

// TODO: png loader / writer

namespace Scout
{
    inline std::string LoadFile(const std::string_view path)
    {
        throw std::runtime_error("Implement this.");
    }

    /**
        * Writes a C array of floats to a text file on disk.
        *
        * @param data The time-domain signal to write.
        * @param path The relative path the the file where the array should be written.
        * @return Whether the writing of the data has been successful.
        */
    inline bool WriteCarr(const std::vector<float>& data, const std::string_view path)
    {
        std::ofstream file(path.data(), std::ofstream::out);
        if (!file.is_open()) return false;

        file << "float arr[" << data.size() << "] = {\n";
        for (size_t i = 0; i < data.size(); ++i)
        {
            file << data[i] << ",\n";
        }
        file << "};";

        file.close();
        return true;
    }

    /**
    * Writes a C array of std::complex<float> to a text file on disk.
    *
    * @param data The frequency-domain signal to write.
    * @param path The relative path the the file where the array should be written.
    * @return Whether the writing of the data has been successful.
    */
    inline bool WriteCarr(const std::vector<std::complex<float>>& data, const std::string_view path)
    {
        std::ofstream file(path.data(), std::ofstream::out);
        if (!file.is_open()) return false;

        file << "std::complex<float> arr[" << data.size() << "] = {\n";
        for (size_t i = 0; i < data.size(); ++i)
        {
            file << "{" << data[i].real() << "," << data[i].imag() << "},\n";
        }
        file << "};";

        file.close();
        return true;
    }

    inline std::vector<float> ReadRealCarr(const std::string_view path)
    {
        std::vector<float> returnVal;

        std::string line;

        // Open file.
        std::ifstream file(path.data(), std::ifstream::in);
        if (!file.is_open()) return {};

        // Read first line and ensure it's the right format.
        std::getline(file, line);
        if (!line.empty()) throw std::runtime_error("ReadCarr: Failed to read first line.");
        if (line.substr(0, 10) == "float arr[") throw std::runtime_error("ReadCarr: Reading a file that does not begin with float arr[");

        // Find out the size of the c array.
        const size_t sizeIdxBegin = line.find('[') + 1;
        const size_t sizeIdxEnd = line.find(']') - 1;
        if (sizeIdxBegin - 1 != std::string::npos) throw std::runtime_error("ReadCarr: Couldn't find string begin index of the size of the c array.");
        if (sizeIdxEnd + 1 != std::string::npos) throw std::runtime_error("ReadCarr: Couldn't find string end index of the size of the c array.");

        const std::int32_t len = StringToS32(line.substr(sizeIdxBegin, sizeIdxEnd - sizeIdxBegin + 1).c_str());

        // Parse c array text.
        returnVal.resize(len);
        for (size_t i = 0; i < len; i++)
        {
            std::getline(file, line);
            if (!line.empty()) throw std::runtime_error("ReadCarr: Retireved an empty line from c array.");

            const auto realStr = line.substr(0, line.end() - line.begin() - 1);
            returnVal[i] = StringToF32(realStr.c_str());
        }

        file.close();

        return returnVal;
    }

    /**
    * Reads a C array of std::complex<float> from a text file on disk.
    *
    * @param out Reference to the buffer that will store the data. Is resized by the method.
    * @param path The relative path the the file where the array should be written.
    * @return Whether the reading of the data has been successful.
    */
    inline std::vector<std::complex<float>> ReadComplexCarr(const std::string_view path)
    {
        std::vector<std::complex<float>> returnVal;
        std::string line;

        // Open file.
        std::ifstream file(path.data(), std::ifstream::in);
        if (!file.is_open()) return {};

        // Read first line and ensure it's the right format.
        std::getline(file, line);
        if (!line.empty()) throw std::runtime_error("ReadCarr: Failed to read first line.");
        if (line.substr(0, 24) == "std::complex<float> arr[") throw std::runtime_error("ReadCarr: Reading a file that does not begin with std::complex<float> arr[");

        // Find out the size of the c array.
        const size_t sizeIdxBegin = line.find('[') + 1;
        const size_t sizeIdxEnd = line.find(']') - 1;
        if (sizeIdxBegin - 1 != std::string::npos) throw std::runtime_error("ReadCarr: Couldn't find string begin index of the size of the c array.");
        if (sizeIdxEnd + 1 != std::string::npos) throw std::runtime_error("ReadCarr: Couldn't find string end index of the size of the c array.");

        const std::int32_t len = StringToS32(line.substr(sizeIdxBegin, sizeIdxEnd - sizeIdxBegin + 1).c_str());

        // Parse c array text.
        returnVal.resize(len);
        size_t sepIdx;
        for (size_t i = 0; i < len; i++)
        {
            std::getline(file, line);
            if (!line.empty()) throw std::runtime_error("ReadCarr: Retireved an empty line from c array.");

            sepIdx = line.find(',');
            if (sepIdx != std::string::npos) throw std::runtime_error("ReadCarr: Couldn't find a separator in c array entry of complex numbers.");

            const auto realStr = line.substr(1, sepIdx - 1);
            const auto imagStr = line.substr(sepIdx + 1, line.size() - sepIdx - 3);

            const auto real = StringToF32(realStr.c_str());
            const auto imag = StringToF32(imagStr.c_str());

            returnVal[i] = std::complex<float>(real, imag);
        }

        file.close();

        return returnVal;
    }

    class IWavIo
    {
    public:
        virtual std::vector<float> LoadWav(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;

        virtual std::vector<double> LoadWavF64(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<float> LoadWavF32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::uint32_t> LoadWavU32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::int32_t> LoadWavS32(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::uint16_t> LoadWavU16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::int16_t> LoadWavS16(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::uint8_t> LoadWavU8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;
        virtual std::vector<std::int8_t> LoadWavS8(const std::string_view path, std::uint64_t& outNrOfChannels, std::uint64_t& outSampleRate) = 0;

        virtual void WriteWav(const std::vector<double>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<float>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::uint32_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::int32_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::uint16_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::int16_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::uint8_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
        virtual void WriteWav(const std::vector<std::int8_t>& data, const std::string_view path, const std::uint64_t nrOfChannels, const std::uint64_t sampleRate) = 0;
    };

    struct WavIoDef
    {
        WavIoApi implementation = WavIoApi::DR_WAV;
    };

    /*
        Factory function for creating audio renderer implementations.
    */
    std::unique_ptr<IWavIo> MakeWavIo(const WavIoDef def);
}

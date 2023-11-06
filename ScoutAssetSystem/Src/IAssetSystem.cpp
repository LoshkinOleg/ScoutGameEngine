#include <Scout/IAssetSystem.h>

#include <Scout/DrWav/WavIo.h>

namespace Scout
{
	std::unique_ptr<IWavIo> MakeWavIo(const WavIoDef def)
	{
		switch (def.implementation)
		{
		case WavIoApi::DR_WAV:
		{
			return std::make_unique<WavIo_DrWav>();
		}break;

		default:
		{
			throw std::runtime_error("MakeWavIo: unexpected def.implementation");
		}break;
		}
	}
}

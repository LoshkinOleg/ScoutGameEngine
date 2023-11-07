#include <cassert>

#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>
#include <Scout/IAudioEngine.h>
#include <Scout/IAssetSystem.h>
#include <Scout/Math.h>

// TODO: figure out how to clamp signal to -1;1 without introducing wierd artefacts.

int main()
{
	bool shutdown = false;

	Scout::WindowDef windowDef;
	windowDef.implementation = Scout::InputApi::SDL;
	windowDef.windowName = "MyWindow";
	windowDef.windowWidth = 720;
	windowDef.windowHeight = 720;
	auto window = Scout::MakeWindow(windowDef);

	Scout::AudioEngineDef audioEngineDef;
	audioEngineDef.implementation = Scout::AudioApi::PORTAUDIO;
	audioEngineDef.speakersSetup = Scout::SpeakerSetup::STEREO;
	audioEngineDef.engineBufferSamplerate = Scout::Samplerate::Hz_48k;
	audioEngineDef.mixingPolicy = Scout::MixingPolicy::AVERAGE;
	auto audioEngine = Scout::MakeAudioEngine(audioEngineDef);

	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;
	auto audioDataMusic = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/MusicStereo_48kHz_32f.wav", nrOfChannels, sampleRate);
	auto audioDataSweep = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweep_48kHz_32f.wav", nrOfChannels, sampleRate);

	// for (size_t i = 0; i < audioDataSweep.size(); i++)
	// {
	// 	audioDataSweep[i] *= 0.01f;
	// }

	assert(audioDataMusic.size() > 0);
	const auto soundHandleMusic = audioEngine->MakeSound(audioDataMusic, 2, true);
	const auto soundHandleSweep = audioEngine->MakeSound(audioDataSweep, 1, false);
	audioEngine->SetSoundLooped(soundHandleMusic, true);
	audioEngine->SetSoundLooped(soundHandleSweep, true);
	audioEngine->PlaySound(soundHandleMusic);
	audioEngine->PlaySound(soundHandleSweep);

	auto fx = [](std::vector<float>& audioData)->void
	{
		static float theta = 0.0f;

		const size_t len = audioData.size();
		for (size_t i = 0; i < len; i++)
		{
			audioData[i] *= std::sinf(theta);
		}
		theta += 0.1f;
	};
	// audioEngine->RegisterEffectForDisplay(fx);


	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();
	}
	return 0;
}
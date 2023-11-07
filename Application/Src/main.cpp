#include <cassert>

#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>
#include <Scout/IAudioEngine.h>
#include <Scout/IAssetSystem.h>
#include <Scout/Math.h>
#include <Scout/AudioEffects.h>

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
	auto audioDataNoise = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/WhiteNoiseMonoTremolo_48kHz_32f.wav", nrOfChannels, sampleRate);

	assert(audioDataMusic.size() > 0);
	const auto soundHandleMusic = audioEngine->MakeSound(audioDataMusic, 2, true);
	const auto soundHandleSweep = audioEngine->MakeSound(audioDataSweep, 1, false);
	const auto soundHandleNoise = audioEngine->MakeSound(audioDataNoise, 1, false);
	audioEngine->SetSoundLooped(soundHandleMusic, true);
	audioEngine->SetSoundLooped(soundHandleSweep, true);
	audioEngine->SetSoundLooped(soundHandleNoise, true);
	audioEngine->PlaySound(soundHandleMusic);
	audioEngine->PlaySound(soundHandleSweep);
	audioEngine->PlaySound(soundHandleNoise);

	audioEngine->RegisterEffectForDisplay(Scout::MakeLimiterCallback(0.75f));


	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();
	}
	return 0;
}
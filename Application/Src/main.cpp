#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <complex>
#include <algorithm>
#include <numeric>

#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>
#include <Scout/IAudioEngine.h>
#include <Scout/IAssetSystem.h>
#include <Scout/Math.h>
#include <Scout/AudioEffects.h>
#include <Scout/IUiSystem.h>

#include <imgui.h>

// TODO: implement generic change of audio channels for nrOfChannels > 2 (ex: 3.0 to stereo and vice versa)
// TODO: add way to unregister sound fx
// TODO: implement oscilloscope as imgui widget
// TODO: vocoder
// TODO: low pass filter
// TODO: EQ
// TODO: implement ability to use non float PCM
// TODO: resolution change
// TODO: resampling
// TODO: be able to specify audio buffer size
// TODO: figure out why for a 8k clip, DFTing it and IDFTing it at 12k is the minimum required to not loose any frequencies.
// TODO: implement compressor
//			- smooth static curve compressor: TODO
//			- dynamic compressor: TODO
//			- real-time convolution with FFT: TODO
//			- peak measurement // https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/22640362#22640362 



constexpr const char* OUTPUT_PATH = "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav";

int main()
{
	// Modules setup.
	bool shutdown = false;

	Scout::WindowDef windowDef;
	windowDef.implementation = Scout::InputApi::SDL;
	windowDef.windowName = "MyWindow";
	windowDef.windowWidth = 720;
	windowDef.windowHeight = 720;
	auto window = Scout::MakeWindow(windowDef);

	Scout::AudioEngineDef audioEngineDef;
	audioEngineDef.implementation = Scout::AudioApi::PORTAUDIO;
	audioEngineDef.speakersSetup = Scout::SpeakerSetup::MONO;
	audioEngineDef.engineBufferSamplerate = Scout::Samplerate::Hz_8k;
	audioEngineDef.mixingPolicy = Scout::MixingPolicy::AVERAGE;
	auto audioEngine = Scout::MakeAudioEngine(audioEngineDef);

	Scout::GraphicsEngineDef graphicsEngineDef;
	graphicsEngineDef.implementation = Scout::GraphicalApi::SDL_RENDERER;
	graphicsEngineDef.viewportWidth = 720;
	graphicsEngineDef.viewportHeight = 720;
	auto graphicsEngine = Scout::MakeGraphicsEngine(graphicsEngineDef);

	// Setup sounds.
	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;

	auto audioData_Music_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_8kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	audioEngine->PlaySound(soundHandle_Music_1ch);

	const auto resampled = Scout::ResampleSignal(audioData_Music_1ch, 8000, 16000);
	wavIo->WriteWav(resampled, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav", 1, 16000);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		graphicsEngine->Update();
	}

	return 0;
}
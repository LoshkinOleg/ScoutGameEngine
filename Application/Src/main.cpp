#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <complex>
#include <algorithm>

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
// TODO: FFT and IFFT
// TODO: resampling
// TODO: figure out why for a 8k clip, DFTing it and IDFTing it at 12k is the minimum required to not loose any frequencies.
// TODO: implement compressor
//			- smooth static curve compressor: TODO
//			- dynamic compressor: TODO
//			- real-time convolution with FFT: TODO
//			- peak measurement // https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/22640362#22640362 


constexpr const char* OUTPUT_PATH = "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav";


// https://cp-algorithms.com/algebra/fft.html 
void fft(std::vector<std::complex<float>>& a)
{
	if (a.size() == 1) return;

	std::vector<std::complex<float>> a0(a.size() / 2), a1(a.size() / 2);

	// Split signal into even and odd samples
	for (int i = 0; 2 * i < a.size(); i++)
	{
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1];
	}
	
	// Recursive call.
	fft(a0);
	fft(a1);

	// Actual operation.
	const float PI = acosf(-1.0f);
	float ang = 2 * PI / a.size();

	std::complex<float> w(1), wn(cosf(ang), sinf(ang));

	for (int i = 0; 2 * i < a.size(); i++)
	{
		a[i] = a0[i] + w * a1[i];
		a[i + a.size() / 2] = a0[i] - w * a1[i];
		w *= wn;
	}
}

void ifft(std::vector<std::complex<float>>& a) {
	if (a.size() == 1)
		return;

	std::vector<std::complex<float>> a0(a.size() / 2), a1(a.size() / 2);
	for (int i = 0; 2 * i < a.size(); i++) {
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1];
	}
	ifft(a0);
	ifft(a1);

	const float PI = acosf(-1.0f);
	float ang = 2 * PI / a.size() * -1.0f;
	std::complex<float> w(1), wn(cos(ang), sin(ang));
	for (int i = 0; 2 * i < a.size(); i++) {
		a[i] = a0[i] + w * a1[i];
		a[i + a.size() / 2] = a0[i] - w * a1[i];
		a[i] /= 2;
		a[i + a.size() / 2] /= 2;
		w *= wn;
	}
}

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

	// Scout::UiSystemDef uiDef;
	// uiDef.implementation = Scout::UiApi::IMGUI;
	// uiDef.pRenderer = graphicsEngine.get();
	// uiDef.pInputsProvider = window.get();
	// auto uiSystem = Scout::MakeImmediateModeUiSystem(uiDef);

	// Setup sounds.
	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;

	auto audioData_Music_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_8kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const size_t padding = Scout::NearestUpperPowOfTwo(audioData_Music_1ch.size()) - audioData_Music_1ch.size();
	audioData_Music_1ch.insert(audioData_Music_1ch.end(), padding, 0.0f);
	
	std::vector<std::complex<float>> freqDomData(audioData_Music_1ch.size(), {0.0f, 0.0f});
	for (size_t i = 0; i < freqDomData.size(); i++)
	{
		freqDomData[i] = {audioData_Music_1ch[i], 0.0f};
	}
	fft(freqDomData);
	ifft(freqDomData);
	for (size_t i = 0; i < freqDomData.size(); i++)
	{
		// audioData_Music_1ch[i] = std::sqrtf(freqDomData[i].real() * freqDomData[i].real() + freqDomData[i].imag() * freqDomData[i].imag());
		audioData_Music_1ch[i] = freqDomData[i].real();
	}

	wavIo->WriteWav(audioData_Music_1ch, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_8kHz_32f_1ch_generated.wav", 1, 8000);
	
	// const auto soundHandle_Music_1ch = audioEngine->MakeSound(convolved, 1, false);
	// audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	// audioEngine->PlaySound(soundHandle_Music_1ch);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		graphicsEngine->Update();
	}

	return 0;
}
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

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
// TODO: implement compressor
//			- static curve compressor: OK
//			- dynamic compressor: TODO
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
	audioEngineDef.engineBufferSamplerate = Scout::Samplerate::Hz_48k;
	audioEngineDef.mixingPolicy = Scout::MixingPolicy::AVERAGE;
	auto audioEngine = Scout::MakeAudioEngine(audioEngineDef);

	Scout::GraphicsEngineDef graphicsEngineDef;
	graphicsEngineDef.implementation = Scout::GraphicalApi::SDL_RENDERER;
	graphicsEngineDef.viewportWidth = 720;
	graphicsEngineDef.viewportHeight = 720;
	auto graphicsEngine = Scout::MakeGraphicsEngine(graphicsEngineDef);

	Scout::UiSystemDef uiDef;
	uiDef.implementation = Scout::UiApi::IMGUI;
	uiDef.pRenderer = graphicsEngine.get();
	uiDef.pInputsProvider = window.get();
	auto uiSystem = Scout::MakeImmediateModeUiSystem(uiDef);

	// Bound static curve compressor.
	float noiseGateEnd = 0.0f;
	float upwardCompThreshold = 0.25f;
	float downwardCompThreshold = 0.75f;
	float limiterStart = 1.0f;
	float makeupGainFactor = 0.0f;
	float maxAbsSignalValue = 1.0f;
	auto myCompFx = [&](std::vector<float>& signal)
	{
		// https://www.desmos.com/calculator/dywe6plrzk

		// X values
		const float& i = noiseGateEnd;
		const float& j = upwardCompThreshold;
		const float& k = downwardCompThreshold;
		const float& l = limiterStart;

		// Y values
		const float& g = makeupGainFactor;
		const float& M = maxAbsSignalValue;

		auto a = [=](const float x)->float {return 0; };
		auto c = [=](const float x)->float {return x + g; };
		auto b = [=](const float x)->float {return M; };
		auto d = [=](const float x)->float
		{
			return
				x * ((c(j) - a(i)) / (j - i))
				- i * ((c(j) - a(i)) / (j - i))
				;
		};
		auto h = [=](const float x)->float
		{
			return
				x * ((b(l) - c(k)) / (l - k))
				+ (k - k * ((b(l) - c(k)) / (l - k)) + g)
				;
		};

		auto q = [=](const float x)->float {return 0; };
		auto w = [=](const float x)->float {return x - g; };
		auto r = [=](const float x)->float {return -M; };
		auto t = [=](const float x)->float
		{
			return
				x * ((q(-i) - w(-j)) / (j - i))
				+ i * ((q(-i) - w(-j)) / (j - i))
				;
		};
		auto u = [=](const float x)->float
		{
			return
				x * ((w(-k) - r(-l)) / (l - k))
				+ (r(-l) + l * ((w(-k) - r(-l)) / (l - k)))
				;
		};

		for (size_t sample = 0; sample < signal.size(); sample++)
		{
			float& val = signal[sample];
			if (val >= -1.0f && val <= -l)
			{
				val = r(val);
				continue;
			}
			if (val > -l && val <= -k)
			{
				val = u(val);
				continue;
			}
			if (val > -k && val <= -j)
			{
				val = w(val);
				continue;
			}
			if (val > -j && val <= -i)
			{
				val = t(val);
				continue;
			}
			if (val > -i && val <= 0.0f)
			{
				val = q(val);
				continue;
			}
			if (val > 0.0f && val <= i)
			{
				val = a(val);
				continue;
			}
			if (val > i && val <= j)
			{
				val = d(val);
				continue;
			}
			if (val > j && val <= k)
			{
				val = c(val);
				continue;
			}
			if (val > k && val <= l)
			{
				val = h(val);
				continue;
			}
			if (val > l && val <= 1.0f)
			{
				val = b(val);
				continue;
			}
		}
	};

	// Setup sounds.
	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;

	auto audioData_Sine440_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Sine440_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Sine440_1ch = audioEngine->MakeSound(audioData_Sine440_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Sine440_1ch, true);
	audioEngine->PlaySound(soundHandle_Sine440_1ch);
	audioEngine->RegisterEffectForSound(myCompFx, soundHandle_Sine440_1ch);

	auto audioData_Sweep_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweep_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Sweep_1ch = audioEngine->MakeSound(audioData_Sweep_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Sweep_1ch, true);
	// audioEngine->PlaySound(soundHandle_Sweep_1ch);

	auto audioData_SweepReverse_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweepReverse_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_SweepReverse_1ch = audioEngine->MakeSound(audioData_SweepReverse_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_SweepReverse_1ch, true);
	// audioEngine->PlaySound(soundHandle_SweepReverse_1ch);
	
	auto audioData_OffsetSweeps_2ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/OffsetStereoSweeps_48kHz_32f_2ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_OffsetSweeps_2ch = audioEngine->MakeSound(audioData_OffsetSweeps_2ch, 2, true);
	audioEngine->SetSoundLooped(soundHandle_OffsetSweeps_2ch, true);
	// audioEngine->PlaySound(soundHandle_OffsetSweeps_2ch);
	
	auto audioData_Noise_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/WhiteNoiseMonoTremolo_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Noise_1ch = audioEngine->MakeSound(audioData_Noise_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Noise_1ch, true);
	// audioEngine->PlaySound(soundHandle_Noise_1ch);
	
	auto audioData_Music_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	// audioEngine->PlaySound(soundHandle_Music_1ch);
	
	auto audioData_Music_2ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/MusicStereo_48kHz_32f_2ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_Music_2ch = audioEngine->MakeSound(audioData_Music_2ch, 2, true);
	audioEngine->SetSoundLooped(soundHandle_Music_2ch, true);
	// audioEngine->PlaySound(soundHandle_Music_2ch);
	
	auto audioData_DrumNBase_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/drumNBase_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_drumNBase_1ch = audioEngine->MakeSound(audioData_DrumNBase_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_drumNBase_1ch, true);
	// audioEngine->PlaySound(soundHandle_drumNBase_1ch);
	
	std::vector<float> audioData_dummy_1ch = {0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f};
	const auto soundHandle_dummy_1ch = audioEngine->MakeSound(audioData_dummy_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_dummy_1ch, true);
	// audioEngine->PlaySound(soundHandle_dummy_1ch);
	
	std::vector<float> audioData_dummy_2ch = {0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -1.0f, -1.0f, -0.5f, -0.5f};
	const auto soundHandle_dummy_2ch = audioEngine->MakeSound(audioData_dummy_2ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_dummy_2ch, true);
	// audioEngine->PlaySound(soundHandle_dummy_2ch);

	auto audioData_VolumeLine_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/VolumeLine_48kHz_32_1ch.wav", nrOfChannels, sampleRate);
	const auto soundHandle_VolumeLine_1ch = audioEngine->MakeSound(audioData_VolumeLine_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_VolumeLine_1ch, true);
	// audioEngine->PlaySound(soundHandle_VolumeLine_1ch);

	// Input bindings.
	auto decreaseI = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			noiseGateEnd -= 0.05f;
			noiseGateEnd = std::clamp(noiseGateEnd, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::Q, decreaseI);
	auto increaseI = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			noiseGateEnd += 0.05f;
			noiseGateEnd = std::clamp(noiseGateEnd, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::W, increaseI);

	auto decreaseJ = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			upwardCompThreshold -= 0.05f;
			upwardCompThreshold = std::clamp(upwardCompThreshold, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::A, decreaseJ);
	auto increaseJ = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			upwardCompThreshold += 0.05f;
			upwardCompThreshold = std::clamp(upwardCompThreshold, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::S, increaseJ);

	auto decreaseK = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			downwardCompThreshold -= 0.05f;
			downwardCompThreshold = std::clamp(downwardCompThreshold, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::Y, decreaseK);
	auto increaseK = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			downwardCompThreshold += 0.05f;
			downwardCompThreshold = std::clamp(downwardCompThreshold, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::X, increaseK);

	auto decreaseL = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			limiterStart -= 0.05f;
			limiterStart = std::clamp(limiterStart, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::E, decreaseL);
	auto increaseL = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			limiterStart += 0.05f;
			limiterStart = std::clamp(limiterStart, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::R, increaseL);

	auto decreaseM = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			maxAbsSignalValue -= 0.05f;
			maxAbsSignalValue = std::clamp(maxAbsSignalValue, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::D, decreaseM);
	auto increaseM = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			maxAbsSignalValue += 0.05f;
			maxAbsSignalValue = std::clamp(maxAbsSignalValue, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::F, increaseM);

	auto decreaseG = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			makeupGainFactor -= 0.05f;
			makeupGainFactor = std::clamp(makeupGainFactor, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::C, decreaseG);
	auto increaseG = [&](bool justPressed, bool repeating)
	{
		if (justPressed && !repeating)
		{
			makeupGainFactor += 0.05f;
			makeupGainFactor = std::clamp(makeupGainFactor, 0.0f, 1.0f);
		}
	};
	window->RegisterKeyboardCallback(Scout::KeyboardKey::V, increaseG);

	// Oscilloscope
	auto peekBuffer = [](const std::vector<float>& signalBuffer, std::vector<float>& visualizationSignal)
	{
		assert(visualizationSignal.size() && visualizationSignal.size() < signalBuffer.size());
		std::copy(signalBuffer.begin(), signalBuffer.begin() + visualizationSignal.size(), visualizationSignal.begin());
	};
	auto drawOscilloscope = [](Scout::IGraphicsEngine* renderer, const std::vector<float>& signal)
	{
		size_t oscilloscopeOffset = 0;
		for (size_t i = 0; i < signal.size(); i++)
		{
			if (abs(signal[i]) < 0.01f && signal[i + 1] > signal[i])
			{
				oscilloscopeOffset = i;
				break;
			}
		}
		const float intervalX = 1.0f / signal.size();
		for (size_t i = 0; i < signal.size() - 1 - oscilloscopeOffset; i++)
		{
			renderer->DrawLine(intervalX * i, -(signal[i + oscilloscopeOffset] / 2.0f) + 0.5f, intervalX * (i + 1), -(signal[i + 1 + oscilloscopeOffset] / 2.0f) + 0.5f, 1.0f, Scout::COLOR_WHITE);
		}
	};
	std::vector<float> visualizationSignal(1024, 0.0f);

	auto drawStaticCompControls = [&]()
	{
		ImGui::NewFrame();
		ImGui::SliderFloat("Noise gate end: ", &noiseGateEnd, 0.0f, 0.99f);
		ImGui::SliderFloat("Upward compression threshold: ", &upwardCompThreshold, noiseGateEnd, 0.99f);
		ImGui::SliderFloat("Downward compression threshold: ", &downwardCompThreshold, upwardCompThreshold, 0.99f);
		ImGui::SliderFloat("Limiter start: ", &limiterStart, downwardCompThreshold, 0.99f);
		ImGui::SliderFloat("Makeup gain: ", &makeupGainFactor, -0.99f, 0.99f);
		ImGui::SliderFloat("Limiter max value: ", &maxAbsSignalValue, limiterStart, 0.99f);
	};
	uiSystem->RegisterDrawingCallback(drawStaticCompControls);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		auto& buff = audioEngine->GetRawBuffer();
		peekBuffer(buff, visualizationSignal);
		drawOscilloscope(graphicsEngine.get(), visualizationSignal);

		graphicsEngine->Update();
	}

	return 0;
}
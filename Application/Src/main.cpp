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

// TODO: implement generic change of audio channels for nrOfChannels > 2 (ex: 3.0 to stereo and vice versa)
// TODO: add way to unregister sound fx
// TODO: implement compressor
//			- peak measurement // https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/22640362#22640362 
// TODO: debug static curve compressor. look at the graphs. Issues: downwardCompNeg, upwardCompNeg, downwardCompPos

constexpr const char* OUTPUT_PATH = "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav";

int main()
{
	auto simpleCompressor = [](std::vector<float>& signal)->void
	{
		for (size_t i = 0; i < signal.size(); i++)
		{
			// Results in buzzing artefacts if sine is played. Results in aliasing if sweep is played. The wave form is reminescent of the graph for quantization noise.
			// signal[i] = 0.85f * cbrtf(signal[i]);
			// signal[i] = 0.85f * powf(signal[i], 1.0f/1.8f);
			// signal[i] = tanhf(signal[i] * 1.45f);
			signal[i] = tanhf(signal[i] * 2.45f);
		}
	};
	auto attenuationFx = [](std::vector<float>& signal)
	{
		for (size_t i = 0; i < signal.size(); i++)
		{
			signal[i] *= 0.5f;
		}
	};
	auto simpleNoiseGate = [](std::vector<float>& signal)
	{
		for (size_t i = 0; i < signal.size(); i++)
		{
			if ((signal[i] < 0.2f && signal[i] > 0.0f) || (signal[i] > -0.2f && signal[i] < 0.0f))
			{
				signal[i] = 0.0f;
			}
		}
	};
	auto staticCurveCompressor = [](std::vector<float>& signal)
	{
		// https://www.desmos.com/calculator/dywe6plrzk

		// X values
		constexpr const float i = 0.05f;
		constexpr const float j = 0.183f;
		constexpr const float k = 0.673f;
		constexpr const float l = 0.95f;

		// Y values
		constexpr const float g = 0.17f;
		constexpr const float M = 0.954f;

		auto a = [=](const float x)->float {return 0; };
		auto c = [=](const float x)->float {return x + g; };
		auto b = [=](const float x)->float {return M; };
		auto d = [=](const float x)->float
		{
			return
				x * ( (c(j) - a(i)) / (j - i) )
				-i * ( (c(j) - a(i)) / (j - i) )
				;
		};
		auto h = [=](const float x)->float
		{
			return
				x * ( (b(l) - c(k)) / (l - k) )
				+ ( k - k * ( (b(l) - c(k)) / (l - k) ) + g )
				;
		};

		auto q = [=](const float x)->float {return 0; };
		auto w = [=](const float x)->float {return x - g; };
		auto r = [=](const float x)->float {return -M; };
		auto t = [=](const float x)->float
		{
			return
				x * ( (q(-i) - w(-j)) / (j - i) )
				+i * ( (q(-i) - w(-j)) / (j - i) )
				;
		};
		auto u = [=](const float x)->float
		{
			return
				x * ( (w(-k) - r(-l)) / (l - k) )
				+ ( r(-l) + l * ( (w(-k) - r(-l)) / (l - k) ) )
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

	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;

	auto audioData_Sine440_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Sine440_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	// simpleCompressor(audioData_Sine440_1ch);
	staticCurveCompressor(audioData_Sine440_1ch);
	wavIo->WriteWav(audioData_Sine440_1ch, OUTPUT_PATH, 1, 48000);
	const auto soundHandle_Sine440_1ch = audioEngine->MakeSound(audioData_Sine440_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Sine440_1ch, true);
	audioEngine->PlaySound(soundHandle_Sine440_1ch);

	auto audioData_Sweep_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweep_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	// simpleCompressor(audioData_Sweep_1ch);
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
	// audioEngine->RegisterEffectForSound(simpleNoiseGate, soundHandle_drumNBase_1ch);
	
	std::vector<float> audioData_dummy_1ch = {0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f};
	const auto soundHandle_dummy_1ch = audioEngine->MakeSound(audioData_dummy_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_dummy_1ch, true);
	// audioEngine->PlaySound(soundHandle_dummy_1ch);
	
	std::vector<float> audioData_dummy_2ch = {0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -1.0f, -1.0f, -0.5f, -0.5f};
	const auto soundHandle_dummy_2ch = audioEngine->MakeSound(audioData_dummy_2ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_dummy_2ch, true);
	// audioEngine->PlaySound(soundHandle_dummy_2ch);

	// std::vector<float> audioData_dummyLinearIncrease_1ch(48000, 0.0f);
	// const float increment = 2.0f / 48000.0f;
	// float accumulatedVal = -1.0f;
	// for (size_t i = 0; i < audioData_dummyLinearIncrease_1ch.size(); i++)
	// {
	// 	audioData_dummyLinearIncrease_1ch[i] = accumulatedVal;
	// 	accumulatedVal += increment;
	// }
	// staticCurveCompressor(audioData_dummyLinearIncrease_1ch);
	// wavIo->WriteWav(audioData_dummyLinearIncrease_1ch, OUTPUT_PATH, 1, 48000);

	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();
	}
	return 0;
}
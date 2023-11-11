#include <cassert>

#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>
#include <Scout/IAudioEngine.h>
#include <Scout/IAssetSystem.h>
// #include <Scout/Math.h>
#include <Scout/AudioEffects.h>
#include <cmath>

// TODO: implement generic change of audio channels for nrOfChannels > 2 (ex: 3.0 to stereo and vice versa)
// TODO: add way to unregister sound fx
// TODO: implement compressor
//			- delay
//			- noise gate
//			- peak measurement
//			- RMS measurement
// TODO: find origin of buzzing when sending audio to headset.

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
	audioEngineDef.speakersSetup = Scout::SpeakerSetup::MONO;
	audioEngineDef.engineBufferSamplerate = Scout::Samplerate::Hz_48k;
	audioEngineDef.mixingPolicy = Scout::MixingPolicy::AVERAGE;
	auto audioEngine = Scout::MakeAudioEngine(audioEngineDef);

	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;
	auto audioData_Sine440_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Sine440_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	auto audioData_Sweep_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweep_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	auto audioData_SweepReverse_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/LinearSweepReverse_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	auto audioData_OffsetSweeps_2ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/OffsetStereoSweeps_48kHz_32f_2ch.wav", nrOfChannels, sampleRate);
	auto audioData_Noise_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/WhiteNoiseMonoTremolo_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	auto audioData_Music_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	auto audioData_Music_2ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/MusicStereo_48kHz_32f_2ch.wav", nrOfChannels, sampleRate);
	auto audioData_DrumNBase_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/drumNBase_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	std::vector<float> audioData_dummy_1ch = {0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f};
	std::vector<float> audioData_dummy_2ch = {0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -1.0f, -1.0f, -0.5f, -0.5f};

	// auto dataDummy = audioData_dummy_2ch;
	// Scout::StereoToMonoSignal(dataDummy, true);

	auto simpleCompressor = [](std::vector<float>& signal)->void
	{
		for (size_t i = 0; i < signal.size(); i++)
		{
			// signal[i] = 0.85f * powf(signal[i], 0.3333f);
			signal[i] = 1.0f * cbrtf(signal[i]);
		}
	};
	simpleCompressor(audioData_Sine440_1ch);

	auto attenuationFx = [](std::vector<float>& signal)
	{
		for (size_t i = 0; i < signal.size(); i++)
		{
			signal[i] *= 0.5f;
		}
	};

	const auto soundHandle_Sine440_1ch = audioEngine->MakeSound(audioData_Sine440_1ch, 1, false);
	const auto soundHandle_Sweep_1ch = audioEngine->MakeSound(audioData_Sweep_1ch, 1, false);
	// const auto soundHandle_SweepBis_1ch = audioEngine->MakeSound(audioData_Sweep_1ch, 1, false);
	// const auto soundHandle_SweepReverse_1ch = audioEngine->MakeSound(audioData_SweepReverse_1ch, 1, false);
	// const auto soundHandle_OffsetSweeps_2ch = audioEngine->MakeSound(audioData_OffsetSweeps_2ch, 2, true);
	// const auto soundHandle_Noise_1ch = audioEngine->MakeSound(audioData_Noise_1ch, 1, false);
	const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	// const auto soundHandle_Music_2ch = audioEngine->MakeSound(audioData_Music_2ch, 2, true);
	// const auto soundHandle_dummy_1ch = audioEngine->MakeSound(audioData_dummy_1ch, 1, false);
	// const auto soundHandle_dummy_2ch = audioEngine->MakeSound(audioData_dummy_2ch, 1, false);
	// const auto soundHandle_drumNBase_1ch = audioEngine->MakeSound(audioData_DrumNBase_1ch, 1, false);
	
	audioEngine->SetSoundLooped(soundHandle_Sine440_1ch, true);
	audioEngine->SetSoundLooped(soundHandle_Sweep_1ch, true);
	// audioEngine->SetSoundLooped(soundHandle_SweepBis_1ch, true);
	// audioEngine->SetSoundLooped(soundHandle_SweepReverse_1ch, true);
	// audioEngine->SetSoundLooped(soundHandle_OffsetSweeps_2ch, true);
	// audioEngine->SetSoundLooped(soundHandle_Noise_1ch, true);
	audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	// audioEngine->SetSoundLooped(soundHandle_Music_2ch, true);
	// audioEngine->SetSoundLooped(soundHandle_dummy_1ch, true);
	// audioEngine->SetSoundLooped(soundHandle_dummy_2ch, true);
	// audioEngine->SetSoundLooped(soundHandle_drumNBase_1ch, true);
	
	audioEngine->PlaySound(soundHandle_Sine440_1ch);
	// audioEngine->PlaySound(soundHandle_Sweep_1ch);
	// audioEngine->PlaySound(soundHandle_SweepBis_1ch);
	// audioEngine->PlaySound(soundHandle_SweepReverse_1ch);
	// audioEngine->PlaySound(soundHandle_OffsetSweeps_2ch);
	// audioEngine->PlaySound(soundHandle_Noise_1ch);
	// audioEngine->PlaySound(soundHandle_Music_1ch);
	// audioEngine->PlaySound(soundHandle_Music_2ch);
	// audioEngine->PlaySound(soundHandle_dummy_1ch);
	// audioEngine->PlaySound(soundHandle_dummy_2ch);
	// audioEngine->PlaySound(soundHandle_drumNBase_1ch);

	// auto signalCopy = audioData_Sine440_1ch;
	// simpleCompressor(audioData_Sine440_1ch);
	// wavIo->WriteWav(audioData_Sine440_1ch, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav", 1, 48000);

	// audioEngine->RegisterEffectForDisplay(Scout::MakeLimiterCallback(0.75f));
	// audioEngine->RegisterEffectForSound(attenuationFx, soundHandle_Sine440_1ch);
	
	// audioEngine->RegisterEffectForDisplay(simpleCompressor);


	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		// auto buff = audioEngine->GetRawBuffer();
		// wavIo->WriteWav(audioData_Sine440_1ch, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/generated.wav", 1, 48000);
		// return 0;
	}
	return 0;
}
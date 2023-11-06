#include <cassert>

#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>
#include <Scout/IAudioEngine.h>
#include <Scout/IAssetSystem.h>
#include <Scout/Math.h>

// TODO: fix issue with stereo looping too early

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
	auto audioEngine = Scout::MakeAudioEngine(audioEngineDef);

	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;
	const auto audioData = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/MusicStereo_48kHz_32f.wav", nrOfChannels, sampleRate);

	assert(audioData.size() > 0);
	const auto soundHandle = audioEngine->MakeSound(audioData, 2, true);
	audioEngine->SetSoundLooped(soundHandle, true);
	audioEngine->PlaySound(soundHandle);

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
	audioEngine->RegisterEffectForDisplay(fx);

	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();
	}
	return 0;
}
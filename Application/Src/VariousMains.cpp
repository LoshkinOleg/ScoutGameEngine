/*
int CompressorMain()
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

	std::vector<float> audioData_dummy_1ch = { 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f };
	const auto soundHandle_dummy_1ch = audioEngine->MakeSound(audioData_dummy_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_dummy_1ch, true);
	// audioEngine->PlaySound(soundHandle_dummy_1ch);

	std::vector<float> audioData_dummy_2ch = { 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -1.0f, -1.0f, -0.5f, -0.5f };
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

int ConvolutionTest()
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

	// Scout::UiSystemDef uiDef;
	// uiDef.implementation = Scout::UiApi::IMGUI;
	// uiDef.pRenderer = graphicsEngine.get();
	// uiDef.pInputsProvider = window.get();
	// auto uiSystem = Scout::MakeImmediateModeUiSystem(uiDef);

	// Setup sounds.
	auto wavIo = Scout::MakeWavIo({});
	uint64_t nrOfChannels, sampleRate;

	auto audioData_Music_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/drumNBase_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	for (size_t i = 0; i < audioData_Music_1ch.size(); i++)
	{
		// audioData_Music_1ch[i] *= 0.25f;
	}
	// const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	// audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	// audioEngine->PlaySound(soundHandle_Music_1ch);

	auto audioData_Impulse_1ch = wavIo->LoadWavF32("C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/MINI_CAVES_E001_M2SIR_48kHz_32f_1ch.wav", nrOfChannels, sampleRate);
	for (size_t i = 0; i < audioData_Impulse_1ch.size(); i++)
	{
		// audioData_Impulse_1ch[i] *= 0.25f;
	}
	// const auto soundHandle_Impulse_1ch = audioEngine->MakeSound(audioData_Impulse_1ch, 1, false);
	// audioEngine->SetSoundLooped(soundHandle_Impulse_1ch, true);
	// audioEngine->PlaySound(soundHandle_Music_1ch);

	std::vector<float> convolved(audioData_Music_1ch.size() + audioData_Impulse_1ch.size() - 1);
	Scout::ConvolveMonoSignals_LinearConvolution_TimeDomain(audioData_Music_1ch, audioData_Impulse_1ch, convolved);
	const auto conv = audioEngine->MakeSound(convolved, 1, false);
	audioEngine->SetSoundLooped(conv, true);
	audioEngine->PlaySound(conv);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		graphicsEngine->Update();
	}

	return 0;
}

int 12kMiniFor8kClipWierdness()
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
	std::vector<std::complex<float>> audioData_Music_1ch_freqDomain(12000, {0.0f, 0.0f}); // Edit: 12k since the minimal frequency is 4k and there's 3 seconds of data.
	Scout::DFT(audioData_Music_1ch_freqDomain, audioData_Music_1ch, audioData_Music_1ch_freqDomain.size());
	std::fill(audioData_Music_1ch_freqDomain.begin() + audioData_Music_1ch_freqDomain.size() / 2, audioData_Music_1ch_freqDomain.end(), std::complex<float>{0.0f, 0.0f});
	Scout::IDFT(audioData_Music_1ch, audioData_Music_1ch_freqDomain, audioData_Music_1ch.size());

	Scout::WavIoDef waveIoDef;
	waveIoDef.implementation = Scout::WavIoApi::DR_WAV;
	auto wavIoOut = Scout::MakeWavIo(waveIoDef);
	wavIoOut->WriteWav(audioData_Music_1ch, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_8kHz_32f_1ch_generated.wav", 1, 8000);

	const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	audioEngine->PlaySound(soundHandle_Music_1ch);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		graphicsEngine->Update();
	}

	return 0;
}

int ExampleOfUsingFirForFiltering()
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

	// https://fiiir.com/ for FIR generation
	const std::vector<float> bandpassFilter = { 0.0 };
	// std::vector<float> convolved(audioData_Music_1ch.size() + bandpassFilter.size() - 1, 0.0f);
	// Scout::ConvolveMonoSignals_LinearConvolution_TimeDomain(audioData_Music_1ch, bandpassFilter, convolved);
	wavIo->WriteWav(bandpassFilter, "C:/Users/user/Desktop/ScoutGameEngine/Resource/Audio/Music_8kHz_32f_1ch_generated.wav", 1, 8000);

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

int spectralFiltersNotWorking()
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
	const auto soundHandle_Music_1ch = audioEngine->MakeSound(audioData_Music_1ch, 1, false);
	audioEngine->SetSoundLooped(soundHandle_Music_1ch, true);
	audioEngine->PlaySound(soundHandle_Music_1ch);

	auto lowPassFx = [&](std::vector<float>& signal)
	{
		assert(audioEngine->GetBufferDuration() < std::chrono::milliseconds(1000)); // Not handling really big buffers.
		const size_t transitionBegin = 1000;
		const size_t transitionEnd = 4000;

		static std::vector<float> paddedSignal(Scout::NearestUpperPowOfTwo((size_t)audioEngine->GetSamplerate()), 0.f);
		static std::vector<std::complex<float>> freqDomSignal(paddedSignal.size(), {0.f,0.f});
		std::copy(signal.begin(), signal.end(), paddedSignal.begin());

		Scout::FFT(paddedSignal, freqDomSignal);
		for (size_t freq = 0; freq < freqDomSignal.size(); freq++)
		{
			if (freq >= transitionBegin && freq <= transitionEnd)
			{
				const float v = Scout::Lerp(1.f, 0.f, (float)(freq - transitionBegin) / (float)(transitionEnd - transitionBegin));
				freqDomSignal[freq] *= v;
			}
			else if (freq > transitionEnd)
			{
				freqDomSignal[freq] = 0.f;
			}
		}
		Scout::IFFT(freqDomSignal, paddedSignal);
		std::copy(paddedSignal.begin(), paddedSignal.begin() + signal.size(), signal.begin());
	};
	auto notchFx = [&](std::vector<float>& signal)
		{
			assert(audioEngine->GetBufferDuration() < std::chrono::milliseconds(1000)); // Not handling really big buffers.
			const size_t a = 500;
			const size_t b = 1000;
			const size_t c = 1500;
			const size_t d = 2000;

			static std::vector<float> paddedSignal(Scout::NearestUpperPowOfTwo((size_t)audioEngine->GetSamplerate()), 0.f);
			static std::vector<std::complex<float>> freqDomSignal(paddedSignal.size(), { 0.f,0.f });
			std::copy(signal.begin(), signal.end(), paddedSignal.begin());

			Scout::FFT(paddedSignal, freqDomSignal);
			for (size_t freq = 0; freq < freqDomSignal.size(); freq++)
			{
				if (freq >= a && freq <= b)
				{
					const float v = Scout::Lerp(1.f, 0.f, (float)(freq - a) / (float)(b - a));
					freqDomSignal[freq] *= v;
				}
				else if (freq > b && freq <= c)
				{
					freqDomSignal[freq] = 0.f;
				}
				if (freq >= c && freq <= d)
				{
					const float v = Scout::Lerp(0.f, 1.f, (float)(freq - c) / (float)(d - c));
					freqDomSignal[freq] *= v;
				}
			}
			Scout::IFFT(freqDomSignal, paddedSignal);
			std::copy(paddedSignal.begin(), paddedSignal.begin() + signal.size(), signal.begin());
		};
	audioEngine->RegisterEffectForDisplay(notchFx);

	// Game loop.
	while (!shutdown)
	{
		window->PollEvents(Scout::HidTypeFlag::MOUSE_AND_KEYBOARD, shutdown);
		audioEngine->Update();

		graphicsEngine->Update();
	}

	return 0;
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
*/
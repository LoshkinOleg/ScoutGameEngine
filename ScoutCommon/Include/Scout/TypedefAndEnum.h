#pragma once

#include <cstdint>
#include <functional>

#include <Scout/PreprocessorMacro.h>

namespace Scout
{
	// Ui system

	using ImmediateModeUiDrawingCallback = std::function<void()>;

	/*
	@firstArgument Pointer to event to process.
	@return Indicates whether the event has been consumed by the UI system.
	*/
	using ImmediateModeUiProcessEventCallback = std::function<bool(const void*)>;

	enum class UiApi : std::uint64_t
	{
		NONE = 0,

		IMGUI,
	};

	// Asset system

	enum class WavIoApi : std::uint64_t
	{
		NONE = 0,

		DR_WAV,
	};

	// Audio

	SCOUT_DECLARE_HANDLE(SoundHandle);
	SCOUT_DECLARE_HANDLE(SfxHandle);

	using SoundSpecificEffectCallback = std::function<void(std::vector<float>& audioData)>; // Applied to specified individual sound.
	using AudioDisplayEffectCallback = std::function<void(std::vector<float>& audioData)>; // Applied by engine by the end of the sounds mixing.

	enum class Bitdepth : std::uint64_t
	{
		NONE = 0,

		F64, // 64-bit float
		F32, // 32-bit float, default for DSP
		F16, // 16-bit half

		B32, // 32-bit integer
		B24, // 24-bit integer, default for audio production
		B16, // 16-bit integer
		B8, // 8-bit integer, minimal bitdepth without going sub-byte

		// Aliases for dynamic ranges
		DynRange_192Db = B32,
		DynRange_144Db = B24,
		DynRange_96Db = B16,
		DynRange_42Db = B8,
	};

	enum class Samplerate : std::uint64_t
	{
		// Referenced from: https://github.com/audiojs/sample-rate

		NONE = 0,

		Hz_8k, // minimal sampling for voice intelligebility
		Hz_16k, // standard VoIP
		Hz_22050, // higher sampled MPEG audio
		Hz_44100, // standard CD
		Hz_48k, // default sampling rate for audio production
		Hz_96k, // DVD and Blu-ray
		Hz_384k, // highest samplerate for common software
	};

	enum class SpeakerSetup : std::uint64_t
	{
		NONE = 0,

		// 1 channel
		MONO,

		// 2 channels
		DUAL_MONO,
		STEREO,

		// 3 channels
		TWO_POINT_ONE,
		LCR,

		// 4 channels
		THREE_POINT_ONE,

		// 5 channels
		FIVE_POINT_O,

		// 6 channels
		FIVE_POINT_ONE,

		// 7 channels
		SEVEN_POINT_O,

		// 8 channels
		SEVEN_POINT_ONE,

		// yolo
		ATMOS,
		VIRTUAL_AMBISONIC,
	};

	enum class AudioApi : std::uint64_t
	{
		NONE = 0,

		PORTAUDIO,
	};

	enum class MixingPolicy: std::uint64_t
	{
		NONE = 0,

		SUM_AND_CLAMP,
	};

	// Graphical

	SCOUT_DECLARE_HANDLE(VaoHandle);
	SCOUT_DECLARE_HANDLE(VboHandle);
	SCOUT_DECLARE_HANDLE(EboHandle);
	SCOUT_DECLARE_HANDLE(TextureHandle);
	SCOUT_DECLARE_HANDLE(ShaderHandle);
	SCOUT_DECLARE_HANDLE(MaterialHandle);
	SCOUT_DECLARE_HANDLE(MeshHandle);
	SCOUT_DECLARE_HANDLE(ModelHandle);

	enum class ShaderParamSize : std::uint64_t
	{
		NONE = 0,

		F32 = 1,
		VEC2F = 2,
		VEC3F = 3,
		VEC4F = 4,
		MAT3 = 9,
		MAT4 = 16,
	};

	enum class GraphicalApi : std::uint64_t
	{
		NONE = 0,

		SDL_RENDERER
	};

	enum class BufferContent : std::uint64_t
	{
		NONE = 0,

		POSITIONS,
		MATRICES,
		TEXCOORDS,
		NORMALS,
		TANGENTS,
		BITANGENTS,
		INDICES,
	};

	enum class ShadingType : std::uint64_t
	{
		NONE = 0,

		WIREFRAME,
		SOLID_COLOR,
		BASE_COLOR,
		GOOCH,
	};

	enum class TextureColorFormat : std::uint64_t
	{
		NONE = 0,

		RGBA_B8,
		RGB_B8,
		RED_F32,
	};

	enum class TextureSampling : std::uint64_t
	{
		NONE = 0,

		NEAREST,
		LINEAR,
	};

	enum class TextureWrapping : std::uint64_t
	{
		NONE = 0,

		CLAMP,
		REPEAT,
		MIRRORED_REPEAT,
	};

	enum class TextureCompression : std::uint64_t
	{
		NONE = 0,
	};

	enum class AlphaBlending : std::uint64_t
	{
		NONE = 0,

		ALPHA,
		ONE_MINUS_ALPHA,
	};

	enum class TextureSlot : std::uint64_t
	{
		NONE = 0,

		TEX_0,
		TEX_1,
		TEX_2,
		TEX_3,
		TEX_4,
		TEX_5,
		TEX_6,
		TEX_7,
		TEX_8,
		TEX_9,
		TEX_10,
		TEX_11,
		TEX_12,
		TEX_13,
		TEX_14,
		TEX_15, // Minimal number of texture slots defined by OpenGL33

		BASE_COLOR = TEX_0,

		SPECULAR = TEX_1,
		ROUGHNESS = TEX_2,
		CLEARCOAT = TEX_3,

		NORMALS = TEX_4,

		GLOW = TEX_5,

		METALLIC = TEX_6,

		HEIGHT_MAP = TEX_7,

		PARRALAX = TEX_8,
	};

	// Input system

	enum class InputApi : std::uint64_t
	{
		NONE = 0,

		SDL,
	};

	enum class HidTypeFlag : std::uint64_t
	{
		NONE = 0,

		KEYBOARD = 1 << 0,
		MOUSE = 1 << 1,
		MOUSE_AND_KEYBOARD = MOUSE | KEYBOARD,
	};

	enum class InputAction : std::uint64_t
	{
		NONE = 0,

		QUIT,

		CONFIRM,
		CANCEL,

		RIGHT,
		UP,
		LEFT,
		DOWN,
		FORWARD,
		BACKWARD,

		TOUCH_PRIMARY, // LMB or brief screen touch
		TOUCH_SECONDARY, // RMB or held screen touch
		TOUCH_MOVE, // Mouse motion or screen touch and move
		ZOOM_IN, // Scroll wheel up or two fingers screen touch then spread
		ZOOM_OUT, // Scroll wheel down or two fingers screen touch then join
	};

	enum class MouseKey : std::uint64_t
	{
		NONE = 0,

		LMB,
		RMB,
		MMB,
		M4,
		M5
	};

	/*
		First argument is horizontal mouse movement delta.
		Second argument is vertical mouse movement delta.
	*/
	using MouseMovementCallback = std::function<void(float, float)>;
	/*
		First argument is vertical mouse wheel delta.
		Second argument is horizontal mouse movement delta.
	*/
	using MouseScrollCallback = std::function<void(float, float)>;
	/*
		First argument is true when button has just been pressed.
		First argument is false when button has just been relesed.
		Second argument is true when the event was a double (or more) click.
		Third argument is horizontal screenspace mouse position.
		Fourth argument is vertical screenspace mouse position.
	*/
	using MouseBtnCallback = std::function<void(bool, bool, float, float)>;

	enum class KeyboardKey : std::uint64_t
	{
		NONE = 0,

		ESCAPE,

		// Horizontal movement keys.
		W,
		UP_ARROW,
		A,
		LEFT_ARROW,
		S,
		DOWN_ARROW,
		D,
		RIGHT_ARROW,

		// Use keys
		E,
		F,

		// Misc action keys.
		Q,
		R,
		C,
		X,
		Z, // Y on QWERTZ layout
		ONE,
		TWO,
		THREE,
		FOUR,
		TILDE, // � on QWERTZ layout
		SPACE, // Move up key
		TAB, // Cycle key

		// Modifier keys
		LCTRL = (std::uint64_t)1 << 63, // Move down key
		LSHIFT = (std::uint64_t)1 << 62, // Sprint key
		LALT = (std::uint64_t)1 << 61,
	};

	/*
		First argument is true when key has just been pressed.
		First argument is false when key has just been relesed.
		Second argument is true when key is repeating.
	*/
	using KeyboardCallback = std::function<void(bool, bool)>;
}
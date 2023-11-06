# ON HOLD
Because turns out trying to build a whole engine without doing extensive research into game engine architecture isn't a good idea.

# Scout Game Engine
Collection of in-house game engine modules set up using CMake. Made for personal experimentation.<br>
Modules are located in their own git repositories which are included in this repo as submodules.<br>
To clone this repo with the submodules: git clone --recurse-submodules https://github.com/LoshkinOleg/ScoutGameEngine.git <br>
To pull the submodules if you didn't clone the repo as suggested above: git submodule update --init --recursive <br>
To pull external changes to submodules: git submodule update --remote --merge <br>

# TODO
- Implement rendering of textures with SDL implementation
- Make tool to enforce naming conventions of files and folders.
- Move abstract-implementation specific enum conversion functions to TypedefAndEnum files, and rename these functions as "ToAbstractEnum_implementation" and "ToImplEnum_implementation"
- Make a pass on all exceptions to see which ones can be replaced by asserts.
- Implement a macro to make throwing exceptions more readable.
- Define a consistent std::string_view vs. Scout::Path usage in code
- Create tests for all modules (especially math and string ones).

# Bugs
- Portaudio implementation of audio engine doesn't work on Linux, see readme in ScoutAudioEngine for details.
- Portaudio implementation breaks for very large latencies (3 sec)

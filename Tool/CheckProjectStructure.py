"""
This tool is made to detect naming inconsistencies across directories and files.

- Use the singular form for naming classes, files and folders. (ex: GraphicEngine, not GraphicsEngine)
- For acronyms, use PascalCase. (ex: Sdl, not SDL or sdl)
- For implementation files
- For folders:
	- Use PascalCase
	- Exemptions:
		- Contents of Thirdparty
        - IDE and git folders
        - build folder
- For classes:
	- Interfaces start with I . (ex: IWindow)
	- Implementations of interfaces bear the name of the abstraction followed by _<implementation> . (ex: Window_Sdl, GraphicEngine_BespokeGl)
- For files:
	- Use PascalCase.ext
	- Implementation files bear the name of the abstraction and are located in an implementation subdirectory. (ex: /include/Scout/Sdl/Window.h contains definition of Window_Sdl, a subclass of IWindow)
	- For sound files:
        - Use PascalCase_<sampleRate>_<bitDepth>.ext
    - Exemptions:
		- Contents of Thirdparty
		- CMakeLists.txt files
        - .gitignore file
        - readme files

Usage:
- Place script at project's root directory.
- Run "python CheckProjectStructure.py"
- The tool will give you the path of the first non-conforming directory or file it finds.
"""

folders_to_ignore = [".git", ".vscode", ".vs"]
files_to_ignore = [".gitignore", "CMakeLists.txt", "README.md", "README.MD"]
sound_files_extensions = ["wav"]

def check_filename(file_name):
    pass

def check_directory(dir_name):
    pass
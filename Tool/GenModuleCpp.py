"""
This script generates a cpp file that includes all the headers of a module
to force the compilation of a static library for IDE's like Visual Studio that
refuses to compile a static library unless it has .cpp files. Useful for header-only
modules.

Usage:
- Place script at the module's root folder (ex: in ScoutCommon/).
- Run "python genModuleCpp.py"

You should now have a <moduleName>.cpp file in the <moduleName>/src/ folder that
includes all headers defined in <moduleName>/include/Scout/** .
"""

import os
from pathlib import Path

workingDir = os.getcwd()
headersDir = workingDir + "/Include"
srcDir = workingDir + "/Src"

if (not os.path.exists(headersDir)):
    print("Expected a headers directory at /Include but it does not exist.")
    quit()
if (not os.path.exists(srcDir)):
    os.makedirs(srcDir)

headers = list(Path(headersDir).rglob("*.[hH]"))
if (not headers):
    print("Could not recursively retireve any headers from /Include .")
    quit()

outputCppContents = "// This file is only here to force IDE's to compile a static library for header-only modules.\n"
for header in headers:
    outputCppContents += "#include <Scout/"
    outputCppContents += os.path.basename(header)
    outputCppContents += ">\n"

outputFile = open(srcDir + "/" + os.path.basename(workingDir) + ".cpp", "x")
outputFile.write(outputCppContents)
outputFile.close()

print("Script has completed execution successfully.")
quit()

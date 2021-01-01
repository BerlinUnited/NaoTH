# Patch evaluation

## Windows Installation
pip install cppyy fails if VS2017 is not installed. It fails silently if it is installed but
another (older) version is found in the windows path.

Until VS2015 you usually need to put something like this `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin` in the path
in order to use some features of Visual Studio. But this prevents cppyy from compiling the headers it needs.

The installation guide says you need to set the STDCXX variable to 11. But it does not make any difference for me [Benji]

## Usage

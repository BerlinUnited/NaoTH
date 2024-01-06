# Patch evaluation

## Windows Installation

pip install cppyy fails if VS2017 is not installed. It fails silently if it is installed but
another (older) version is found in the windows path.

Until VS2015 you usually need to put something like this `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin` in the path
in order to use some features of Visual Studio. But this prevents cppyy from compiling the headers it needs.

## MacOS Installation

For MacOS systems, it might be necessary to set the environment variable STDCXX="17"
BEFORE installing cppyy. Otherwise, you may encounter compile errors when including CPP
header files.If you want to be safe, run `export STDCXX="17"` before installing the cppyy
dependency.

```
export STDCXX="17"
pip install -e naoth
pip install cppyy opencv-python Pillow
```

## Usage

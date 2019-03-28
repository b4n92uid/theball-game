# The Ball Game by b4n92uid

TheBall is game in C++/OpenGL developped for educational purpose

## Author

- [BELDJOUHRI Abdelghani](contact@beldjouhri-abdelghani.com)

## Contributors

- skynet1994 : Portage linux
- mouradski : Portage linux
- edhelas : Portage linux
- terteur : Concept art
- Newin : Concept art

## Documentation

![Flowchart](./docs/theball-flowchart.png)

- [Level Script API](./docs/script.md)
- [Level Parameters](./docs/map.md)
- [FAQ](./docs/faq.md)

## Generating code documentation

```bash
cd doxygen
doxygen Doxyfile
```

## Compile

Download required libraries :

- Boost (v1.61)
  - DateTime
  - FileSystem
  - System
- DevIL (v1.7.8)
- FMOD (v4.44.32)
- Lua (v5.1.4)
- Newton Game Dynamics (v3.13)
- RocketLib (v1.3.0)
- SDL (v1.2.15)
- TheBall engine (dev-master)

Create build directory

```sh
mkdir build
cd build
```

Generate project file

```sh
cmake -G "Visual Studio 15 2017" ..
```

Setup flags for third-party library in `build/CMakeCache.txt`

```cmake
//Boost
THEBALL_BOOST_INC:PATH=D:/librairies/cpp/boost/boost_1_61_0

//Boost libs
THEBALL_BOOST_LIBS:FILEPATH=boost_system-vc140-mt-1_61.lib;boost_filesystem-vc140-mt-1_61.lib;boost_regex-vc140-mt-1_61.lib;boost_signals-vc140-mt-1_61.lib

//Boost lib dir
THEBALL_BOOST_LIB_DIR:PATH=D:/librairies/cpp/boost/boost_1_61_0/stage/lib

//Devil
THEBALL_DEVIL_INC:PATH=D:/librairies/cpp/devil/DevIL-SDK-x86-1.7.8/include

//Devil lib
THEBALL_DEVIL_LIB:FILEPATH=D:/librairies/cpp/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

//Fmod
THEBALL_FMOD_INC:PATH=C:/fmodex/api/inc

//Fmod lib
THEBALL_FMOD_LIB:FILEPATH=C:/fmodex/api/lib/fmodex_vc.lib

//Lua
THEBALL_LUA_INC:PATH=D:/librairies/cpp/lua/lua5_1_4_Win32_dll8_lib/include

//Lua lib
THEBALL_LUA_LIB:FILEPATH=D:/librairies/cpp/lua/lua5_1_4_Win32_dll8_lib/lua51.lib

//Newton
THEBALL_NEWTON_INC:PATH=D:/librairies/cpp/newton/newton-dynamics-newton-3.13a/sdk/dgNewton

//Newton lib
THEBALL_NEWTON_LIB:FILEPATH=D:/librairies/cpp/newton/newton-dynamics-newton-3.13a/sdk/projects/visualStudio_2015_dll/Win32/newton/Release/newton.lib

//Rocket
THEBALL_ROCKET_INC:PATH=D:/librairies/cpp/librocket/libRocket/Include

//Rocket lib
THEBALL_ROCKET_LIB:FILEPATH=RocketDebugger.lib;RocketControls.lib;RocketCore.lib

//Rocket lib dir
THEBALL_ROCKET_LIB_DIR:PATH=D:/librairies/cpp/librocket/libRocket/Build/msvc/Debug

//Sdl
THEBALL_SDL_INC:PATH=D:/librairies/cpp/sdl/SDL/SDL-1.2.15/include

//Sdl lib
THEBALL_SDL_LIB:FILEPATH=SDL.lib;SDLmain.lib

//Sdl lib dir
THEBALL_SDL_LIB_DIR:PATH=D:/librairies/cpp/sdl/SDL/SDL-1.2.15/lib/x86

//Tbengine
THEBALL_TBENGINE_INC:PATH=D:/projects/dev/theball/theball-engine/sources

//Tbengine plugins
THEBALL_TBENGINE_INC_PLUGINS:PATH=D:/projects/dev/theball/theball-engine/plugins

//Tbengine lib
THEBALL_TBENGINE_LIB:FILEPATH=D:/projects/dev/theball/theball-engine/build/Debug/tbengine.lib
```

Build !

```bash
MSBuild theball-game.vcxproj
```

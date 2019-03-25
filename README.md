# The Ball Game by b4n92uid

## Compile

Required librairies

- BOOST (v1.61)
- DEVIL (v1.7.8)
- FMOD (v4.44.32)
- FREETYPE (v2.3.5-1)
- LUA (v5.1.4)
- NEWTON (v3.13)
- ROCKET (v1.3.0)
- SDL (v1.2.15)
- TBENGINE (dev-master)

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

```
//Boost
THEBALL_BOOST_INC:PATH=D:/librairies/cpp/boost/boost_1_61_0

//Boost libs
THEBALL_BOOST_LIBS:FILEPATH=libboost_system-mgw53-1_61.a

//Boost lib dir
THEBALL_BOOST_LIB_DIR:PATH=D:/librairies/cpp/boost/boost_1_61_0/stage/lib

//Devil
THEBALL_DEVIL_INC:PATH=D:/librairies/cpp/devil/DevIL-SDK-x86-1.7.8/include

//Devil lib
THEBALL_DEVIL_LIB:FILEPATH=D:/librairies/cpp/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

//Fmod
THEBALL_FMOD_INC:PATH=C:/fmodex/api/inc

//Fmod lib
THEBALL_FMOD_LIB:FILEPATH=C:/fmodex/api/lib/libfmodex.a

//Freetype
THEBALL_FREETYPE_INC:PATH=D:/librairies/cpp/freetype/freetype-2.3.5-1-bin/include

//Freetype lib
THEBALL_FREETYPE_LIB:FILEPATH=D:/librairies/cpp/freetype/freetype-2.3.5-1-bin/lib/freetype.lib

//Lua
THEBALL_LUA_INC:PATH=D:/librairies/cpp/lua/lua5_1_4_Win32_dll8_lib/include

//Lua lib
THEBALL_LUA_LIB:FILEPATH=D:/librairies/cpp/lua/lua5_1_4_Win32_dll8_lib/lua51.lib

//Newton
THEBALL_NEWTON_INC:PATH=D:/librairies/cpp/newton/newton-dynamics-2.36/coreLibrary_200/source/newton

//Newton lib
THEBALL_NEWTON_LIB:FILEPATH=D:/librairies/cpp/newton/newton-dynamics-2.36/packages/Win32/debugDll/newton_d.lib

//Rocket
THEBALL_ROCKET_INC:PATH=D:/librairies/cpp/librocket/libRocket/Include

//Rocket lib
THEBALL_ROCKET_LIB:FILEPATH=libRocketControls.dll.a;libRocketCore.dll.a;libRocketDebugger.dll.a

//Rocket lib dir
THEBALL_ROCKET_LIB_DIR:PATH=D:/librairies/cpp/librocket/libRocket/Build/msys.debug

//Sdl
THEBALL_SDL_INC:PATH=D:/librairies/cpp/sdl/SDL/SDL-1.2.15/include

//Sdl lib
THEBALL_SDL_LIB:FILEPATH=SDL.lib;SDLmain.lib

//Sdl lib dir
THEBALL_SDL_LIB_DIR:PATH=D:/librairies/cpp/sdl/SDL/SDL-1.2.15/lib

//Tbengine
THEBALL_TBENGINE_INC:PATH=D:/projects/dev/theball/tbengine/sources

//Tbengine plugins
THEBALL_TBENGINE_INC_PLUGINS:PATH=D:/projects/dev/theball/tbengine/plugins

//Tbengine lib
THEBALL_TBENGINE_LIB:FILEPATH=D:/projects/dev/theball/tbengine/build/Debug/tbengine.lib
```

Start compilation with MSBUILD

```sh
cd ..
build.bat
```

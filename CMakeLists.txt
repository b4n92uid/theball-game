cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME theball-game)

project(${PROJECT_NAME})

set(THEBALL_TBENGINE_INC "" CACHE PATH "Tbengine")
set(THEBALL_TBENGINE_INC_PLUGINS "" CACHE PATH "Tbengine plugins")
set(THEBALL_DEVIL_INC "" CACHE PATH "Devil")
set(THEBALL_FREETYPE_INC "" CACHE PATH "Freetype")
set(THEBALL_SDL_INC "" CACHE PATH "Sdl")
set(THEBALL_NEWTON_INC "" CACHE PATH "Newton")
set(THEBALL_FMOD_INC "" CACHE PATH "Fmod")
set(THEBALL_BOOST_INC "" CACHE PATH "Boost")
set(THEBALL_LUA_INC "" CACHE PATH "Lua")
set(THEBALL_ROCKET_INC "" CACHE PATH "Rocket")

set(THEBALL_SDL_LIB_DIR "" CACHE PATH "Sdl lib dir")
set(THEBALL_BOOST_LIB_DIR "" CACHE PATH "Boost lib dir")
set(THEBALL_ROCKET_LIB_DIR "" CACHE PATH "Rocket lib dir")

set(THEBALL_TBENGINE_LIB "" CACHE FILEPATH "Tbengine lib")
set(THEBALL_DEVIL_LIB "" CACHE FILEPATH "Devil lib")
set(THEBALL_FREETYPE_LIB "" CACHE FILEPATH "Freetype lib")
set(THEBALL_SDL_LIB "" CACHE FILEPATH "Sdl lib")
set(THEBALL_NEWTON_LIB "" CACHE FILEPATH "Newton lib")
set(THEBALL_FMOD_LIB "" CACHE FILEPATH "Fmod lib")
set(THEBALL_BOOST_LIBS "" CACHE FILEPATH "Boost libs")
set(THEBALL_LUA_LIB "" CACHE FILEPATH "Lua lib")
set(THEBALL_ROCKET_LIB "" CACHE FILEPATH "Rocket lib")

include_directories(${THEBALL_TBENGINE_INC})
include_directories(${THEBALL_TBENGINE_INC_PLUGINS})

include_directories(${THEBALL_DEVIL_INC})
include_directories(${THEBALL_FREETYPE_INC})
include_directories(${THEBALL_SDL_INC})
include_directories(${THEBALL_NEWTON_INC})
include_directories(${THEBALL_FMOD_INC})
include_directories(${THEBALL_BOOST_INC})
include_directories(${THEBALL_LUA_INC})
include_directories(${THEBALL_ROCKET_INC})

link_directories(${THEBALL_SDL_LIB_DIR})
link_directories(${THEBALL_BOOST_LIB_DIR})
link_directories(${THEBALL_ROCKET_LIB_DIR})

file(GLOB_RECURSE THBALL_SOURCES sources/*)
file(GLOB_RECURSE SDLDEVICE_SOURCES ${THEBALL_TBENGINE_INC_PLUGINS}/SDLDevice/*)
file(GLOB_RECURSE NEWTONBALL_SOURCES ${THEBALL_TBENGINE_INC_PLUGINS}/NewtonBall/*)
file(GLOB_RECURSE ROCKETGUI_SOURCES ${THEBALL_TBENGINE_INC_PLUGINS}/RocketGuiManager/*)

add_executable(${PROJECT_NAME}
    ${ROCKETGUI_SOURCES}
    ${NEWTONBALL_SOURCES}
    ${SDLDEVICE_SOURCES}
    ${THBALL_SOURCES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ../debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ../release
)

target_link_libraries(${PROJECT_NAME}
    ${THEBALL_TBENGINE_LIB}
    ${THEBALL_BOOST_LIBS}
    ${THEBALL_DEVIL_LIB}
    ${THEBALL_SDL_LIB}
    ${THEBALL_ROCKET_LIB}
    ${THEBALL_FREETYPE_LIB}
    ${THEBALL_NEWTON_LIB}
    ${THEBALL_LUA_LIB}
    ${THEBALL_FMOD_LIB}
    glu32
    opengl32
)

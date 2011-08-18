project(theBall)
cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})

include_directories(${TBENGINE_INC})
include_directories(${TBENGINE_INC_PLUGINS})

include_directories(${DEVIL_INC})
include_directories(${FREETYPE_INC})

include_directories(${SDL_INC})
include_directories(${NEWTON_INC})
include_directories(${FMOD_INC})
include_directories(${BOOST_INC})
include_directories(${LUA_INC})

link_directories(${SDL_LIB_DIR})
link_directories(${BOOST_LIB_DIR})

file(GLOB_RECURSE THBALL_SOURCES sources/*)
file(GLOB_RECURSE SDLDEVICE_SOURCES ${TBENGINE_INC_PLUGINS}/SDLDevice/*)
file(GLOB_RECURSE NEWTONBALL_SOURCES ${TBENGINE_INC_PLUGINS}/NewtonBall/*)

add_executable(theball ${THBALL_SOURCES} ${SDLDEVICE_SOURCES} ${NEWTONBALL_SOURCES})

target_link_libraries(theball 
    ${TBENGINE_LIB} 
    ${DEVIL_LIB} 
    ${FREETYPE_LIB} 
    ${SDL_LIB} 
    ${NEWTON_LIB} 
    ${FMOD_LIB} 
    ${BOOST_LIBS} 
    ${LUA_LIB} 
    opengl32 glu32)

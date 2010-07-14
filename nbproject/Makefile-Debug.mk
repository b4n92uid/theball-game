#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/2087136692/Player.o \
	${OBJECTDIR}/sources/PlayTeamManager.o \
	${OBJECTDIR}/sources/PlayFragManager.o \
	${OBJECTDIR}/sources/Object.o \
	${OBJECTDIR}/sources/EditorEntity.o \
	${OBJECTDIR}/_ext/2087136692/Item.o \
	${OBJECTDIR}/sources/MaterialManager.o \
	${OBJECTDIR}/sources/Weapon.o \
	${OBJECTDIR}/sources/EditorLight.o \
	${OBJECTDIR}/sources/PlayAloneManager.o \
	${OBJECTDIR}/sources/AloneModeAi.o \
	${OBJECTDIR}/_ext/987314391/NewtonParallelScene.o \
	${OBJECTDIR}/sources/TeamModeAi.o \
	${OBJECTDIR}/_ext/1661411980/PlayManager.o \
	${OBJECTDIR}/sources/Settings.o \
	${OBJECTDIR}/sources/DynamicObject.o \
	${OBJECTDIR}/sources/SoundManager.o \
	${OBJECTDIR}/sources/FragModeAi.o \
	${OBJECTDIR}/_ext/1825732268/BldParser.o \
	${OBJECTDIR}/_ext/1661411980/EditorManager.o \
	${OBJECTDIR}/sources/Controller.o \
	${OBJECTDIR}/sources/GameManager.o \
	${OBJECTDIR}/_ext/2087136692/AppManager.o \
	${OBJECTDIR}/_ext/987314391/NewtonNode.o \
	${OBJECTDIR}/sources/StaticObject.o \
	${OBJECTDIR}/_ext/1825732268/AIControl.o \
	${OBJECTDIR}/_ext/1825732268/UserControl.o \
	${OBJECTDIR}/_ext/131439419/SDLDevice.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lmingw32 /D/projets/c++/tbe/dist/Debug/MinGW-Windows/libtbe.a /D/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib /D/Librairies/C++/Freetype/freetype-2.3.9/objs/.libs/libfreetype.a /D/Librairies/C++/Sdl/SDL/SDL-1.2.14/lib/libSDLmain.a /D/Librairies/C++/Sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a /D/Librairies/C++/Newton/NewtonWin-2.22/sdk/x32/dll_vs7/newton.lib /D/Librairies/C++/Fmod/fmodapi375win/api/lib/libfmod.a /D/Librairies/C++/Tinyxml/tinyxml/libtinyxml.a -lopengl32 -lglu32 build/ressource.o

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/MinGW-Windows/theball.exe

dist/Debug/MinGW-Windows/theball.exe: build/ressource.o

dist/Debug/MinGW-Windows/theball.exe: /D/projets/c++/tbe/dist/Debug/MinGW-Windows/libtbe.a

dist/Debug/MinGW-Windows/theball.exe: /D/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Freetype/freetype-2.3.9/objs/.libs/libfreetype.a

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Sdl/SDL/SDL-1.2.14/lib/libSDLmain.a

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Newton/NewtonWin-2.22/sdk/x32/dll_vs7/newton.lib

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Fmod/fmodapi375win/api/lib/libfmod.a

dist/Debug/MinGW-Windows/theball.exe: /D/Librairies/C++/Tinyxml/tinyxml/libtinyxml.a

dist/Debug/MinGW-Windows/theball.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/MinGW-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/theball ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/2087136692/Player.o: /D/Projets/C++/theball/sources/Player.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2087136692
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2087136692/Player.o /D/Projets/C++/theball/sources/Player.cpp

build/ressource.o: /D/Projets/C++/theball/sources/ressource.rc 
	${MKDIR} -p build
	@echo Compiling ressources...
	windres sources/ressource.rc build/ressource.o

${OBJECTDIR}/sources/PlayTeamManager.o: sources/PlayTeamManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/PlayTeamManager.o sources/PlayTeamManager.cpp

${OBJECTDIR}/sources/PlayFragManager.o: sources/PlayFragManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/PlayFragManager.o sources/PlayFragManager.cpp

${OBJECTDIR}/sources/Object.o: sources/Object.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Object.o sources/Object.cpp

${OBJECTDIR}/sources/EditorEntity.o: sources/EditorEntity.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/EditorEntity.o sources/EditorEntity.cpp

${OBJECTDIR}/_ext/2087136692/Item.o: /D/Projets/C++/theball/sources/Item.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2087136692
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2087136692/Item.o /D/Projets/C++/theball/sources/Item.cpp

${OBJECTDIR}/sources/MaterialManager.o: sources/MaterialManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/MaterialManager.o sources/MaterialManager.cpp

${OBJECTDIR}/sources/Weapon.o: sources/Weapon.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Weapon.o sources/Weapon.cpp

${OBJECTDIR}/sources/EditorLight.o: sources/EditorLight.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/EditorLight.o sources/EditorLight.cpp

${OBJECTDIR}/sources/PlayAloneManager.o: sources/PlayAloneManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/PlayAloneManager.o sources/PlayAloneManager.cpp

${OBJECTDIR}/sources/AloneModeAi.o: sources/AloneModeAi.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/AloneModeAi.o sources/AloneModeAi.cpp

${OBJECTDIR}/_ext/987314391/NewtonParallelScene.o: ../tbe/plugins/Newton/NewtonParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/987314391
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/987314391/NewtonParallelScene.o ../tbe/plugins/Newton/NewtonParallelScene.cpp

${OBJECTDIR}/sources/TeamModeAi.o: sources/TeamModeAi.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/TeamModeAi.o sources/TeamModeAi.cpp

${OBJECTDIR}/_ext/1661411980/PlayManager.o: /D/projets/c++/theball/sources/PlayManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1661411980
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1661411980/PlayManager.o /D/projets/c++/theball/sources/PlayManager.cpp

${OBJECTDIR}/sources/Settings.o: sources/Settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Settings.o sources/Settings.cpp

${OBJECTDIR}/sources/DynamicObject.o: sources/DynamicObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/DynamicObject.o sources/DynamicObject.cpp

${OBJECTDIR}/sources/SoundManager.o: sources/SoundManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/SoundManager.o sources/SoundManager.cpp

${OBJECTDIR}/sources/FragModeAi.o: sources/FragModeAi.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/FragModeAi.o sources/FragModeAi.cpp

${OBJECTDIR}/_ext/1825732268/BldParser.o: /D/projets/c++/theBall/sources/BldParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1825732268
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1825732268/BldParser.o /D/projets/c++/theBall/sources/BldParser.cpp

${OBJECTDIR}/_ext/1661411980/EditorManager.o: /D/projets/c++/theball/sources/EditorManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1661411980
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1661411980/EditorManager.o /D/projets/c++/theball/sources/EditorManager.cpp

${OBJECTDIR}/sources/Controller.o: sources/Controller.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Controller.o sources/Controller.cpp

${OBJECTDIR}/sources/GameManager.o: sources/GameManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/GameManager.o sources/GameManager.cpp

${OBJECTDIR}/_ext/2087136692/AppManager.o: /D/Projets/C++/theball/sources/AppManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2087136692
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2087136692/AppManager.o /D/Projets/C++/theball/sources/AppManager.cpp

${OBJECTDIR}/_ext/987314391/NewtonNode.o: ../tbe/plugins/Newton/NewtonNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/987314391
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/987314391/NewtonNode.o ../tbe/plugins/Newton/NewtonNode.cpp

${OBJECTDIR}/sources/StaticObject.o: sources/StaticObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/StaticObject.o sources/StaticObject.cpp

${OBJECTDIR}/_ext/1825732268/AIControl.o: /D/projets/c++/theBall/sources/AIControl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1825732268
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1825732268/AIControl.o /D/projets/c++/theBall/sources/AIControl.cpp

${OBJECTDIR}/_ext/1825732268/UserControl.o: /D/projets/c++/theBall/sources/UserControl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1825732268
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1825732268/UserControl.o /D/projets/c++/theBall/sources/UserControl.cpp

${OBJECTDIR}/_ext/131439419/SDLDevice.o: ../tbe/plugins/SDLDevice/SDLDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/131439419
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -DTIXML_USE_STL -DTHEBALL_COMPILE_DEBUG -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/Librairies/C++/Newton/NewtonWin-2.22/sdk -I/D/Librairies/C++/Fmod/fmodapi375win/api/inc -I/D/Librairies/C++/Tinyxml/tinyxml -I/D/projets/c++/tbe/sources -I/D/projets/c++/tbe/plugins -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/131439419/SDLDevice.o ../tbe/plugins/SDLDevice/SDLDevice.cpp

# Subprojects
.build-subprojects:
	cd D\:/projets/c++/tbe && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/MinGW-Windows/theball.exe
	${RM} build/ressource.o

# Subprojects
.clean-subprojects:
	cd D\:/projets/c++/tbe && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

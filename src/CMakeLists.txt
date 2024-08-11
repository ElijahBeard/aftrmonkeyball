cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)
PROJECT( "MonkeyMovement" ) #This command populates CMAKE_SIZEOF_VOID_P and should be one of the first commands in the file
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}" )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

#These are the files that will appear in the output project (ie, in MSVC, this will be in the vcxproj)
#All cpp files that ought to be compiled in this project must be included here
FILE( GLOB sources ${CMAKE_SOURCE_DIR}/*.cpp )
FILE( GLOB headers ${CMAKE_SOURCE_DIR}/*.h )
message( STATUS "HEADERS: ${headers}" ) 
message( STATUS "SOURCES: ${sources}" )

SET( aftrModuleCMakeHelperCmakeIncludePath "../../../include/cmake/aftrModuleCMakeHelper.cmake" )
include( ${aftrModuleCMakeHelperCmakeIncludePath} ) #sets ${AFTR_PATH_TO_CMAKE_SCRIPTS}, cmake_vars, and compiler_flags

SET( aftrModuleCommonProjectIncludesAndLibs "${AFTR_PATH_TO_CMAKE_SCRIPTS}/aftrModuleCommonProjectIncludesAndLibs.cmake" )
include( ${aftrModuleCommonProjectIncludesAndLibs} )

print_aftr_cmake_vars()

#Set CMAKE_CXX_FLAGS if any default flags need modification
#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings} ${cppFlags}" )
#SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnings}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "CMAKE_C_FLAGS  : ${CMAKE_C_FLAGS}" )

#Set any additional compiler flags -- by Default, usr/include/cmake/aftrModuleCMakeHelper.cmake sets AftrBurnerEngine-wide settings
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
   #The following two variables (warnings and cppFlags) are set inside aftrModuleCMakeHelper.cmake).
   #This is where GNU/Clang specific compiler flags specifically for building this module are located.
   ###SET( warnings "${warnings} -Wall -Wextra -myWarningFlags" )
   ###SET( cppFlags "${cppFlags} -fpermissive -std=c++11 -myGNUCompilerFlag" )    #GNU/Clang specific CPP compiler flags
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
   #The following two variables (warnings and cppFlags) are set inside aftrModuleCMakeHelper.cmake).
   #This is where MSVC specific compiler flags specifically for building the engine are located.
   ###SET( warnings "${warnings} /W4 /EHsc" )
   ###SET( warnings "${warnings} /wd4100 /myWarningFlag" ) #disable warning: 'identifier' : unreferenced formal parameter   
   ###SET( cppFlags "${cppFlags} /std:c++latest" ) #MSVC Specific CPP compiler flags, MP=Multi Processor compilations 
endif()

set(PHYSX_RELEASE_DIR "${AFTR_USR_INCLUDE_DIR}/../physx/physx/bin/win.x86_64.vc143.md/release/")
set(PHYSX_DEBUG_DIR "${AFTR_USR_INCLUDE_DIR}/../physx/physx/bin/win.x86_64.vc143.md/debug/")


file(GLOB PHYSX_RELEASE_LIBS "${PHYSX_RELEASE_DIR}/*.lib")
file(GLOB PHYSX_DEBUG_LIBS "${PHYSX_DEBUG_DIR}/*.lib")
IF( WIN32 ) #MSVC Compiler library paths
   TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
               optimized "${PHYSX_RELEASE_LIBS}" 
                    debug "${PHYSX_DEBUG_LIBS}"
               optimized "${AFTR_USERLAND_LIB_PATH}/irrKlang.lib"
                     debug "${AFTR_USERLAND_LIB_PATH}/irrKlang.lib"
               optimized "${AFTR_USERLAND_LIB_PATH}/portaudio_x64.lib"
                     debug "${AFTR_USERLAND_LIB_PATH}/portaudio_x64.lib"
               optimized "${AFTR_USERLAND_LIB_PATH}/rtmidi/Release/rtmidi.lib"
                     debug "${AFTR_USERLAND_LIB_PATH}/rtmidi/Debug/rtmidi.lib"
                #optimized "${CMAKE_SOURCE_DIR}/lib${AFTR_NBITS}/myLocalLib.lib" #Located in ../lib64/ or ../lib32/
                #    debug "${CMAKE_SOURCE_DIR}/lib${AFTR_NBITS}/myLocalLib.lib"
                         )
ENDIF()

#This section is already populated with default values from: ../../../include/cmake/aftrModuleCommonProjectIncludesAndLibs.cmake
IF( UNIX ) #Linux, Apple, *NIX platforms (see ../../../include/cmake/aftrModuleCommonProjectIncludesAndLibs.cmake for more details)
   TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
                #optimized "${AFTR_USERLAND_LIB_PATH}/mySampleLibraryNeededInThisModule.lib"
                #    debug "${AFTR_USERLAND_LIB_PATH}/mySampleLibraryNeededInThisModule_debug.lib"
                #optimized "${CMAKE_SOURCE_DIR}/lib${AFTR_NBITS}/myLocalLib.lib" #Located in ../lib64/ or ../lib32/
                #    debug "${CMAKE_SOURCE_DIR}/lib${AFTR_NBITS}/myLocalLib.lib"
                #          "mySystemInstalledlib" #called libMySystemInstalledlib.a (perhaps in /usr/lib64/)
                        )
ENDIF()

#This section is already populated with default values from: ../../../include/cmake/aftrModuleCommonProjectIncludesAndLibs.cmake
#This can be made WIN32 or UNIX specific, depending on the platform, if desired.
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PRIVATE 
                           "${AFTR_USR_INCLUDE_DIR}/../irrklang/include"
                           "${AFTR_USR_INCLUDE_DIR}/../physx/physx/include/"
                           "${AFTR_USR_INCLUDE_DIR}/../rtmidi-master/"
                           "${AFTR_USR_INCLUDE_DIR}/../portaudio/include/"
                           #"${CMAKE_SOURCE_DIR}/../my3rdPartyLib/include/"
                          )



SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings} ${cppFlags}" )  #These two lines should be removed sln 3 aug 2022
SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnings}" )                  #These two lines should be removed sln 3 aug 2022
MESSAGE( STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "CMAKE_C_FLAGS  : ${CMAKE_C_FLAGS}" )                          
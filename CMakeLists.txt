cmake_minimum_required(VERSION 3.7)
project(Path-Tracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF ()

IF (CMAKE_BUILD_TYPE STREQUAL Release)
    # Using OpenMP
    IF (NOT APPLE)
        FIND_PACKAGE(OpenMP)
        IF (OPENMP_FOUND)
            message("OPENMP FOUND.")
            ADD_DEFINITIONS(-DUSE_OPENMP)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
            set(CMAKE_SHARE_LINKER_FLAGS "${CMAKE_SHARE_LINKER_FLAGS} ${OpenMP_SHARE_LINKER_FLAGS}")
        ELSE ()
            message("OPENMP Not Found!")
        ENDIF ()
    ENDIF()
ENDIF ()

IF (WIN32)
    # Change to your include location
    set(INC_DIR E:\\SDK\\mingw\\include)
    include_directories(${INC_DIR})
    # Change to your lib location
    set(LINK_DIR E:\\SDK\\mingw\\lib)
    link_directories(${LINK_DIR})
elseif (APPLE)
elseif (UNIX)
endif ()


aux_source_directory(source/accelerators ACCELERATORS_FILES)
aux_source_directory(source/core CORE_FILES)
aux_source_directory(source/ext EXT_FILES)
aux_source_directory(source/integrators INTEGRATORS_FILES)
aux_source_directory(source/lights LIGHTS_FILES)
aux_source_directory(source/main MAIN_FILES)
aux_source_directory(source/materials MATERIALS_FILES)
aux_source_directory(source/samplers SAMPLERS_FILES)
aux_source_directory(source/shapes SHAPES_FILES)
aux_source_directory(source/textures TEXTURES_FILES)

set(SOURCE_FILES ${ACCELERATORS_FILES} ${CORE_FILES} ${EXT_FILES} ${INTEGRATORS_FILES}
        ${LIGHTS_FILES} ${MAIN_FILES} ${MATERIALS_FILES} ${SAMPLERS_FILES} ${SHAPES_FILES}
        ${TEXTURES_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
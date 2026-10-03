cmake_minimum_required(VERSION 3.16)

project(StormTrack LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "StormTrack is a Windows-only library (WinAPI/GDI).")
endif()

# Корень проекта = родитель папки, где лежит этот CMakeLists.txt
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(STORMTRACK_BUILD_ICON "Generate StormTrackIconData.hpp from res/ if possible" ON)
option(STORMTRACK_BUILD_DEMO "Build the StormTrack demo application" ON)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE STORMTRACK_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_ROOT}/src/GraphCore/*.cpp"
    "${PROJECT_ROOT}/src/GraphModules/*.cpp"
    "${PROJECT_ROOT}/src/RaiiWinApi/*.cpp"
    "${PROJECT_ROOT}/src/StormTrack/*.cpp"
)

file(GLOB_RECURSE STORMTRACK_HEADERS CONFIGURE_DEPENDS
    "${PROJECT_ROOT}/src/GraphCore/*.hpp"
    "${PROJECT_ROOT}/src/GraphModules/*.hpp"
    "${PROJECT_ROOT}/src/RaiiWinApi/*.hpp"
    "${PROJECT_ROOT}/src/StormTrack/*.hpp"
)

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------
add_library(StormTrack STATIC ${STORMTRACK_SOURCES} ${STORMTRACK_HEADERS})
add_library(StormTrack::StormTrack ALIAS StormTrack)

target_compile_features(StormTrack PUBLIC cxx_std_17)

target_include_directories(StormTrack
    PUBLIC
        "${PROJECT_ROOT}/src"
        "${PROJECT_ROOT}/src/GraphCore"
        "${PROJECT_ROOT}/src/GraphModules"
        "${PROJECT_ROOT}/src/GraphModules/BaseMath"
        "${PROJECT_ROOT}/src/GraphModules/Data"
        "${PROJECT_ROOT}/src/GraphModules/UI"
        "${PROJECT_ROOT}/src/GraphModules/UI/Ruler"
        "${PROJECT_ROOT}/src/RaiiWinApi"
        "${PROJECT_ROOT}/src/StormTrack"
)

target_compile_definitions(StormTrack
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
)

if(MSVC)
    target_compile_options(StormTrack PRIVATE /utf-8)
endif()

target_link_libraries(StormTrack PUBLIC user32 gdi32 kernel32)

# ---------------------------------------------------------------------------
# Optional: icon generation (ico2hpp.vbs)
# ---------------------------------------------------------------------------
set(STORMTRACK_ICON_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(STORMTRACK_ICON_DATA "${STORMTRACK_ICON_GENERATED_DIR}/StormTrackIconData.hpp")
set(STORMTRACK_ICON_SCRIPT "${PROJECT_ROOT}/res/ico2hpp.vbs")
set(STORMTRACK_ICON_FILE   "${PROJECT_ROOT}/res/stormtrack.ico")

find_program(CSCRIPT_EXECUTABLE cscript)

if(STORMTRACK_BUILD_ICON AND CSCRIPT_EXECUTABLE
   AND EXISTS "${STORMTRACK_ICON_SCRIPT}"
   AND EXISTS "${STORMTRACK_ICON_FILE}")

    add_custom_command(TARGET StormTrack PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${STORMTRACK_ICON_GENERATED_DIR}"
        COMMAND "${CSCRIPT_EXECUTABLE}" //nologo
                "${STORMTRACK_ICON_SCRIPT}"
                "${STORMTRACK_ICON_FILE}"
                "${STORMTRACK_ICON_GENERATED_DIR}"
        COMMENT "Generating StormTrackIconData.hpp"
        VERBATIM
    )

    target_include_directories(StormTrack PRIVATE "${STORMTRACK_ICON_GENERATED_DIR}")

else()
    message(STATUS "StormTrack: icon generation skipped.")
endif()

# ---------------------------------------------------------------------------
# Unified SDK output directory: <root>/bin/x64/<Config>/
# ---------------------------------------------------------------------------
set(STORMTRACK_DIST_DIR "${PROJECT_ROOT}/bin/x64/$<CONFIG>")

# ---------------------------------------------------------------------------
# Demo executable (optional)
# ---------------------------------------------------------------------------
if(STORMTRACK_BUILD_DEMO AND EXISTS "${PROJECT_ROOT}/src/main.cpp")

    add_executable(StormTrackDemo "${PROJECT_ROOT}/src/main.cpp")
    target_link_libraries(StormTrackDemo PRIVATE StormTrack::StormTrack)

    set_target_properties(StormTrackDemo PROPERTIES
        OUTPUT_NAME "StormTrackDemo"
        RUNTIME_OUTPUT_DIRECTORY "${STORMTRACK_DIST_DIR}"
    )

    if(MSVC)
        target_compile_options(StormTrackDemo PRIVATE /utf-8)
    endif()

    message(STATUS "StormTrack: demo enabled (StormTrackDemo).")
else()
    message(STATUS "StormTrack: demo disabled.")
endif()

# ---------------------------------------------------------------------------
# Post-build: copy .lib and public header next to the demo
# ---------------------------------------------------------------------------
add_custom_command(TARGET StormTrack POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STORMTRACK_DIST_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:StormTrack>"
            "${STORMTRACK_DIST_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${PROJECT_ROOT}/src/StormTrack/StormTrack.hpp"
            "${STORMTRACK_DIST_DIR}/"
    COMMENT "Copying StormTrack.lib and StormTrack.hpp -> bin/x64/$<CONFIG>/"
    VERBATIM
)
cmake_minimum_required(VERSION 3.22)

project(DisklordzDrumEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DISKLOORDZ_JUCE_DIR "" CACHE PATH "Existing JUCE source tree (skips a second clone)")
if(DISKLOORDZ_JUCE_DIR STREQUAL "")
    set(_dlz_juce_default "${CMAKE_CURRENT_SOURCE_DIR}/../../build/_deps/juce-src")
    if(EXISTS "${_dlz_juce_default}/CMakeLists.txt")
        set(DISKLOORDZ_JUCE_DIR "${_dlz_juce_default}")
    endif()
endif()

set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(NOT DISKLOORDZ_JUCE_DIR STREQUAL "" AND EXISTS "${DISKLOORDZ_JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${DISKLOORDZ_JUCE_DIR}" "${CMAKE_BINARY_DIR}/juce-build" EXCLUDE_FROM_ALL)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.15
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

function(dlz_configure_console target)
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_definitions(${target} PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(${target} PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_formats
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
endfunction()

juce_add_console_app(DrumBatchExporter PRODUCT_NAME "DrumBatchExporter")
target_sources(DrumBatchExporter PRIVATE src/DrumBatchExporter.cpp)
dlz_configure_console(DrumBatchExporter)

juce_add_console_app(DrumEngineTests PRODUCT_NAME "DrumEngineTests")
target_sources(DrumEngineTests PRIVATE src/DrumEngineTests.cpp)
dlz_configure_console(DrumEngineTests)

enable_testing()
add_test(NAME DrumEngineTests COMMAND DrumEngineTests)

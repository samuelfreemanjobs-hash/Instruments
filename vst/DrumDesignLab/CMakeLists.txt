cmake_minimum_required(VERSION 3.22)
project(DiskLordzDrumDesignLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DISKLORDZ_BUILD_JUCE_PLUGIN "Build JUCE VST3/Standalone" OFF)
set(JUCE_DIR "" CACHE PATH "Path to JUCE (contains CMakeLists.txt)")

add_library(DiskLordzDrumCore INTERFACE)
target_include_directories(DiskLordzDrumCore INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

enable_testing()

add_executable(ddl_m1_core_test Tests/m1_core_test.cpp)
target_link_libraries(ddl_m1_core_test PRIVATE DiskLordzDrumCore)
add_test(NAME ddl_m1_core_test COMMAND ddl_m1_core_test)

add_executable(DiskLordzBatchRenderer Tools/render_kit_batch.cpp)
target_link_libraries(DiskLordzBatchRenderer PRIVATE DiskLordzDrumCore)

if(MSVC)
    target_compile_options(ddl_m1_core_test PRIVATE /W3 /permissive- /fp:fast)
    target_compile_options(DiskLordzBatchRenderer PRIVATE /W3 /permissive- /fp:fast)
endif()

if(DISKLORDZ_BUILD_JUCE_PLUGIN)
    if(NOT JUCE_DIR)
        set(JUCE_DIR "C:/Users/user/Downloads/juce-9.0.2-windows/JUCE" CACHE PATH "" FORCE)
    endif()
    if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "JUCE not found at JUCE_DIR=${JUCE_DIR}")
    endif()
    add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/juce)

    juce_add_plugin(DrumDesignLab
        COMPANY_NAME "DiskLordz"
        PLUGIN_MANUFACTURER_CODE Inst
        PLUGIN_CODE DlDL
        FORMATS VST3 Standalone
        PRODUCT_NAME "DiskLordz Drum Design Lab"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
    )

    target_sources(DrumDesignLab PRIVATE
        Source/Plugin/PluginProcessor.cpp
        Source/Plugin/PluginEditor.cpp
    )
    target_include_directories(DrumDesignLab PRIVATE Source)
    target_link_libraries(DrumDesignLab
        PRIVATE
            DiskLordzDrumCore
            juce::juce_audio_utils
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags
    )
    target_compile_definitions(DrumDesignLab
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            JUCE_DISPLAY_SPLASH_SCREEN=0
    )
endif()

message(STATUS "DiskLordz DDL — JUCE plugin=${DISKLORDZ_BUILD_JUCE_PLUGIN}")

cmake_minimum_required(VERSION 3.22)

project(MeterTap VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)
set(FACTORY_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../factory-lib)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git clone --branch 7.0.12 https://github.com/juce-framework/JUCE.git vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

# Design-system LookAndFeel + components (theme selected below)
set(FACTORY_GUI_BUILD_GALLERY OFF CACHE BOOL "" FORCE)
set(FACTORY_GUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${FACTORY_LIB_DIR} factory_lib_build)

# Default theme for product builds; override with -DMETERTAP_THEME=studio-light for re-theme verify.
set(METERTAP_THEME "disklordz" CACHE STRING "Factory GUI theme id for MeterTap")

juce_add_plugin(MeterTap
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.metertap"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE DlMT
    FORMATS VST3 Standalone
    PRODUCT_NAME "MeterTap"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MeterTap
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_include_directories(MeterTap PRIVATE
    ${SHARED_DSP_DIR}
    ${FACTORY_LIB_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_compile_definitions(MeterTap
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

# Keep LTO off for faster Cloud/agent iteration (product ship still Release).
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MeterTap PROPERTIES ${_flag} FALSE)
endforeach()

target_link_libraries(MeterTap
    PRIVATE
        factory_gui
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

factory_gui_use_theme(MeterTap THEME ${METERTAP_THEME})

# Phase 4 — processor contract tests (JUCE console; links shared MeterTap code)
enable_testing()
juce_add_console_app(MeterTapProcessorTests PRODUCT_NAME "MeterTapProcessorTests")
target_sources(MeterTapProcessorTests PRIVATE tests/ProcessorTests.cpp)
target_include_directories(MeterTapProcessorTests PRIVATE
    ${SHARED_DSP_DIR}
    ${FACTORY_LIB_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(MeterTapProcessorTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MeterTapProcessorTests PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(MeterTapProcessorTests
    PRIVATE
        MeterTap
        juce::juce_audio_utils)
add_test(NAME MeterTapProcessorContracts COMMAND MeterTapProcessorTests)

# Editor offscreen render tests (design-spec pixels + both scale factors)
juce_add_console_app(MeterTapEditorRenderTests PRODUCT_NAME "MeterTapEditorRenderTests")
target_sources(MeterTapEditorRenderTests PRIVATE tests/EditorRenderTests.cpp)
target_include_directories(MeterTapEditorRenderTests PRIVATE
    ${SHARED_DSP_DIR}
    ${FACTORY_LIB_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(MeterTapEditorRenderTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        METERTAP_RENDER_OUT_DIR="${CMAKE_BINARY_DIR}/editor-renders")
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MeterTapEditorRenderTests PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(MeterTapEditorRenderTests
    PRIVATE
        MeterTap
        factory_gui
        juce::juce_audio_utils
        juce::juce_gui_basics
        juce::juce_graphics)
factory_gui_use_theme(MeterTapEditorRenderTests THEME ${METERTAP_THEME})
add_test(NAME MeterTapEditorRenderTests COMMAND MeterTapEditorRenderTests)

cmake_minimum_required(VERSION 3.22)

project(MeterTap VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)
set(FACTORY_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../factory-lib)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git clone --branch 7.0.12 https://github.com/juce-framework/JUCE.git vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

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
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MeterTap
    PRIVATE
        Source/PluginProcessor.cpp)

target_include_directories(MeterTap PRIVATE
    ${SHARED_DSP_DIR}
    ${FACTORY_LIB_DIR}/analysis)

target_compile_definitions(MeterTap
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MeterTap
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

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
        JUCE_USE_CURL=0)
target_link_libraries(MeterTapProcessorTests
    PRIVATE
        MeterTap
        juce::juce_audio_utils)
add_test(NAME MeterTapProcessorContracts COMMAND MeterTapProcessorTests)

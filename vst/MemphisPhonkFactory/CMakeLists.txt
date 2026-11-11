cmake_minimum_required(VERSION 3.22)

project(MemphisPhonkFactory VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)

# JUCE: git submodule at vst/JUCE (see vst/README.md)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(MemphisPhonkFactory
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.memphisphonkfactory"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE MEMP
    FORMATS VST3 Standalone
    PRODUCT_NAME "MemphisPhonkFactory"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

target_sources(MemphisPhonkFactory
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Dsp/MemphisPhonkEngine.cpp
        Source/Ui/PatternGridComponent.cpp)

target_include_directories(MemphisPhonkFactory PRIVATE
    ${SHARED_DSP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)

foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MemphisPhonkFactory PROPERTIES ${_flag} FALSE)
endforeach()

target_compile_definitions(MemphisPhonkFactory
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MemphisPhonkFactory
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

enable_testing()
juce_add_console_app(MemphisPhonkEngineTests PRODUCT_NAME "MemphisPhonkEngineTests")
target_sources(MemphisPhonkEngineTests PRIVATE tests/EngineTests.cpp)
target_include_directories(MemphisPhonkEngineTests PRIVATE
    ${SHARED_DSP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(MemphisPhonkEngineTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MemphisPhonkEngineTests PROPERTIES ${_flag} FALSE)
endforeach()
target_sources(MemphisPhonkEngineTests PRIVATE Source/Dsp/MemphisPhonkEngine.cpp)
target_link_libraries(MemphisPhonkEngineTests
    PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_core)
add_test(NAME MemphisPhonkEngineContracts COMMAND MemphisPhonkEngineTests)

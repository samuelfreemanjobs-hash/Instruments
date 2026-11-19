cmake_minimum_required(VERSION 3.22)

project(MemphisGrit VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git clone --branch 7.0.12 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(MemphisGrit
    COMPANY_NAME "DiskLordz"
    BUNDLE_ID "com.disklordz.memphisgrit"
    PLUGIN_MANUFACTURER_CODE Dlrz
    PLUGIN_CODE MgGr
    FORMATS VST3 Standalone
    PRODUCT_NAME "Memphis Grit"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MemphisGrit
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/MemphisGritDsp.cpp)

target_include_directories(MemphisGrit PRIVATE ${SHARED_DSP_DIR})

target_compile_definitions(MemphisGrit
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MemphisGrit
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

enable_testing()
juce_add_console_app(MemphisGritProcessorTests PRODUCT_NAME "MemphisGritProcessorTests")
target_sources(MemphisGritProcessorTests PRIVATE tests/ProcessorTests.cpp)
target_include_directories(MemphisGritProcessorTests PRIVATE
    ${SHARED_DSP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(MemphisGritProcessorTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(MemphisGritProcessorTests PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(MemphisGritProcessorTests
    PRIVATE
        MemphisGrit
        juce::juce_audio_utils)
add_test(NAME MemphisGritProcessorContracts COMMAND MemphisGritProcessorTests)

cmake_minimum_required(VERSION 3.22)

project(StemShield VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. Clone JUCE 8: git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(StemShield
    COMPANY_NAME "DiskLordz"
    BUNDLE_ID "com.disklordz.stemshield"
    PLUGIN_MANUFACTURER_CODE Dlrz
    PLUGIN_CODE StSh
    FORMATS VST3 Standalone
    PRODUCT_NAME "DiskLordz Stem-Shield"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(StemShield
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/StemShieldDsp.cpp)

target_include_directories(StemShield PRIVATE ${SHARED_DSP_DIR})

target_compile_definitions(StemShield
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(StemShield PROPERTIES ${_flag} FALSE)
endforeach()

target_link_libraries(StemShield
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

enable_testing()
juce_add_console_app(StemShieldProcessorTests PRODUCT_NAME "StemShieldProcessorTests")
target_sources(StemShieldProcessorTests PRIVATE tests/ProcessorTests.cpp)
target_include_directories(StemShieldProcessorTests PRIVATE ${SHARED_DSP_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(StemShieldProcessorTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(StemShieldProcessorTests PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(StemShieldProcessorTests
    PRIVATE
        StemShield
        juce::juce_audio_utils
        juce::juce_dsp)
add_test(NAME StemShieldProcessorContracts COMMAND StemShieldProcessorTests)

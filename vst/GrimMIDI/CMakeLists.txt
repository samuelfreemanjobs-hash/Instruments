cmake_minimum_required(VERSION 3.22)

project(GrimMIDI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. Clone JUCE 8: git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(GrimMIDI
    COMPANY_NAME "DiskLordz"
    BUNDLE_ID "com.disklordz.grimmidi"
    PLUGIN_MANUFACTURER_CODE Dlrz
    PLUGIN_CODE GrMi
    FORMATS VST3 Standalone
    PRODUCT_NAME "DiskLordz Grim-MIDI"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

option(GRIMMIDI_BUILD_EXPORT_PACK "Build offline MIDI pack exporter" OFF)

target_sources(GrimMIDI
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/GrimChordEngine.cpp
        Source/ProgressionMidiBuilder.cpp)

target_compile_definitions(GrimMIDI
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(GrimMIDI PROPERTIES ${_flag} FALSE)
endforeach()

target_link_libraries(GrimMIDI
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

enable_testing()
juce_add_console_app(GrimMIDIProcessorTests PRODUCT_NAME "GrimMIDIProcessorTests")
target_sources(GrimMIDIProcessorTests PRIVATE tests/ProcessorTests.cpp)
target_include_directories(GrimMIDIProcessorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(GrimMIDIProcessorTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(GrimMIDIProcessorTests PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(GrimMIDIProcessorTests
    PRIVATE
        GrimMIDI
        juce::juce_audio_utils)
add_test(NAME GrimMIDIProcessorContracts COMMAND GrimMIDIProcessorTests)

if(GRIMMIDI_BUILD_EXPORT_PACK)
  juce_add_console_app(GrimMIDIExportPack PRODUCT_NAME "GrimMIDIExportPack")
  target_sources(GrimMIDIExportPack PRIVATE tools/ExportMidiPack.cpp Source/GrimChordEngine.cpp
      Source/ProgressionMidiBuilder.cpp)
  target_include_directories(GrimMIDIExportPack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
  target_compile_definitions(GrimMIDIExportPack
      PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_DISPLAY_SPLASH_SCREEN=0)
  target_link_libraries(GrimMIDIExportPack PRIVATE juce::juce_audio_basics juce::juce_core
      juce::juce_events juce::juce_data_structures)
endif()

cmake_minimum_required(VERSION 3.22)

project(TrapLordSynth VERSION 0.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../JUCE_VERSION")
  file(READ "${CMAKE_CURRENT_LIST_DIR}/../../JUCE_VERSION" TRAP_LORD_JUCE_TAG)
  string(STRIP "${TRAP_LORD_JUCE_TAG}" TRAP_LORD_JUCE_TAG)
  if(TRAP_LORD_JUCE_TAG VERSION_LESS "8.0.0")
    set(TRAP_LORD_JUCE_TAG "8.0.15")
  endif()
else()
  set(TRAP_LORD_JUCE_TAG "8.0.15")
endif()

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG ${TRAP_LORD_JUCE_TAG}
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(TrapLordSynth
    COMPANY_NAME "DiskLordz Audio"
    BUNDLE_ID com.disklordz.audio.traplordsynth
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE DskL
    PLUGIN_CODE TrLp
    FORMATS VST3 Standalone
    PRODUCT_NAME "TRAP LORD SYNTH")

target_sources(TrapLordSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/SynthVoice.h)

target_compile_definitions(TrapLordSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(TrapLordSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

juce_generate_juce_header(TrapLordSynth)

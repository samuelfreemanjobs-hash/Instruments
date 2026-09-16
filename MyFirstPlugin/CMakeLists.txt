cmake_minimum_required(VERSION 3.22)

project(InstrumentsPlugins VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(READ "${CMAKE_CURRENT_LIST_DIR}/../JUCE_VERSION" JUCE_PINNED_VERSION)
string(STRIP "${JUCE_PINNED_VERSION}" JUCE_PINNED_VERSION)

option(MYFIRSTPLUGIN_COPY_AFTER_BUILD "Copy MyFirstPlugin VST3 to the OS plugin folder after build" OFF)
option(MYFIRSTSYNTH_COPY_AFTER_BUILD "Copy MyFirstSynth VST3 to the OS plugin folder after build" OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE
    GIT_TAG        ${JUCE_PINNED_VERSION}
)
FetchContent_MakeAvailable(JUCE)

set(_plugin_common_defs
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

# --- Effect: MyFirstPlugin (VST3) ---
juce_add_plugin(MyFirstPlugin
    COMPANY_NAME "MyCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE "MyCo"
    PLUGIN_CODE "Mfp1"
    FORMATS VST3 Standalone
    PRODUCT_NAME "MyFirstPlugin"
    COPY_PLUGIN_AFTER_BUILD ${MYFIRSTPLUGIN_COPY_AFTER_BUILD}
)

target_sources(MyFirstPlugin PRIVATE
    ParameterIds.h
    PluginProcessor.h
    PluginProcessor.cpp
    PluginEditor.h
    PluginEditor.cpp
)

target_link_libraries(MyFirstPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
)

target_compile_definitions(MyFirstPlugin PUBLIC ${_plugin_common_defs})
juce_generate_juce_header(MyFirstPlugin)

# --- Instrument: MyFirstSynth (VSTi) ---
juce_add_plugin(MyFirstSynth
    COMPANY_NAME "MyCompany"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE "MyCo"
    PLUGIN_CODE "Mfs1"
    FORMATS VST3 Standalone
    PRODUCT_NAME "MyFirstSynth"
    COPY_PLUGIN_AFTER_BUILD ${MYFIRSTSYNTH_COPY_AFTER_BUILD}
)

target_sources(MyFirstSynth PRIVATE
    SynthParameterIds.h
    SynthProcessor.h
    SynthProcessor.cpp
    SynthEditor.h
    SynthEditor.cpp
)

target_link_libraries(MyFirstSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
)

target_compile_definitions(MyFirstSynth PUBLIC ${_plugin_common_defs})
juce_generate_juce_header(MyFirstSynth)

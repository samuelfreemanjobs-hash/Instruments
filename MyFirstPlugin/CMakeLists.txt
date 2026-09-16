cmake_minimum_required(VERSION 3.22)

project(MyFirstPlugin VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MYFIRSTPLUGIN_COPY_AFTER_BUILD "Install VST3 to the OS plugin folder after build" OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE
    GIT_TAG        7.0.12
)
FetchContent_MakeAvailable(JUCE)

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

target_compile_definitions(MyFirstPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

juce_generate_juce_header(MyFirstPlugin)

cmake_minimum_required(VERSION 3.22)
project(OvernightDrumSampler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# NEVER copy into the user's live DAW folder on this machine.
option(COPY_PLUGIN_AFTER_BUILD "Copy VST3 after build" OFF)
set(COPY_PLUGIN_AFTER_BUILD OFF CACHE BOOL "" FORCE)

set(JUCE_DIR "" CACHE PATH "Path to JUCE")
if(NOT JUCE_DIR)
  set(JUCE_DIR "C:/Users/user/Downloads/juce-9.0.2-windows/JUCE" CACHE PATH "" FORCE)
endif()
if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}")
endif()

add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/juce)

juce_add_plugin(OvernightDrumSampler
  COMPANY_NAME "DiskLordz"
  PLUGIN_MANUFACTURER_CODE Dlrd
  PLUGIN_CODE Odrm
  FORMATS VST3 Standalone
  PRODUCT_NAME "DiskLordz Overnight Drum"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(OvernightDrumSampler PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp
)

target_compile_definitions(OvernightDrumSampler
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_include_directories(OvernightDrumSampler PRIVATE Source)

target_link_libraries(OvernightDrumSampler
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

message(STATUS "OvernightDrumSampler COPY_PLUGIN_AFTER_BUILD=${COPY_PLUGIN_AFTER_BUILD}")

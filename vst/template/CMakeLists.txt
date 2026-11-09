cmake_minimum_required(VERSION 3.22)

project(TemplateSynth VERSION 1.0.0)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)

# JUCE: prefer local vst/JUCE checkout; else FetchContent from repo-root JUCE_VERSION.
set(_juce_local "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
if(EXISTS "${_juce_local}/CMakeLists.txt")
  add_subdirectory("${_juce_local}" JUCE)
else()
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE_VERSION")
    message(FATAL_ERROR
      "JUCE not found. Clone the pin from /JUCE_VERSION into vst/JUCE "
      "(tools/build-vst.sh does this) or keep JUCE_VERSION at the repo root for FetchContent.")
  endif()
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE_VERSION" JUCE_PINNED_VERSION)
  string(STRIP "${JUCE_PINNED_VERSION}" JUCE_PINNED_VERSION)
  include(FetchContent)
  message(STATUS "template: FetchContent JUCE ${JUCE_PINNED_VERSION}")
  FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG ${JUCE_PINNED_VERSION}
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(TemplateSynth
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.templatesynth"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Tmpl
    FORMATS VST3 Standalone
    PRODUCT_NAME "Template Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

target_sources(TemplateSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_include_directories(TemplateSynth PRIVATE ${SHARED_DSP_DIR})

target_compile_definitions(TemplateSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(TemplateSynth
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

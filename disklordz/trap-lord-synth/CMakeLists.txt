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

juce_add_binary_data(TrapLordSynthFactoryData SOURCES assets/factory/preset_bank.json)

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
    Source/SynthVoice.h
    Source/SynthVoice.cpp
    Source/PresetLibrary.cpp
    Source/PresetLibrary.h
    Source/MpcXpmWriter.cpp
    Source/MpcXpmWriter.h
    Source/KeygroupExportWorker.cpp
    Source/KeygroupExportWorker.h
    Source/KeygroupExportOptions.h
    Source/OfflineAudition.cpp
    Source/OfflineAudition.h
    Source/PluginEditorFormatters.cpp
    Source/PluginEditorFormatters.h)

target_compile_definitions(TrapLordSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(TrapLordSynth PRIVATE
    TrapLordSynthFactoryData
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

juce_generate_juce_header(TrapLordSynth)

juce_add_console_app(TrapLordSynthRenderCLI PRODUCT_NAME "TrapLordSynthRender")
target_sources(TrapLordSynthRenderCLI PRIVATE tools/HeadlessRenderMain.cpp)
target_include_directories(TrapLordSynthRenderCLI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_BINARY_DIR}/TrapLordSynth_artefacts/JuceLibraryCode)
target_compile_definitions(TrapLordSynthRenderCLI
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_PLUGINHOST_VST=0
        JUCE_PLUGINHOST_VST3=0
        JUCE_PLUGINHOST_AU=0)
foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  set_target_properties(TrapLordSynthRenderCLI PROPERTIES ${_flag} FALSE)
endforeach()
target_link_libraries(TrapLordSynthRenderCLI
    PRIVATE
        TrapLordSynth
        TrapLordSynthFactoryData
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

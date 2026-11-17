cmake_minimum_required(VERSION 3.22)
project(DrumsmithNative VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(drumsmith_dsp INTERFACE)
target_include_directories(drumsmith_dsp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(drumsmith-render tools/render_cli.cpp)
target_link_libraries(drumsmith-render PRIVATE drumsmith_dsp)
enable_testing()
add_executable(drumsmith-dsp-test tests/test_engine.cpp)
target_link_libraries(drumsmith-dsp-test PRIVATE drumsmith_dsp)
add_test(NAME native_dsp COMMAND drumsmith-dsp-test)
option(DRUMSMITH_BUILD_JUCE "Build JUCE VST3 and standalone desktop application" OFF)
if(DRUMSMITH_BUILD_JUCE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
  else()
    find_package(JUCE CONFIG REQUIRED)
  endif()
  juce_add_plugin(DrumsmithPlugin
      COMPANY_NAME "DiskLordz Audio"
      BUNDLE_ID "com.disklordz.drumsmith"
      PLUGIN_MANUFACTURER_CODE Dklz
      PLUGIN_CODE Dsm2
      FORMATS VST3 Standalone
      PRODUCT_NAME "DRUMSMITH"
      IS_SYNTH TRUE
      NEEDS_MIDI_INPUT TRUE
      NEEDS_MIDI_OUTPUT FALSE
      EDITOR_WANTS_KEYBOARD_FOCUS FALSE)
  target_sources(DrumsmithPlugin PRIVATE
      juce/PluginProcessor.cpp juce/PluginEditor.cpp)
  target_include_directories(DrumsmithPlugin PRIVATE include juce)
  target_link_libraries(DrumsmithPlugin PRIVATE drumsmith_dsp
      juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp
      juce::juce_gui_extra)
  target_compile_definitions(DrumsmithPlugin PRIVATE
      JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
endif()

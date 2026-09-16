cmake_minimum_required(VERSION 3.22)

project(JDUpgraded VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

set(GENERATED_ROM_DIR "${CMAKE_BINARY_DIR}/generated")
set(GENERATED_ROM_FILE "${GENERATED_ROM_DIR}/jdupg_cleanroom.rom")

add_executable(GenerateCleanroomRom tools/GenerateCleanroomRom.cpp)
target_include_directories(GenerateCleanroomRom PRIVATE "${CMAKE_SOURCE_DIR}/Source")

add_custom_command(
    OUTPUT "${GENERATED_ROM_FILE}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${GENERATED_ROM_DIR}"
    COMMAND GenerateCleanroomRom "${GENERATED_ROM_FILE}"
    DEPENDS GenerateCleanroomRom
    COMMENT "Synthesizing cleanroom JDUPG ROM (128 waves)"
)

add_custom_target(JDUpgraded_GenerateRom DEPENDS "${GENERATED_ROM_FILE}")

juce_add_binary_data(JDUpgradedRomData SOURCES "${GENERATED_ROM_FILE}")
add_dependencies(JDUpgradedRomData JDUpgraded_GenerateRom)

juce_add_plugin(JDUpgraded
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.jdupgraded
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE JdUp
    FORMATS VST3 Standalone
    PRODUCT_NAME "JD Upgraded"
)

target_sources(JDUpgraded
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/VoicePool.cpp
        Source/DSP/Tone.cpp
        Source/Assets/RomBank.cpp
        Source/Assets/FactoryPatchLibrary.cpp
        Source/Preset/SysExParser.cpp
        Source/Preset/JdPatchSysexMapper.cpp
)

target_compile_definitions(JDUpgraded
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JDUPGRADED_CONTROL_RATE_DIVISOR=32
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx2" JDUPGRADED_HAS_AVX2_FLAG)
if(JDUPGRADED_HAS_AVX2_FLAG)
    target_compile_options(JDUpgraded PRIVATE -mavx2 -mfma)
    target_compile_definitions(JDUpgraded PRIVATE JDUPGRADED_USE_AVX2=1)
endif()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <arm_neon.h>
int main() {
    float32x4_t a = vdupq_n_f32(0.0f);
    float32x4_t b = vdupq_n_f32(1.0f);
    float32x4_t c = vaddq_f32(a, b);
    (void) c;
    return 0;
}
" JDUPGRADED_HAS_NEON)
if(JDUPGRADED_HAS_NEON AND NOT JDUPGRADED_HAS_AVX2_FLAG)
    target_compile_definitions(JDUpgraded PRIVATE JDUPGRADED_USE_NEON=1)
endif()

target_link_libraries(JDUpgraded
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        JDUpgradedRomData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(JDUpgraded
    PRIVATE
        Source
        "${CMAKE_BINARY_DIR}/juce_binarydata_JDUpgradedRomData/JuceLibraryCode"
)

add_executable(OfflineRender tools/OfflineRender.cpp)
target_link_libraries(OfflineRender
    PRIVATE
        JDUpgraded
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
)
target_include_directories(OfflineRender
    PRIVATE
        Source
        "${CMAKE_BINARY_DIR}/juce_binarydata_JDUpgradedRomData/JuceLibraryCode"
)
target_compile_definitions(OfflineRender
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)
add_dependencies(OfflineRender JDUpgradedRomData)

add_executable(ExportPreset tools/ExportPreset.cpp)
target_link_libraries(ExportPreset
    PRIVATE
        JDUpgraded
        juce::juce_audio_processors
        juce::juce_gui_extra
)
target_include_directories(ExportPreset
    PRIVATE
        Source
        "${CMAKE_BINARY_DIR}/juce_binarydata_JDUpgradedRomData/JuceLibraryCode"
)
target_compile_definitions(ExportPreset
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)
add_dependencies(ExportPreset JDUpgradedRomData)

add_executable(SpectralDiff tools/SpectralDiff.cpp)
target_link_libraries(SpectralDiff
    PRIVATE
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
)
target_compile_definitions(SpectralDiff
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

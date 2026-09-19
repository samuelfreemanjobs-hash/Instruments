# DISKLORDZ ROMPLER — sample-based rompler (standalone product, not WAVE-909).

set (DLRROM_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set (DLRROM_FILE "${DLRROM_GENERATED_DIR}/disklordz_factory.dlrrom")

add_custom_command (
    OUTPUT "${DLRROM_FILE}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLRROM_GENERATED_DIR}"
    COMMAND DisklordzSynth_BuildRawRom "${DLRROM_FILE}"
    DEPENDS DisklordzSynth_BuildRawRom
    COMMENT "Synthesizing Disklordz raw wave ROM DLRROM01 (DisklordzSynth engines)"
)

add_custom_target (DisklordzRompler_GenerateRom DEPENDS "${DLRROM_FILE}")

juce_add_binary_data (DisklordzRomplerPackData SOURCES "${DLRROM_FILE}")
add_dependencies (DisklordzRomplerPackData DisklordzRompler_GenerateRom)

juce_add_plugin (DisklordzRompler
    COMPANY_NAME "Disklordz"
    BUNDLE_ID com.disklordz.rompler
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Dlrz
    PLUGIN_CODE Dlrp
    FORMATS VST3 Standalone
    PRODUCT_NAME "DISKLORDZ ROMPLER"
)

target_sources (DisklordzRompler
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
)

target_compile_definitions (DisklordzRompler
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (DisklordzRompler
    PRIVATE
        DisklordzRomplerPackData
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (DisklordzRompler
    PRIVATE
        Source
        "${CMAKE_SOURCE_DIR}/DisklordzSynth/include"
        "${CMAKE_CURRENT_BINARY_DIR}/juce_binarydata_DisklordzRomplerPackData/JuceLibraryCode"
)

juce_generate_juce_header (DisklordzRompler)

add_executable (DisklordzRomplerTests Tests/RomplerEngineTests.cpp)
target_include_directories (DisklordzRomplerTests PRIVATE Source "${CMAKE_SOURCE_DIR}/DisklordzSynth/include")
target_compile_features (DisklordzRomplerTests PRIVATE cxx_std_20)
target_compile_definitions (DisklordzRomplerTests PRIVATE DLRROM_TEST_ROM="${DLRROM_FILE}")
add_dependencies (DisklordzRomplerTests DisklordzRompler_GenerateRom)

add_test (NAME DisklordzRomplerEngine COMMAND DisklordzRomplerTests)

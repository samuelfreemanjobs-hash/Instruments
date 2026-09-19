# DISKLORDZ ROMPLER — sample-based rompler (standalone product, not WAVE-909).

set (DLRROM_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

set (DLRROM_BANK_OUTPUTS "")
foreach (BANK RANGE 0 15)
    if (BANK LESS 10)
        set (DLRROM_BANK_SUFFIX "0${BANK}")
    else()
        set (DLRROM_BANK_SUFFIX "${BANK}")
    endif()
    list (APPEND DLRROM_BANK_OUTPUTS "${DLRROM_GENERATED_DIR}/pcm_bank_${DLRROM_BANK_SUFFIX}.dlrrom")
endforeach()

add_custom_command (
    OUTPUT ${DLRROM_BANK_OUTPUTS}
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLRROM_GENERATED_DIR}"
    COMMAND DisklordzSynth_BuildRawRom "${DLRROM_GENERATED_DIR}"
    DEPENDS DisklordzSynth_BuildRawRom
    COMMENT "Synthesizing 16 factory PCM expansion ROMs (DLRROM01)"
)

add_custom_target (DisklordzRompler_GenerateRom DEPENDS ${DLRROM_BANK_OUTPUTS})

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
        Source/Assets/FactoryRomLoader.cpp
)

target_compile_definitions (DisklordzRompler
    PRIVATE
        DISKLORDZ_FACTORY_ROM_DIR="${DLRROM_GENERATED_DIR}"
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (DisklordzRompler
    PRIVATE
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
)

juce_generate_juce_header (DisklordzRompler)

function (disklordz_rompler_install_pcm_roms target_name)
    add_custom_command (
        TARGET ${target_name} PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target_name}>/Resources/DisklordzRom"
        COMMENT "Ensure PCM ROM resource folder for ${target_name}"
    )
    foreach (ROMFILE IN LISTS DLRROM_BANK_OUTPUTS)
        add_custom_command (
            TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ROMFILE}"
                    "$<TARGET_FILE_DIR:${target_name}>/Resources/DisklordzRom/"
            COMMENT "Install ${ROMFILE}"
        )
    endforeach()
    add_dependencies (${target_name} DisklordzRompler_GenerateRom)
endfunction()

disklordz_rompler_install_pcm_roms (DisklordzRompler_VST3)
disklordz_rompler_install_pcm_roms (DisklordzRompler_Standalone)

add_executable (DisklordzRomplerTests Tests/RomplerEngineTests.cpp)
target_include_directories (DisklordzRomplerTests PRIVATE Source "${CMAKE_SOURCE_DIR}/DisklordzSynth/include")
target_compile_features (DisklordzRomplerTests PRIVATE cxx_std_20)
target_compile_definitions (DisklordzRomplerTests PRIVATE DLRROM_TEST_ROM_DIR="${DLRROM_GENERATED_DIR}")
add_dependencies (DisklordzRomplerTests DisklordzRompler_GenerateRom)

add_test (NAME DisklordzRomplerEngine COMMAND DisklordzRomplerTests)

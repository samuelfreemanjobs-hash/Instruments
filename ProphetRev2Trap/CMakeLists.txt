# Night Circuit — Prophet Rev2–inspired trap / vaporwave poly synth (JUCE subdirectory)

juce_add_plugin (ProphetRev2Trap
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.prophetrev2trap
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Pr2T
    FORMATS VST3 Standalone
    PRODUCT_NAME "Night Circuit"
)

target_sources (ProphetRev2Trap
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
        Source/Presets/PresetManager.cpp
)

target_compile_definitions (ProphetRev2Trap
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (ProphetRev2Trap
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (ProphetRev2Trap PRIVATE Source)

juce_generate_juce_header (ProphetRev2Trap)

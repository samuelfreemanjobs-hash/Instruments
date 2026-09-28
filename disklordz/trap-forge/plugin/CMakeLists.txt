juce_add_plugin (TrapForge
    COMPANY_NAME "Disklordz"
    BUNDLE_ID com.disklordz.trapforge
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Dskz
    PLUGIN_CODE TrFg
    FORMATS VST3 Standalone
    PRODUCT_NAME "TRAP-FORGE"
)

target_sources (TrapForge
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/TrapForgePreset.cpp
        Source/DrumRender.cpp
)

target_compile_definitions (TrapForge
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (TrapForge
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (TrapForge PRIVATE Source)

juce_generate_juce_header (TrapForge)

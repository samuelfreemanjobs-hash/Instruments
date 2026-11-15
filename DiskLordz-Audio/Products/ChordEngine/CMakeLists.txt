juce_add_plugin(ChordEngine
    COMPANY_NAME "DiskLordz Audio"
    BUNDLE_ID com.disklordz.audio.chordengine
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE DskL
    PLUGIN_CODE ChE1
    FORMATS VST3 Standalone
    PRODUCT_NAME "CHORD ENGINE"
    COPY_PLUGIN_AFTER_BUILD OFF
)

target_sources(ChordEngine PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_link_libraries(ChordEngine PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
)

target_compile_definitions(ChordEngine PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_generate_juce_header(ChordEngine)

pf_add_juce_plugin(
    TARGET ReferenceSynth
    PRODUCT_NAME "Reference Synth"
    PLUGIN_CODE RefS
    MANUFACTURER_CODE Inst
    BUNDLE_ID com.instruments.reference-synth
    IS_SYNTH TRUE
    SOURCES
        SynthParameterIds.h
        SynthProcessor.h
        SynthProcessor.cpp
        SynthEditor.h
        SynthEditor.cpp
)

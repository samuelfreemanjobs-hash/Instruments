pf_add_juce_plugin(
    TARGET MoogVoyager
    PRODUCT_NAME "V Voyager"
    PLUGIN_CODE OfqX
    MANUFACTURER_CODE Inst
    BUNDLE_ID com.instruments.moog-voyager
    IS_SYNTH TRUE
    SOURCES
        SynthParameterIds.h
        SynthProcessor.h
        SynthProcessor.cpp
        SynthEditor.h
        SynthEditor.cpp
)

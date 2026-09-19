pf_add_juce_plugin(
    TARGET ReferenceEffect
    PRODUCT_NAME "Reference Effect"
    PLUGIN_CODE RefE
    MANUFACTURER_CODE Inst
    BUNDLE_ID com.instruments.reference-effect
    IS_SYNTH FALSE
    SOURCES
        ParameterIds.h
        PluginProcessor.h
        PluginProcessor.cpp
        PluginEditor.h
        PluginEditor.cpp
)

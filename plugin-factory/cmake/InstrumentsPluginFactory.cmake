# Instruments Plugin Factory — shared CMake helpers for JUCE VST3 targets.

function(pf_add_juce_plugin)
    set(options "")
    set(oneValueArgs TARGET PRODUCT_NAME PLUGIN_CODE MANUFACTURER_CODE BUNDLE_ID IS_SYNTH COPY_AFTER_BUILD)
    set(multiValueArgs SOURCES FORMATS)
    cmake_parse_arguments(PF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT PF_TARGET)
        message(FATAL_ERROR "pf_add_juce_plugin: TARGET is required")
    endif()

    if(NOT PF_PRODUCT_NAME)
        set(PF_PRODUCT_NAME "${PF_TARGET}")
    endif()

    if(NOT PF_PLUGIN_CODE)
        message(FATAL_ERROR "pf_add_juce_plugin: PLUGIN_CODE is required (four characters, one uppercase)")
    endif()

    if(NOT PF_MANUFACTURER_CODE)
        set(PF_MANUFACTURER_CODE "Inst")
    endif()

    if(NOT PF_FORMATS)
        set(PF_FORMATS VST3 Standalone)
    endif()

    if(PF_IS_SYNTH)
        set(_is_synth TRUE)
        set(_needs_midi TRUE)
    else()
        set(_is_synth FALSE)
        set(_needs_midi FALSE)
    endif()

    if(NOT DEFINED PF_COPY_AFTER_BUILD)
        set(PF_COPY_AFTER_BUILD OFF)
    endif()

    set(_bundle_args "")
    if(PF_BUNDLE_ID)
        list(APPEND _bundle_args BUNDLE_ID "${PF_BUNDLE_ID}")
    endif()

    juce_add_plugin(${PF_TARGET}
        COMPANY_NAME "Instruments"
        ${_bundle_args}
        IS_SYNTH ${_is_synth}
        NEEDS_MIDI_INPUT ${_needs_midi}
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD ${PF_COPY_AFTER_BUILD}
        PLUGIN_MANUFACTURER_CODE ${PF_MANUFACTURER_CODE}
        PLUGIN_CODE ${PF_PLUGIN_CODE}
        FORMATS ${PF_FORMATS}
        PRODUCT_NAME "${PF_PRODUCT_NAME}"
    )

    target_sources(${PF_TARGET} PRIVATE ${PF_SOURCES})

    target_link_libraries(${PF_TARGET}
        PRIVATE
            juce::juce_audio_utils
            juce::juce_audio_processors
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags
    )

    target_compile_definitions(${PF_TARGET}
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
    )

    juce_generate_juce_header(${PF_TARGET})
endfunction()

function(pf_load_plugin_registry registry_file out_var)
    if(NOT EXISTS "${registry_file}")
        message(FATAL_ERROR "Plugin registry not found: ${registry_file}")
    endif()

    file(STRINGS "${registry_file}" _lines)
    set(_dirs "")
    foreach(_line IN LISTS _lines)
        string(STRIP "${_line}" _line)
        if(_line STREQUAL "")
            continue()
        endif()
        if(_line MATCHES "^#")
            continue()
        endif()
        list(APPEND _dirs "${_line}")
    endforeach()
    set(${out_var} "${_dirs}" PARENT_SCOPE)
endfunction()

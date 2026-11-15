cmake_minimum_required(VERSION 3.22)

set(DL_UI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(dl_ui STATIC
    src/UiLayout.cpp
    src/SkinAssetCache.cpp
    src/SkinnableLookAndFeel.cpp
    src/OnScreenKeyboardStrip.cpp
    src/LayoutEditorShell.cpp
)

target_include_directories(dl_ui
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(dl_ui PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_features(dl_ui PUBLIC cxx_std_20)

target_compile_definitions(dl_ui
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(dl_ui
    PUBLIC
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_audio_utils
)

function(dl_ui_attach_skin product_target)
    cmake_parse_arguments(SKIN "" "TARGET;SKIN" "" ${ARGN})
    if(NOT SKIN_TARGET OR NOT SKIN_SKIN)
        message(FATAL_ERROR "dl_ui_attach_skin requires TARGET and SKIN")
    endif()

    set(_skin_dir "${CMAKE_SOURCE_DIR}/shared/disklordz_ui/skins/${SKIN_SKIN}")
    if(NOT EXISTS "${_skin_dir}/layout.json")
        message(FATAL_ERROR "Missing ${_skin_dir}/layout.json")
    endif()

    set(_asset_files "${_skin_dir}/layout.json")
    if(EXISTS "${_skin_dir}/assets")
        file(GLOB _extras CONFIGURE_DEPENDS "${_skin_dir}/assets/*")
        list(APPEND _asset_files ${_extras})
    endif()

  # Skin binary blob target (per product).
    juce_add_binary_data(${SKIN_TARGET}
        NAMESPACE JDUpgradedSkin
        HEADER_NAME JDUpgradedSkinBinaryData.h
        SOURCES ${_asset_files})
    target_link_libraries(${product_target} PRIVATE ${SKIN_TARGET} dl_ui)
    target_compile_definitions(${product_target} PRIVATE DL_UI_SKIN_ID="${SKIN_SKIN}")
endfunction()
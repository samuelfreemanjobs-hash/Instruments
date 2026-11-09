cmake_minimum_required(VERSION 3.22)

# Standalone configure: cmake -B build-factory-gui -S vst/factory-lib \
#   -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 \
#   -DFACTORY_GUI_THEME=disklordz
#
# Or include from a parent that already provides JUCE:
#   add_subdirectory(vst/factory-lib)

project(FactoryGui VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FACTORY_GUI_BUILD_GALLERY "Build FactoryGuiGallery standalone" ON)
option(FACTORY_GUI_BUILD_TESTS "Build FactoryGuiRenderTests" ON)
option(FACTORY_GUI_USE_SYSTEM_FONTS "Skip vendored OFL TTFs; use JUCE defaults" OFF)
set(FACTORY_GUI_THEME "disklordz" CACHE STRING "Theme id for gallery/tests (disklordz|_template|studio-light)")

# Pin JUCE for standalone configures. Defaults to repo-root JUCE_VERSION (7.0.12).
# Override: -DFACTORY_GUI_JUCE_TAG=8.0.15  (do NOT silently reuse build/_deps).
set(_juce_pin_file "${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE_VERSION")
if(EXISTS "${_juce_pin_file}")
  file(READ "${_juce_pin_file}" _juce_pin_default)
  string(STRIP "${_juce_pin_default}" _juce_pin_default)
else()
  set(_juce_pin_default "7.0.12")
endif()
set(FACTORY_GUI_JUCE_TAG "${_juce_pin_default}" CACHE STRING "JUCE git tag for FetchContent when not provided by parent")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FactoryGui.cmake")

# ---------------------------------------------------------------------------
# JUCE — reuse parent target, optional submodule, else FetchContent(FACTORY_GUI_JUCE_TAG)
# Never auto-pick monorepo build/_deps (that tree may be a different major than JUCE_VERSION).
# ---------------------------------------------------------------------------
if(NOT TARGET juce::juce_gui_basics)
  set(_juce_submodule "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
  if(EXISTS "${_juce_submodule}/CMakeLists.txt")
    message(STATUS "factory_gui: using vst/JUCE submodule")
    add_subdirectory("${_juce_submodule}" "${CMAKE_BINARY_DIR}/juce_submodule")
  else()
    include(FetchContent)
    message(STATUS "factory_gui: FetchContent JUCE ${FACTORY_GUI_JUCE_TAG}")
    FetchContent_Declare(
      JUCE
      GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
      GIT_TAG ${FACTORY_GUI_JUCE_TAG}
      GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
  endif()
endif()

# ---------------------------------------------------------------------------
# factory_gui — INTERFACE sources + includes
# ---------------------------------------------------------------------------
set(_fg "${CMAKE_CURRENT_SOURCE_DIR}")
set(FACTORY_GUI_SOURCES
  "${_fg}/gui/FactoryLookAndFeel.cpp"
  "${_fg}/gui/components/RotaryKnob.cpp"
  "${_fg}/gui/components/LedToggle.cpp"
  "${_fg}/gui/components/ValueReadout.cpp"
  "${_fg}/gui/components/LevelMeter.cpp"
  "${_fg}/gui/components/MeterScale.cpp"
  "${_fg}/gui/components/SectionPanel.cpp"
  "${_fg}/gui/components/HeaderBar.cpp"
  "${_fg}/gui/components/PresetBrowser.cpp"
)

add_library(factory_gui INTERFACE)
target_include_directories(factory_gui INTERFACE
  "${_fg}/gui"
  "${_fg}/gui/components"
)
target_sources(factory_gui INTERFACE ${FACTORY_GUI_SOURCES})
target_compile_features(factory_gui INTERFACE cxx_std_20)
target_link_libraries(factory_gui INTERFACE
  juce::juce_gui_basics
  juce::juce_graphics
  juce::juce_core
  juce::juce_events
  juce::juce_data_structures
)

# ---------------------------------------------------------------------------
# Gallery (Standalone GUI app) — theme selected at configure time
# ---------------------------------------------------------------------------
if(FACTORY_GUI_BUILD_GALLERY)
  juce_add_gui_app(FactoryGuiGallery
    PRODUCT_NAME "Factory GUI Gallery"
    COMPANY_NAME "DiskLordz"
    BUNDLE_ID "com.disklordz.factory-gui-gallery"
    NEEDS_CURL FALSE
    NEEDS_WEB_BROWSER FALSE
  )
  target_sources(FactoryGuiGallery PRIVATE gui/gallery/FactoryGuiGallery.cpp)
  target_compile_definitions(FactoryGuiGallery PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
  )
  # Keep LTO off for gallery/tests (task: save time)
  foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
    set_target_properties(FactoryGuiGallery PROPERTIES ${_flag} FALSE)
  endforeach()
  target_link_libraries(FactoryGuiGallery PRIVATE
    factory_gui
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
  )
  factory_gui_use_theme(FactoryGuiGallery THEME ${FACTORY_GUI_THEME})
endif()

# ---------------------------------------------------------------------------
# Render tests (console app + ctest) — one binary per theme
# ---------------------------------------------------------------------------
if(FACTORY_GUI_BUILD_TESTS)
  enable_testing()
  set(_factory_gui_test_themes disklordz _template studio-light)
  foreach(_theme IN LISTS _factory_gui_test_themes)
    string(REPLACE "-" "_" _safe "${_theme}")
    string(REPLACE "." "_" _safe "${_safe}")
    set(_tgt "FactoryGuiRenderTests_${_safe}")

    juce_add_console_app(${_tgt}
      PRODUCT_NAME "FactoryGuiRenderTests_${_safe}"
      COMPANY_NAME "DiskLordz"
      NEEDS_CURL FALSE
      NEEDS_WEB_BROWSER FALSE
    )
    target_sources(${_tgt} PRIVATE "${_fg}/gui/tests/FactoryGuiRenderTests.cpp")
    target_compile_definitions(${_tgt} PRIVATE
      JUCE_WEB_BROWSER=0
      JUCE_USE_CURL=0
      JUCE_DISPLAY_SPLASH_SCREEN=0
      FACTORY_GUI_RENDER_OUT_DIR="${CMAKE_BINARY_DIR}/gui-renders"
    )
    foreach(_flag IN ITEMS INTERPROCEDURAL_OPTIMIZATION INTERPROCEDURAL_OPTIMIZATION_RELEASE)
      set_target_properties(${_tgt} PROPERTIES ${_flag} FALSE)
    endforeach()
    target_link_libraries(${_tgt} PRIVATE
      factory_gui
      juce::juce_gui_basics
      juce::juce_graphics
      juce::juce_events
      juce::juce_recommended_config_flags
      juce::juce_recommended_warning_flags
    )
    factory_gui_use_theme(${_tgt} THEME ${_theme})
    add_test(NAME FactoryGuiRenderTests_${_safe} COMMAND ${_tgt})
  endforeach()

  # Convenience alias target builds all theme test binaries
  add_custom_target(FactoryGuiRenderTests
    DEPENDS FactoryGuiRenderTests_disklordz FactoryGuiRenderTests__template FactoryGuiRenderTests_studio_light)
endif()

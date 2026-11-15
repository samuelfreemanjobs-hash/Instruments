# Factory GUI helpers — theme include + font binary data.
#
# Usage from a product CMakeLists that already has JUCE:
#   add_subdirectory(path/to/vst/factory-lib)
#   factory_gui_use_theme(MyPlugin THEME disklordz)
#   target_link_libraries(MyPlugin PRIVATE factory_gui)

set(FACTORY_GUI_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." CACHE INTERNAL "")

function(factory_gui_theme_font_files theme_id out_var)
  if(theme_id STREQUAL "studio-light")
    set(_families IBMPlexSans IBMPlexMono)
    set(_sans IBMPlexSans)
    set(_mono IBMPlexMono)
  else()
    # disklordz, _template, and any Inter/JetBrains theme
    set(_families Inter JetBrainsMono)
    set(_sans Inter)
    set(_mono JetBrainsMono)
  endif()

  set(_files)
  foreach(_fam IN LISTS _families)
    foreach(_weight Regular Medium SemiBold Bold)
      set(_f "${FACTORY_GUI_ROOT}/gui/fonts/${_fam}/${_fam}-${_weight}.ttf")
      if(EXISTS "${_f}")
        list(APPEND _files "${_f}")
      endif()
    endforeach()
  endforeach()
  set(${out_var} "${_files}" PARENT_SCOPE)
endfunction()

function(factory_gui_use_theme target_name)
  cmake_parse_arguments(FG "" "THEME" "" ${ARGN})
  if(NOT FG_THEME)
    message(FATAL_ERROR "factory_gui_use_theme(${target_name}): THEME <id> is required")
  endif()

  set(_gen_dir "${FACTORY_GUI_ROOT}/gui/generated/${FG_THEME}")
  if(NOT EXISTS "${_gen_dir}/DesignTokens.h")
    message(FATAL_ERROR
      "factory_gui_use_theme: missing ${_gen_dir}/DesignTokens.h — "
      "run python3 vst/factory-lib/design-system/scripts/gen_juce_tokens.py --theme ${FG_THEME}")
  endif()

  target_include_directories(${target_name} PRIVATE "${_gen_dir}")
  target_compile_definitions(${target_name} PRIVATE
    FACTORY_GUI_THEME_ID="${FG_THEME}"
  )

  if(FACTORY_GUI_USE_SYSTEM_FONTS)
    target_compile_definitions(${target_name} PRIVATE FACTORY_GUI_USE_SYSTEM_FONTS=1)
    return()
  endif()

  factory_gui_theme_font_files(${FG_THEME} _font_files)
  if(NOT _font_files)
    message(WARNING
      "factory_gui_use_theme: no TTFs for theme '${FG_THEME}'; "
      "falling back to system fonts (FACTORY_GUI_USE_SYSTEM_FONTS)")
    target_compile_definitions(${target_name} PRIVATE FACTORY_GUI_USE_SYSTEM_FONTS=1)
    return()
  endif()

  # Sanitize theme id for a CMake target name
  string(REPLACE "-" "_" _safe_theme "${FG_THEME}")
  string(REPLACE "." "_" _safe_theme "${_safe_theme}")
  set(_bd_target "factory_gui_fonts_${_safe_theme}")

  if(NOT TARGET ${_bd_target})
    juce_add_binary_data(${_bd_target}
      HEADER_NAME "FactoryGuiFonts.h"
      NAMESPACE FactoryGuiFonts
      SOURCES ${_font_files}
    )
  endif()

  target_link_libraries(${target_name} PRIVATE ${_bd_target})
endfunction()

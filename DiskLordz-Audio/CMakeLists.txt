cmake_minimum_required(VERSION 3.22)

# JUCE 8 juceaide needs a C compiler in the project languages list.
project(DiskLordzAudio VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../JUCE_VERSION")
  message(FATAL_ERROR "Missing ../JUCE_VERSION — pin the monorepo JUCE tag there (single source of truth).")
endif()
file(READ "${CMAKE_CURRENT_LIST_DIR}/../JUCE_VERSION" JUCE_PINNED_VERSION)
string(STRIP "${JUCE_PINNED_VERSION}" JUCE_PINNED_VERSION)
message(STATUS "DiskLordz-Audio JUCE pin: ${JUCE_PINNED_VERSION}")

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE
  GIT_TAG ${JUCE_PINNED_VERSION}
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

add_subdirectory(Products/ChordEngine)

cmake_minimum_required(VERSION 3.22)
project(MeterTapAnalysisTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(ANALYSIS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(test_peak_rms test_peak_rms.cpp)
target_include_directories(test_peak_rms PRIVATE ${ANALYSIS_INCLUDE})
add_test(NAME V1_V2_V3_Drift_PeakRms COMMAND test_peak_rms)

add_executable(test_ballistics test_ballistics.cpp)
target_include_directories(test_ballistics PRIVATE ${ANALYSIS_INCLUDE})
add_test(NAME V6_MeterBallistics COMMAND test_ballistics)

add_executable(compile_check compile_check.cpp)
target_include_directories(compile_check PRIVATE ${ANALYSIS_INCLUDE})
add_test(NAME Phase2_CompileCheck COMMAND compile_check)

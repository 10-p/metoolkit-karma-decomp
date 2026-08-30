# metoolkit-karma-decomp — the top level, so FetchContent works in one line.
#
#   FetchContent_Declare(MetoolkitKarmaDecomp
#     GIT_REPOSITORY https://github.com/10-p/metoolkit-karma-decomp.git
#     GIT_TAG        main)
#   FetchContent_MakeAvailable(MetoolkitKarmaDecomp)
#   target_link_libraries(YourEngine PRIVATE metoolkit_decomp::all)
#
# This file exists ONLY to make that work. The substance is in
# metoolkit_decomp/CMakeLists.txt; read that one.
#
# ⚠ THE DUMMY IS NOT ADDED BY DEFAULT. metoolkit_decomp_dummy defines target
# names that mirror the real ones, and adding both unconditionally would put two
# libraries called McdPrimitives in one build. Ask for it explicitly:
#
#   set(METOOLKIT_KARMA_DECOMP_WITH_DUMMY ON)
#
# and then link metoolkit_decomp_dummy::all INSTEAD of metoolkit_decomp::all.

cmake_minimum_required(VERSION 3.16)
project(metoolkit_karma_decomp LANGUAGES C)

option(METOOLKIT_KARMA_DECOMP_WITH_DUMMY
       "Also configure the empty placeholder libraries (see metoolkit_decomp_dummy/)" OFF)

add_subdirectory(metoolkit_decomp)

if(METOOLKIT_KARMA_DECOMP_WITH_DUMMY)
  add_subdirectory(metoolkit_decomp_dummy)
endif()

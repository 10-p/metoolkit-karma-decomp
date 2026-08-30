# metoolkit_decomp_dummy — the link-line placeholder, reproduced.
#
#   target_link_libraries(YourEngine PRIVATE metoolkit_decomp_dummy::all)
#
# ---------------------------------------------------------------------------
# WHAT THE ORIGINAL metoolkit_dummy ACTUALLY IS, BECAUSE THE NAME MISLEADS.
#
# It is NOT a stub implementation. Inspected on 2026-08-30: all sixteen `.lib`
# files in `metoolkit_dummy/lib.{rel,chk}/win32_single_msvcrt` are 680 bytes,
# BYTE-IDENTICAL TO ONE ANOTHER, and each contains exactly one object compiled
# from `C:\Projects\tmp\Dummy\D1.c` that defines no symbols at all. They are
# EMPTY ARCHIVES WITH THE RIGHT NAMES.
#
# That is a real and useful thing, and it is worth saying what problem it solves.
# UT2003/UT2004's makefiles always name the Karma libraries on the link line. A
# build that wants Karma out compiles the calls away (`NO_KARMA`) but the link
# line does not change, so the linker still has to find something called `Mdt`.
# These satisfy that and contribute nothing.
#
# ⚠ SO THIS WILL NOT LINK A PROGRAM THAT ACTUALLY CALLS KARMA. If you link this
# and get undefined references to McdInit, MdtWorldCreate and friends, the dummy
# is working exactly as designed and your problem is that the calls are still
# compiled in. Define NO_KARMA (or your project's equivalent) as well.
#
# It exists here so a consumer can A/B physics on and off by swapping ONE target
# — `metoolkit_decomp::all` for `metoolkit_decomp_dummy::all` — with the rest of
# the build untouched. That A/B is how the recovered library was judged
# throughout: see decomp/proven.txt and the engine's `native` vs
# `native-nokarma` presets.
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)
project(metoolkit_decomp_dummy LANGUAGES C)

set(MDD_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(MDD_REPO "${CMAKE_CURRENT_LIST_DIR}/..")

# The library names come from the SAME generated map the real thing uses, so the
# two can never drift apart. If a library appears there, a placeholder for it
# appears here.
file(STRINGS "${MDD_REPO}/metoolkit_decomp/library-map.txt" MDD_LINES REGEX "^[^#]")
set(MDD_LIBRARIES "")
foreach(line IN LISTS MDD_LINES)
  string(REGEX MATCH "^([A-Za-z0-9_]+)[ \t]" _m "${line}")
  if(_m)
    list(APPEND MDD_LIBRARIES "${CMAKE_MATCH_1}")
  endif()
endforeach()
list(REMOVE_DUPLICATES MDD_LIBRARIES)

# An archive needs at least one translation unit to exist at all; empty.c is it.
# It is a real file rather than a generated one so that `ar t` on the result
# tells the next reader what they are looking at.
foreach(lib IN LISTS MDD_LIBRARIES)
  add_library(metoolkit_decomp_dummy_${lib} STATIC "${MDD_ROOT}/src/empty.c")
  add_library(metoolkit_decomp_dummy::${lib} ALIAS metoolkit_decomp_dummy_${lib})
  set_target_properties(metoolkit_decomp_dummy_${lib} PROPERTIES OUTPUT_NAME "${lib}")
  set_property(TARGET metoolkit_decomp_dummy_${lib} PROPERTY POSITION_INDEPENDENT_CODE ON)
endforeach()

add_library(metoolkit_decomp_dummy STATIC "${MDD_ROOT}/src/empty.c")
add_library(metoolkit_decomp_dummy::all ALIAS metoolkit_decomp_dummy)

# The headers ARE provided, and deliberately: swapping the implementation out
# must not stop the consumer's sources compiling. Only the definitions go away.
set(METOOLKIT_DIR "${MDD_REPO}/metoolkit" CACHE PATH
    "Karma SDK root: include/ + lib.{chk,rel}/")
target_include_directories(metoolkit_decomp_dummy PUBLIC
    "${MDD_REPO}/metoolkit_decomp/include"
    "${METOOLKIT_DIR}/include"
    "${METOOLKIT_DIR}/include/MdtBcl"
    "${METOOLKIT_DIR}/include/MdtKea"
    "${METOOLKIT_DIR}/include/McdCommon"
    "${METOOLKIT_DIR}/include/McdPrimitives"
    "${METOOLKIT_DIR}/include/McdFrame"
    "${METOOLKIT_DIR}/include/Mst"
    "${METOOLKIT_DIR}/include/MeGlobals"
    "${METOOLKIT_DIR}/include/MeApp")

# Same INTERFACE define as the real target. A consumer that swaps between the
# two must not have the calling convention change underneath it.
target_compile_definitions(metoolkit_decomp_dummy INTERFACE ME_FORCE_CDECL=1)

list(LENGTH MDD_LIBRARIES MDD_COUNT)
message(STATUS
  "metoolkit_decomp_dummy: ${MDD_COUNT} empty placeholder(s) — NO physics. "
  "Compile the Karma calls out too (NO_KARMA) or the link will fail.")

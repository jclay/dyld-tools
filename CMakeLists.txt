cmake_minimum_required(VERSION 3.30)
project(dyld-tools VERSION 1340.0 LANGUAGES C CXX OBJC OBJCXX)

if(NOT APPLE)
    message(FATAL_ERROR "This project only builds on macOS/Apple platforms")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================================================================
#  Strip bridgeos availability annotations from upstream headers
# =============================================================================
#
# The public macOS SDK doesn't define `bridgeos` as a platform, so annotations
# like __API_AVAILABLE(..., bridgeos(5.0)) cause hard errors.  At configure
# time we copy the affected headers with the annotations removed so the build
# uses them instead of the originals from include/mach-o/.

set(_bridgeos_headers dyld.h dyld_introspection.h dyld_priv.h dyld_process_info.h)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include-generated/mach-o")
foreach(_hdr ${_bridgeos_headers})
    file(READ "${CMAKE_SOURCE_DIR}/include/mach-o/${_hdr}" _content)
    # Remove bridgeos(...) with version from availability/SPI lists.
    string(REGEX REPLACE ",[ \t]*bridgeos\\([0-9.]+\\)" "" _content "${_content}")
    string(REGEX REPLACE "bridgeos\\([0-9.]+\\),[ \t]*" "" _content "${_content}")
    string(REGEX REPLACE "bridgeos\\([0-9.]+\\)"        "" _content "${_content}")
    # Remove bare 'bridgeos' (no version) from unavailability lists.
    string(REGEX REPLACE ",[ \t]*bridgeos([^(])" "\\1" _content "${_content}")
    string(REGEX REPLACE "bridgeos,[ \t]*"              "" _content "${_content}")
    # Remove standalone __API_UNAVAILABLE(bridgeos) and any empty remainders.
    string(REGEX REPLACE "__API_UNAVAILABLE\\(bridgeos\\)" "" _content "${_content}")
    string(REGEX REPLACE "__API_UNAVAILABLE\\(\\)"         "" _content "${_content}")
    file(WRITE "${CMAKE_BINARY_DIR}/include-generated/mach-o/${_hdr}" "${_content}")
endforeach()

# Some sources include these by bare name (e.g. #include "dyld_priv.h").
# Create copies in include-generated/ so they resolve via the include path.
foreach(_hdr dyld_priv.h dyld_introspection.h)
    file(COPY_FILE
        "${CMAKE_BINARY_DIR}/include-generated/mach-o/${_hdr}"
        "${CMAKE_BINARY_DIR}/include-generated/${_hdr}"
        ONLY_IF_DIFFERENT)
endforeach()

# =============================================================================
#  Shared build interface — consumed by all targets via target_link_libraries
# =============================================================================

add_library(dyld_build_settings INTERFACE)

target_compile_features(dyld_build_settings INTERFACE cxx_std_20)

target_compile_options(dyld_build_settings INTERFACE
    -Wmost
    -Wimplicit-fallthrough
    -Wno-unknown-pragmas
    -Wno-assume
    -Wno-vla-extension
    -Wno-unused-but-set-variable
    -Wno-deprecated-declarations
    -fno-exceptions
    -include ${CMAKE_SOURCE_DIR}/stubs/compat_availability.h
)

# -fno-rtti is added PRIVATE to each target that needs it (all except
# dyld_usage, which requires RTTI for dynamic_cast).  Keeping it out of the
# INTERFACE avoids propagation issues where PRIVATE -frtti overrides can't
# win against INTERFACE -fno-rtti due to CMake flag ordering.

# =============================================================================
#  Fetch private SDK headers from the open-source XNU project
# =============================================================================
#
# Several private headers (corecrypto, kdebug, dyld_pager, etc.) are not in
# the public macOS SDK but ARE published in Apple's open-source XNU repo.
# We download the specific files we need at configure time and place them
# in xnu-includes/ with the directory layout our sources expect.
#
# stubs/corecrypto/ wrappers add extern "C" then #include_next the real ones.
# The other XNU headers already have __BEGIN_DECLS / __END_DECLS.

set(_xnu_tag "xnu-11417.101.15")
set(_xnu_url "https://raw.githubusercontent.com/apple-oss-distributions/xnu/${_xnu_tag}")
set(_xnu_dir "${CMAKE_BINARY_DIR}/xnu-includes")

# (remote-path  local-path) pairs
set(_xnu_headers
    # corecrypto — struct layouts and function decls for libcorecrypto.dylib
    "EXTERNAL_HEADERS/corecrypto/cc.h            corecrypto/cc.h"
    "EXTERNAL_HEADERS/corecrypto/cc_config.h     corecrypto/cc_config.h"
    "EXTERNAL_HEADERS/corecrypto/cc_impl.h       corecrypto/cc_impl.h"
    "EXTERNAL_HEADERS/corecrypto/cc_error.h      corecrypto/cc_error.h"
    "EXTERNAL_HEADERS/corecrypto/ccn.h           corecrypto/ccn.h"
    "EXTERNAL_HEADERS/corecrypto/ccdigest.h      corecrypto/ccdigest.h"
    "EXTERNAL_HEADERS/corecrypto/ccsha1.h        corecrypto/ccsha1.h"
    "EXTERNAL_HEADERS/corecrypto/ccsha2.h        corecrypto/ccsha2.h"
    # kdebug / kernel tracing
    "bsd/sys/kdebug_private.h                    sys/kdebug_private.h"
    # jetsam / memory pressure
    "bsd/sys/kern_memorystatus.h                 sys/kern_memorystatus.h"
    # abort reasons (also a transitive dep of kern_memorystatus.h)
    "bsd/sys/reason.h                            sys/reason.h"
    "bsd/sys/reason.h                            System/sys/reason.h"
    # SIP (System Integrity Protection)
    "bsd/sys/csr.h                               System/sys/csr.h"
    # dyld pager — mwl_region / mwl_info_hdr struct layouts
    "osfmk/mach/dyld_pager.h                     System/mach/dyld_pager.h"
)

foreach(_pair ${_xnu_headers})
    string(REGEX MATCH "^([^ ]+) +(.+)$" _ "${_pair}")
    set(_remote "${CMAKE_MATCH_1}")
    set(_local  "${CMAKE_MATCH_2}")
    set(_dest   "${_xnu_dir}/${_local}")
    if(NOT EXISTS "${_dest}")
        get_filename_component(_dest_dir "${_dest}" DIRECTORY)
        file(MAKE_DIRECTORY "${_dest_dir}")
        file(DOWNLOAD "${_xnu_url}/${_remote}" "${_dest}" STATUS _dl_status)
        list(GET _dl_status 0 _dl_code)
        if(NOT _dl_code EQUAL 0)
            message(FATAL_ERROR "Failed to download ${_remote}: ${_dl_status}")
        endif()
    endif()
endforeach()

# Generated headers MUST come before include/ so the bridgeos-stripped versions
# shadow the originals.  stubs/ comes first for all other private-SDK shims.
target_include_directories(dyld_build_settings INTERFACE
    ${CMAKE_SOURCE_DIR}/stubs
    ${CMAKE_BINARY_DIR}/include-generated
    ${CMAKE_BINARY_DIR}/xnu-includes      # corecrypto/ symlink
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/dyld
    ${CMAKE_SOURCE_DIR}/mach_o
    ${CMAKE_SOURCE_DIR}/mach_o_writer
    ${CMAKE_SOURCE_DIR}/lsl
    ${CMAKE_SOURCE_DIR}/other-tools
    ${CMAKE_SOURCE_DIR}/cache_builder
    ${CMAKE_SOURCE_DIR}/libdyld
    ${CMAKE_SOURCE_DIR}/kernel-collection-builder
)

target_compile_definitions(dyld_build_settings INTERFACE INTERNAL_BUILD=1)

# =============================================================================
#  Static libraries
# =============================================================================

add_library(mach_o STATIC)
target_sources(mach_o PRIVATE
    mach_o/Architecture.cpp
    mach_o/Archive.cpp
    mach_o/BindOpcodes.cpp
    mach_o/ChainedFixups.cpp
    mach_o/CompactUnwind.cpp
    mach_o/DataInCode.cpp
    mach_o/DwarfDebug.cpp
    mach_o/Error.cpp
    mach_o/ExportsTrie.cpp
    mach_o/Fixups.cpp
    mach_o/FunctionStarts.cpp
    mach_o/FunctionVariants.cpp
    mach_o/GradedArchitectures.cpp
    mach_o/Header.cpp
    mach_o/Image.cpp
    mach_o/Instructions.cpp
    mach_o/LinkerOptimizationHints.cpp
    mach_o/LoggingStub.cpp
    mach_o/Misc.cpp
    mach_o/NListSymbolTable.cpp
    mach_o/ObjC.cpp
    mach_o/Platform.cpp
    mach_o/Policy.cpp
    mach_o/RebaseOpcodes.cpp
    mach_o/SplitSeg.cpp
    mach_o/Symbol.cpp
    mach_o/Universal.cpp
    mach_o/Version32.cpp
    mach_o/Version64.cpp
)
target_link_libraries(mach_o PUBLIC dyld_build_settings)
target_compile_options(mach_o PRIVATE -fno-rtti)

add_library(mach_o_writer STATIC)
target_sources(mach_o_writer PRIVATE
    mach_o_writer/ArchiveWriter.cpp
    mach_o_writer/BindOpcodesWriter.cpp
    mach_o_writer/ChainedFixupsWriter.cpp
    mach_o_writer/ChunkBumpAllocator.cpp
    mach_o_writer/CompactUnwindWriter.cpp
    mach_o_writer/DataInCodeWriter.cpp
    mach_o_writer/ExportsTrieWriter.cpp
    mach_o_writer/FunctionStartsWriter.cpp
    mach_o_writer/FunctionVariantsWriter.cpp
    mach_o_writer/HeaderWriter.cpp
    mach_o_writer/LinkerOptimizationHintsWriter.cpp
    mach_o_writer/NListSymbolTableWriter.cpp
    mach_o_writer/RebaseOpcodesWriter.cpp
    mach_o_writer/SplitSegWriter.cpp
    mach_o_writer/UniversalWriter.cpp
)
target_link_libraries(mach_o_writer PUBLIC dyld_build_settings)
target_compile_options(mach_o_writer PRIVATE -fno-rtti)

# -- lsl (lower system layer) --

add_library(lsl STATIC)
target_sources(lsl PRIVATE
    lsl/Allocator.cpp
    lsl/CRC32c.cpp
    lsl/PVLEInt64.cpp
)
target_link_libraries(lsl PUBLIC dyld_build_settings)
target_compile_options(lsl PRIVATE -fno-rtti)

# -- dyld_common: core Mach-O parsing and shared cache support --
#
# All common/ sources live here, compiled once without tool-specific
# BUILDING_* defines.  Tools that need BUILDING_*-gated code (e.g.
# dyld_shared_cache_util needing forEachPatchableUseOfExport) also
# compile those specific sources as per-tool sources with the right
# defines.  STATIC archive semantics ensure the per-tool .o supersedes
# the archive's copy — no duplicate symbols.

add_library(dyld_common STATIC)
target_sources(dyld_common PRIVATE
    common/Algorithm.cpp
    common/CachePatching.cpp
    common/ClosureFileSystemNull.cpp
    common/ClosureFileSystemPhysical.cpp
    common/Diagnostics.cpp
    common/DyldSharedCache.cpp
    common/FileManager.cpp
    common/MachOAnalyzer.cpp
    common/MachOFile.cpp
    common/MachOLayout.cpp
    common/MachOLoaded.cpp
    common/MetadataVisitor.cpp
    common/MurmurHash.cpp
    common/PerfectHash.cpp
    common/PropertyList.cpp
    common/SafeVMPrimitives.cpp
    common/TargetPolicy.cpp
    common/Utilities.cpp
)
target_link_libraries(dyld_common PUBLIC dyld_build_settings lsl)
target_compile_options(dyld_common PRIVATE -fno-rtti)

# -- dyld_runtime: dyld internals (used by closure_util and shared_cache_util) --

add_library(dyld_runtime STATIC)
target_sources(dyld_runtime PRIVATE
    dyld/DyldDelegates.cpp
    dyld/DyldProcessConfig.cpp
    dyld/DyldRuntimeState.cpp
    dyld/JustInTimeLoader.cpp
    dyld/Loader.cpp
    dyld/PrebuiltLoader.cpp
    dyld/PrebuiltObjC.cpp
    dyld/PrebuiltSwift.cpp
    dyld/SharedCacheRuntime.cpp
    dyld/Tracing.cpp
)
target_compile_definitions(dyld_runtime PRIVATE BUILDING_CLOSURE_UTIL=1)
# dyld_runtime references symbols from dyld_common (e.g. MetadataVisitor).
# Declaring the dependency ensures CMake orders dyld_common AFTER dyld_runtime
# in the link line so the linker can resolve those references.
target_link_libraries(dyld_runtime PUBLIC dyld_build_settings dyld_common)
target_compile_options(dyld_runtime PRIVATE -fno-rtti)

# =============================================================================
#  CLI tools
# =============================================================================

# -- dyld_info: Mach-O binary inspector ------------------------------------

add_executable(dyld_info)
target_sources(dyld_info PRIVATE
    other-tools/dyld_info.cpp
    other-tools/FileUtils.cpp
    other-tools/MiscFileUtils.cpp
    other-tools/SymbolicatedImage.cpp
    common/SwiftVisitor.cpp
)
target_compile_definitions(dyld_info PRIVATE BUILDING_DYLDINFO=1)
target_compile_options(dyld_info PRIVATE -fno-rtti)
target_link_libraries(dyld_info PRIVATE mach_o dyld_common)

# -- dyld_nm: symbol table viewer ------------------------------------------

add_executable(dyld_nm)
target_sources(dyld_nm PRIVATE
    other-tools/dyld_nm.cpp
    other-tools/MiscFileUtils.cpp
)
target_compile_definitions(dyld_nm PRIVATE BUILDING_NM=1)
target_compile_options(dyld_nm PRIVATE -fno-rtti)
target_link_libraries(dyld_nm PRIVATE mach_o dyld_common)

# -- machocheck: Mach-O validation -----------------------------------------

add_executable(machocheck)
target_sources(machocheck PRIVATE
    other-tools/machocheck.cpp
    other-tools/os_macho_rules.cpp
    other-tools/MiscFileUtils.cpp
)
target_compile_definitions(machocheck PRIVATE BUILDING_MACHOCHECK=1)
target_compile_options(machocheck PRIVATE -fno-rtti)
target_link_libraries(machocheck PRIVATE mach_o dyld_common)

# -- dyld_inspect: process introspection ------------------------------------

add_executable(dyld_inspect)
target_sources(dyld_inspect PRIVATE other-tools/dyld_inspect.cpp)
target_compile_options(dyld_inspect PRIVATE -fno-rtti)
target_link_libraries(dyld_inspect PRIVATE mach_o)

# -- dyld_usage: kernel trace-based event viewer ----------------------------

add_executable(dyld_usage)
target_sources(dyld_usage PRIVATE other-tools/dyld_usage.cpp)
# dyld_usage uses dynamic_cast — RTTI must stay enabled (no -fno-rtti).
target_link_libraries(dyld_usage PRIVATE mach_o)

find_library(KTRACE_LIB ktrace)
if(KTRACE_LIB)
    target_link_libraries(dyld_usage PRIVATE ${KTRACE_LIB})
else()
    message(WARNING "libktrace not found — dyld_usage will build but may not link")
endif()

# -- dyld_closure_util: closure viewer/creator ------------------------------

add_executable(dyld_closure_util)
target_sources(dyld_closure_util PRIVATE
    other-tools/dyld_closure_util.cpp
    common/ObjCVisitor.cpp
    common/OptimizerSwift.cpp
    common/SwiftVisitor.cpp
    common/MetadataVisitor.cpp
    common/ProcessAtlas.cpp
)
target_compile_definitions(dyld_closure_util PRIVATE BUILDING_CLOSURE_UTIL=1)
target_compile_options(dyld_closure_util PRIVATE -fno-rtti)
target_link_libraries(dyld_closure_util PRIVATE
    mach_o mach_o_writer dyld_common dyld_runtime
)

# -- dyld_shared_cache_util: shared cache inspector/extractor ---------------

add_executable(dyld_shared_cache_util)
target_sources(dyld_shared_cache_util PRIVATE
    other-tools/dyld_shared_cache_util.cpp
    other-tools/dsc_iterator.cpp
    other-tools/dsc_extractor.cpp
    other-tools/make_obj_file.cpp
    other-tools/prune_trie.cpp
    other-tools/SymbolicatedImage.cpp
    common/ObjCVisitor.cpp
    common/OptimizerSwift.cpp
    common/SwiftVisitor.cpp
    common/MetadataVisitor.cpp
    common/ProcessAtlas.cpp
    # Recompiled with BUILDING_SHARED_CACHE_UTIL for functions gated behind
    # that define (e.g. forEachPatchableUseOfExport).  The per-tool .o files
    # supersede the copies in libdyld_common.a via STATIC archive semantics.
    common/DyldSharedCache.cpp
    common/CachePatching.cpp
)
target_compile_definitions(dyld_shared_cache_util PRIVATE
    BUILDING_SHARED_CACHE_UTIL=1
    BUILDING_CLOSURE_UTIL=1
)
target_compile_options(dyld_shared_cache_util PRIVATE -faligned-allocation -fno-rtti)
target_link_libraries(dyld_shared_cache_util PRIVATE
    mach_o mach_o_writer dyld_common dyld_runtime
    "-framework CoreFoundation"
)

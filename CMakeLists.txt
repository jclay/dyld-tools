cmake_minimum_required(VERSION 3.20)
project(dyld-tools C CXX OBJC OBJCXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJC_STANDARD 11)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Platform checks -----------------------------------------------------------

if(NOT APPLE)
    message(FATAL_ERROR "This project only builds on macOS/Apple platforms")
endif()

# --- Global compiler settings --------------------------------------------------

add_compile_options(
    -Wmost
    -Wimplicit-fallthrough
    -Wno-unknown-pragmas
    -Wno-assume
    -Wno-vla-extension
    -Wno-unused-but-set-variable
    -fno-exceptions
    -fno-rtti
    -include ${CMAKE_SOURCE_DIR}/stubs/compat_availability.h
)

# --- Include paths -------------------------------------------------------------
# Stubs MUST come first so they shadow private SDK headers
set(STUBS_DIR "${CMAKE_SOURCE_DIR}/stubs")

# Common include directories shared by all targets
set(COMMON_INCLUDES
    ${STUBS_DIR}              # Stubs for private SDK headers (must be first)
    ${CMAKE_SOURCE_DIR}/include       # Public-style headers (mach-o/dyld_priv.h, etc.)
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

# --- Common preprocessor definitions ------------------------------------------

set(COMMON_DEFINES
    INTERNAL_BUILD=1
)

# =============================================================================
#  Static Libraries
# =============================================================================

# --- libmach_o ----------------------------------------------------------------

add_library(mach_o STATIC
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

target_include_directories(mach_o PUBLIC ${COMMON_INCLUDES})
target_compile_definitions(mach_o PRIVATE ${COMMON_DEFINES})
target_compile_options(mach_o PRIVATE -Wno-deprecated-declarations)

# --- libmach_o_writer ---------------------------------------------------------

add_library(mach_o_writer STATIC
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

target_include_directories(mach_o_writer PUBLIC ${COMMON_INCLUDES})
target_compile_definitions(mach_o_writer PRIVATE ${COMMON_DEFINES})
target_compile_options(mach_o_writer PRIVATE -Wno-deprecated-declarations)

# =============================================================================
#  Helper: common source files used by multiple tools
# =============================================================================

# Common infrastructure sources (from common/)
set(COMMON_SRCS_DIAGNOSTICS     common/Diagnostics.cpp)
set(COMMON_SRCS_MACHO_FILE      common/MachOFile.cpp)
set(COMMON_SRCS_MACHO_LOADED    common/MachOLoaded.cpp)
set(COMMON_SRCS_MACHO_ANALYZER  common/MachOAnalyzer.cpp)
set(COMMON_SRCS_MACHO_LAYOUT    common/MachOLayout.cpp)
set(COMMON_SRCS_SHARED_CACHE    common/DyldSharedCache.cpp)
set(COMMON_SRCS_TARGET_POLICY   common/TargetPolicy.cpp)
set(COMMON_SRCS_CACHE_PATCHING  common/CachePatching.cpp)
set(COMMON_SRCS_SWIFT_VISITOR   common/SwiftVisitor.cpp)
set(COMMON_SRCS_OBJC_VISITOR    common/ObjCVisitor.cpp)
set(COMMON_SRCS_METADATA        common/MetadataVisitor.cpp)
set(COMMON_SRCS_OPTIMIZER_SWIFT common/OptimizerSwift.cpp)
set(COMMON_SRCS_PERFECT_HASH    common/PerfectHash.cpp)
set(COMMON_SRCS_MURMURHASH      common/MurmurHash.cpp)
set(COMMON_SRCS_UTILITIES       common/Utilities.cpp)
set(COMMON_SRCS_FILE_MANAGER    common/FileManager.cpp)
set(COMMON_SRCS_CLOSURE_FS_PHYS common/ClosureFileSystemPhysical.cpp)
set(COMMON_SRCS_CLOSURE_FS_NULL common/ClosureFileSystemNull.cpp)
set(COMMON_SRCS_PROCESS_ATLAS   common/ProcessAtlas.cpp)
set(COMMON_SRCS_ALGORITHM       common/Algorithm.cpp)
set(COMMON_SRCS_SYMBOLS_CACHE   common/SymbolsCache.cpp)
set(COMMON_SRCS_SAFE_VM         common/SafeVMPrimitives.cpp)
set(COMMON_SRCS_PROPERTY_LIST   common/PropertyList.cpp)

# lsl sources
set(LSL_SRCS_ALLOCATOR          lsl/Allocator.cpp)
set(LSL_SRCS_CRC32              lsl/CRC32c.cpp)
set(LSL_SRCS_PVLE               lsl/PVLEInt64.cpp)

# =============================================================================
#  Helper function for CLI tool targets
# =============================================================================

function(add_dyld_tool TARGET_NAME)
    cmake_parse_arguments(TOOL "" "" "SOURCES;DEFINES;FRAMEWORKS;LIBRARIES" ${ARGN})

    add_executable(${TARGET_NAME} ${TOOL_SOURCES})

    target_include_directories(${TARGET_NAME} PRIVATE ${COMMON_INCLUDES})

    target_compile_definitions(${TARGET_NAME} PRIVATE
        ${COMMON_DEFINES}
        ${TOOL_DEFINES}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE mach_o)

    if(TOOL_LIBRARIES)
        target_link_libraries(${TARGET_NAME} PRIVATE ${TOOL_LIBRARIES})
    endif()

    if(TOOL_FRAMEWORKS)
        foreach(fw ${TOOL_FRAMEWORKS})
            target_link_libraries(${TARGET_NAME} PRIVATE "-framework ${fw}")
        endforeach()
    endif()

    # Suppress common warnings in legacy code
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wno-deprecated-declarations
        -Wno-format-nonliteral
    )
endfunction()

# =============================================================================
#  CLI Tools
# =============================================================================

# --- dyld_info ----------------------------------------------------------------
# Mach-O binary inspector. The most broadly useful tool.

add_dyld_tool(dyld_info
    SOURCES
        other-tools/dyld_info.cpp
        ${COMMON_SRCS_SHARED_CACHE}
        ${COMMON_SRCS_DIAGNOSTICS}
        ${COMMON_SRCS_SWIFT_VISITOR}
        other-tools/FileUtils.cpp
        ${COMMON_SRCS_CACHE_PATCHING}
        ${COMMON_SRCS_TARGET_POLICY}
        other-tools/SymbolicatedImage.cpp
        ${COMMON_SRCS_MACHO_FILE}
        ${COMMON_SRCS_MACHO_LOADED}
        ${COMMON_SRCS_MACHO_ANALYZER}
        ${COMMON_SRCS_MACHO_LAYOUT}
        ${COMMON_SRCS_METADATA}
        other-tools/MiscFileUtils.cpp
    DEFINES
        BUILDING_DYLDINFO=1
)

# --- dyld_nm ------------------------------------------------------------------
# Symbol table viewer for Mach-O binaries and shared caches.

add_dyld_tool(dyld_nm
    SOURCES
        other-tools/dyld_nm.cpp
        ${COMMON_SRCS_TARGET_POLICY}
        ${COMMON_SRCS_ALGORITHM}
        ${COMMON_SRCS_SHARED_CACHE}
        ${COMMON_SRCS_MACHO_FILE}
        ${COMMON_SRCS_MACHO_LOADED}
        ${COMMON_SRCS_MACHO_LAYOUT}
        ${COMMON_SRCS_CACHE_PATCHING}
        other-tools/MiscFileUtils.cpp
        ${COMMON_SRCS_DIAGNOSTICS}
    DEFINES
        BUILDING_NM=1
)

# --- machocheck ---------------------------------------------------------------
# Mach-O validation tool.

add_dyld_tool(machocheck
    SOURCES
        other-tools/machocheck.cpp
        other-tools/os_macho_rules.cpp
        other-tools/MiscFileUtils.cpp
        ${COMMON_SRCS_TARGET_POLICY}
        ${COMMON_SRCS_SHARED_CACHE}
        ${COMMON_SRCS_DIAGNOSTICS}
        ${COMMON_SRCS_MACHO_FILE}
        ${COMMON_SRCS_MACHO_LOADED}
        ${COMMON_SRCS_MACHO_LAYOUT}
        ${COMMON_SRCS_CACHE_PATCHING}
    DEFINES
        BUILDING_MACHOCHECK=1
)

# --- dyld_symbols_cache -------------------------------------------------------
# Symbols cache management tool.
# NOTE: Disabled — source references File.h which is missing from this snapshot.
# add_dyld_tool(dyld_symbols_cache ...)

# --- dyld_inspect -------------------------------------------------------------
# Process introspection tool (shared cache info for running processes).

add_dyld_tool(dyld_inspect
    SOURCES
        other-tools/dyld_inspect.cpp
    DEFINES
        # No special defines needed
)

# --- dyld_usage ---------------------------------------------------------------
# Kernel trace-based dyld event viewer. Requires libktrace at runtime.

add_dyld_tool(dyld_usage
    SOURCES
        other-tools/dyld_usage.cpp
    DEFINES
        # No special defines needed
)
# dyld_usage links against libktrace — try to find it, fall back to stub
find_library(KTRACE_LIB ktrace)
if(KTRACE_LIB)
    target_link_libraries(dyld_usage PRIVATE ${KTRACE_LIB})
else()
    message(WARNING "libktrace not found — dyld_usage will build but may not link")
endif()
target_compile_options(dyld_usage PRIVATE -frtti)

# --- dyld_closure_util --------------------------------------------------------
# Closure viewer/creator. Heavy dependency on dyld runtime internals.

add_dyld_tool(dyld_closure_util
    SOURCES
        other-tools/dyld_closure_util.cpp
        ${COMMON_SRCS_PERFECT_HASH}
        dyld/Loader.cpp
        dyld/JustInTimeLoader.cpp
        dyld/PrebuiltLoader.cpp
        dyld/PrebuiltSwift.cpp
        dyld/PrebuiltObjC.cpp
        dyld/DyldProcessConfig.cpp
        dyld/DyldRuntimeState.cpp
        dyld/DyldDelegates.cpp
        dyld/Tracing.cpp
        dyld/SharedCacheRuntime.cpp
        ${COMMON_SRCS_CACHE_PATCHING}
        ${COMMON_SRCS_OPTIMIZER_SWIFT}
        ${COMMON_SRCS_MACHO_FILE}
        ${COMMON_SRCS_MACHO_LOADED}
        ${COMMON_SRCS_MACHO_ANALYZER}
        ${COMMON_SRCS_MACHO_LAYOUT}
        ${COMMON_SRCS_DIAGNOSTICS}
        ${COMMON_SRCS_METADATA}
        ${COMMON_SRCS_OBJC_VISITOR}
        ${COMMON_SRCS_SWIFT_VISITOR}
        ${COMMON_SRCS_CLOSURE_FS_PHYS}
        ${COMMON_SRCS_UTILITIES}
        ${COMMON_SRCS_SHARED_CACHE}
        ${COMMON_SRCS_MURMURHASH}
        ${COMMON_SRCS_TARGET_POLICY}
        ${COMMON_SRCS_FILE_MANAGER}
        ${LSL_SRCS_ALLOCATOR}
        ${LSL_SRCS_CRC32}
        ${LSL_SRCS_PVLE}
    DEFINES
        BUILDING_CLOSURE_UTIL=1
    LIBRARIES
        mach_o_writer
)

# --- dyld_shared_cache_util ---------------------------------------------------
# Shared cache inspector/extractor. Heavy dependency on dyld internals.

add_dyld_tool(dyld_shared_cache_util
    SOURCES
        other-tools/dyld_shared_cache_util.cpp
        dyld/DyldProcessConfig.cpp
        other-tools/dsc_iterator.cpp
        other-tools/dsc_extractor.cpp
        ${COMMON_SRCS_CACHE_PATCHING}
        ${COMMON_SRCS_CLOSURE_FS_PHYS}
        ${COMMON_SRCS_SHARED_CACHE}
        ${COMMON_SRCS_DIAGNOSTICS}
        other-tools/make_obj_file.cpp
        ${COMMON_SRCS_OPTIMIZER_SWIFT}
        ${COMMON_SRCS_MACHO_LAYOUT}
        other-tools/prune_trie.cpp
        ${COMMON_SRCS_MACHO_FILE}
        ${COMMON_SRCS_OBJC_VISITOR}
        dyld/DyldRuntimeState.cpp
        dyld/DyldDelegates.cpp
        dyld/Tracing.cpp
        dyld/SharedCacheRuntime.cpp
        ${COMMON_SRCS_MACHO_LOADED}
        ${COMMON_SRCS_MACHO_ANALYZER}
        ${COMMON_SRCS_METADATA}
        ${COMMON_SRCS_TARGET_POLICY}
        ${COMMON_SRCS_UTILITIES}
        ${COMMON_SRCS_PROCESS_ATLAS}
        ${COMMON_SRCS_FILE_MANAGER}
        ${COMMON_SRCS_SAFE_VM}
        other-tools/SymbolicatedImage.cpp
        ${COMMON_SRCS_ALGORITHM}
        ${COMMON_SRCS_PERFECT_HASH}
        ${COMMON_SRCS_MURMURHASH}
        ${COMMON_SRCS_SWIFT_VISITOR}
        dyld/Loader.cpp
        dyld/JustInTimeLoader.cpp
        dyld/PrebuiltLoader.cpp
        dyld/PrebuiltSwift.cpp
        dyld/PrebuiltObjC.cpp
        ${LSL_SRCS_ALLOCATOR}
        ${LSL_SRCS_CRC32}
        ${LSL_SRCS_PVLE}
    DEFINES
        BUILDING_SHARED_CACHE_UTIL=1
        BUILDING_CLOSURE_UTIL=1
    LIBRARIES
        mach_o_writer
    FRAMEWORKS
        CoreFoundation
)
target_compile_options(dyld_shared_cache_util PRIVATE -faligned-allocation)

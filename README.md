# dyld tools

Pre-built macOS (arm64) CLI utilities from Apple's open-source [dyld](https://github.com/apple-oss-distributions/dyld) project. These tools inspect Mach-O binaries and the dyld shared cache -- including libraries that have been removed from disk and merged into the cache, where standard tools like `nm` and `otool` fail.

Built from **dyld-1340** source with a CMake build system and stub headers for private SDK dependencies.

## Install

Download all tools:

```sh
cd /usr/local/bin && for tool in dyld_info dyld_nm dyld_shared_cache_util dyld_closure_util machocheck dyld_inspect dyld_usage; do
  curl -fLO "https://github.com/jclay/dyld-tools/releases/latest/download/$tool" && chmod +x "$tool"
done
```

Or grab individual tools with the `curl` commands in each tool section below.

> **Note:** Pre-built binaries are arm64 (Apple Silicon) only. x86_64 users can [build from source](#building-from-source).

---

## Tools

### dyld_info

Mach-O binary inspector. Shows platforms, segments, fixups, exports, imports, ObjC metadata, load commands, and more.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_info -o /usr/local/bin/dyld_info && chmod +x /usr/local/bin/dyld_info
```

```
Usage: dyld_info <options>* <mach-o file>+ | -all_dir <dir> | -all_dyld_cache
    -arch                       limit to specified architecture(s)
    -platform                   print platform (default if no options specified)
    -segments                   print segments (default if no options specified)
    -linked_dylibs              print all dylibs this image links against (default if no options specified)
    -inits                      print initializers
    -fixups                     print locations dyld will rebase/bind
    -exports                    print all exported symbols
    -imports                    print all symbols needed from other dylibs
    -objc                       print objc classes, categories, etc
    -fixup_chains               print info about chain format and starts
    -fixup_chain_details        print detailed info about every fixup in chain
    -symbolic_fixups            print ranges of each atom of DATA with symbol name and fixups
    -shared_region              print shared cache (split seg) info
    -function_starts            print function starts information
    -opcodes                    print opcodes information
    -load_commands              print load commands
    -uuid                       print UUID of binary
    -function_variants          print info on function variants in binary
    -disassemble                print all code sections using disassembler
    -section <seg> <sect>       print content of section, formatted by section type
    -all_sections               print content of all sections, formatted by section type
    -section_bytes <seg> <sect> print content of section, as raw hex bytes
    -all_sections_bytes         print content of all sections, formatted as raw hex bytes
    -validate_only              only prints any malformedness about file(s)
    -no_validate                don't check for malformedness about file(s)
    -all_dir                    recurse directory to find all mach-o files
    -all_dyld_cache             show specified info about all dylibs in the dyld cache
```

**Examples:**

```sh
# Show platform/SDK info for the dynamic linker
$ dyld_info -platform /usr/lib/dyld
/usr/lib/dyld [arm64e]:
    -platform:
        platform     minOS      sdk
           macOS     15.7      15.7

# List exports from a shared-cache framework
$ dyld_info -exports /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation | head -8
/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation [arm64e]:
    -exports:
        offset      symbol
        0x0012D144  _CFAbsoluteTimeAddGregorianUnits
        0x00042DB4  _CFAbsoluteTimeGetCurrent
        0x000DF258  _CFAbsoluteTimeGetDayOfWeek
        0x0012D544  _CFAbsoluteTimeGetDayOfYear
        0x0012D3E0  _CFAbsoluteTimeGetDifferenceAsGregorianUnits

# Inspect segments and fixups of the dynamic linker
$ dyld_info -segments -fixups /usr/lib/dyld | head -12
/usr/lib/dyld [arm64e]:
    -segments:
        load-offset   segment  section       sect-size  seg-size   init/max-prot
        0x00000000    __TEXT                               624KB    r.x/r.x
        0x00001000             __text           554684
        0x000886C0             __const            9248
        0x0008AAE0             __cstring         65805
        0x0009ABED             __info_plist       1252
        0x0009B0D4             __unwind_info      1192
        0x0009C000    __DATA_CONST                          32KB    rw./rw.
        0x0009C000             __auth_ptr          128
        0x0009C080             __const           30432
```

---

### dyld_nm

Symbol table viewer that works on shared-cache dylibs where `nm` fails.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_nm -o /usr/local/bin/dyld_nm && chmod +x /usr/local/bin/dyld_nm
```

```
Usage: dyld_nm [-arch <arch>]* <options>* <mach-o file>+
    -a     Display all symbol table entries, including those inserted for debuggers
    -g     Display only global (external) symbols
    -m     Symbol details in a human-friendly manner
    -n     Sort by address rather than by symbol name
    -p     Don't sort, display in symbol-table order
    -u     Display only undefined symbols
    -U     Don't display undefined symbols
    -x     Display the symbol table entry's fields in hexadecimal
    -j     Just display the symbol names (no value or type)
```

**Examples:**

```sh
# Standard nm fails on shared-cache frameworks (they don't exist on disk)
$ nm /System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote
nm: error: No such file or directory

# dyld_nm reads them directly from the shared cache
$ dyld_nm -g /System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote | head -8
/System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote [arm64e]:
0000000000000000 U _AnalyticsSendEventLazy
0000000000000000 U _AudioFormatGetProperty
0000000000000000 U _BKSApplicationStateAll
0000000000000000 U _BKSApplicationStateProcessIDKey
0000000000000000 U _BSBundleIDForPID
0000000000000000 U _CC_MD5_Final
0000000000000000 U _CC_MD5_Update

# List only defined (exported) symbols
$ dyld_nm -g -U /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation | grep CFArray | head -5
000000000004c3f0 T _CFArrayAppendArray
000000000004c398 T _CFArrayAppendValue
000000000006ff48 T _CFArrayApplyFunction
000000000004c3f0 T _CFArrayBSearchValues
000000000004a89c T _CFArrayCreate
```

---

### dyld_shared_cache_util

Shared cache inspector and extractor. Lists, inspects, and extracts dylibs from the dyld shared cache.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_shared_cache_util -o /usr/local/bin/dyld_shared_cache_util && chmod +x /usr/local/bin/dyld_shared_cache_util
```

```
Usage: dyld_shared_cache_util <command> [-fs-root] [-inode] [-versions] [-vmaddr] [shared-cache-file]
    -list [-uuid] [-vmaddr]                  list images
    -dependents <dylib-path>                 list dependents of dylib
    -linkedit                                print linkedit contents
    -info                                    print shared cache info
    -stats                                   print size stats
    -slide_info                              print slide info
    -fixups_in_dylib <dylib-path>            print fixups in dylib
    -text_info                               print locations of TEXT segments
    -local_symbols                           print local symbols and locations
    -strings                                 print C strings in images
    -sections                                print summary of section sizes
    -exports                                 list exported symbols in images
    -duplicate_exports                       list symbols exported by multiple images
    -map                                     print map of segment locations
    -json-map                                print map of segment locations in JSON
    -json-dependents                         print dependents in JSON format
    -size                                    print the size of each image
    -objc-info                               print summary of ObjC content
    -objc-protocols                          list ObjC protocols
    -objc-imp-caches                         print contents of ObjC method caches
    -objc-classes                            print ObjC class names and methods in JSON
    -objc-class-layout                       print size, start offset, and ivars of ObjC classes
    -objc-selectors                          print all ObjC selector names and locations in JSON
    -swift-proto                             print Swift protocol conformance table
    -extract <directory>                     extract images into the given directory
    -patch_table                             print symbol patch table
    -list_dylibs_with_section <seg> <sect>   list images with a given section
    -mach_headers                            summarize mach header of each image
    -load_commands                           summarize load commands of each image
    -cache_header                            print header of each shared cache file
    -dylib_symbols                           print all symbol names and locations
    -function_starts                         print address of beginning of each function
```

**Examples:**

```sh
# List all dylibs in the shared cache
$ dyld_shared_cache_util -list | head -5
/usr/lib/libobjc.dylib
/usr/lib/libobjc.A.dylib
/usr/lib/dyld
/usr/lib/system/libsystem_blocks.dylib
/usr/lib/system/libxpc.dylib

# Show shared cache info (UUID, size, mappings)
$ dyld_shared_cache_util -info | head -8
uuid: 87B95872-5E6E-3168-A703-5F00925E0AB2
platform: macOS
built by: B&I
cache type: development
image count: 3257
ASLR entropy: 17-bits (512MB)
mappings:
              __TEXT 1663MB, ...

# Find all dylibs that depend on AppKit
$ dyld_shared_cache_util -dependents /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit | head -5
	/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
	/System/Library/PrivateFrameworks/UIFoundation.framework/Versions/A/UIFoundation
	/System/Library/PrivateFrameworks/CollectionViewCore.framework/Versions/A/CollectionViewCore
	/System/Library/PrivateFrameworks/XCTTargetBootstrap.framework/Versions/A/XCTTargetBootstrap
	/System/Library/PrivateFrameworks/InternationalSupport.framework/Versions/A/InternationalSupport

# Extract all dylibs from the cache to a directory
$ dyld_shared_cache_util -extract /tmp/cache-extract
```

---

### dyld_closure_util

View and create dyld launch closures.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_closure_util -o /usr/local/bin/dyld_closure_util && chmod +x /usr/local/bin/dyld_closure_util
```

```
Usage: dyld_closure_util <mode> [options]
  mode:
    -create_closure <prog-path>            create a closure for the specified main executable
    -list_dyld_cache_closures              list all launch closures in the dyld shared cache
    -print_dyld_cache_closure <prog-path>  find closure for specified program in dyld cache, print as JSON
    -print_dyld_cache_dylib <dylib-path>   print specified cached dylib as JSON
    -print_dyld_cache_dylibs               print all cached dylibs as JSON
    -print_closure_file <closure-path>     print specified program closure as JSON
  options:
    -cache_file <cache-path>               path to cache file to use (default: current cache)
    -build_root <path-prefix>              path prefix when runtime volume is not current boot volume
    -env <var=value>                       DYLD_* env vars to assume
    -verbose_fixups                        force printing fixups
    -no_at_paths                           simulate security not allowing @path expansion
    -no_fallback_paths                     simulate security not allowing default fallback paths
    -allow_insertion_failures              simulate allowing unloadable DYLD_INSERT_LIBRARIES to be ignored
```

**Examples:**

```sh
# Print cached dylib info as JSON
$ dyld_closure_util -print_dyld_cache_dylib /usr/lib/libobjc.A.dylib | head -10
    {
      "path":    "/usr/lib/libobjc.A.dylib",
      "loader":  "c.0",
      "vm-size": "0xA5598000",
      "overridable": "true",
      "supports-catalyst": "true",
      "catalyst-override": "false",
      "has-initializers": "false",
      "segments": [
        {

# List all launch closures in the cache
$ dyld_closure_util -list_dyld_cache_closures | head -5
```

---

### machocheck

Mach-O validation tool. Checks binaries against Apple's OS Mach-O format rules.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/machocheck -o /usr/local/bin/machocheck && chmod +x /usr/local/bin/machocheck
```

```
Usage: machocheck -verifier_dstroot <root-path> [-verifier_error_list <path>] <mach-o file>+
```

**Examples:**

```sh
# Validate all Mach-O binaries under a directory
$ machocheck -verifier_dstroot /usr/lib /usr/lib/dyld
```

---

### dyld_inspect

Query shared cache info (UUID, base address) for running processes.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_inspect -o /usr/local/bin/dyld_inspect && chmod +x /usr/local/bin/dyld_inspect
```

```
Usage: dyld_inspect <options>* [ -p pid | -all | -all_installed_caches ]
    -shared_cache_uuid       print shared cache UUID
    -shared_cache_address    print shared cache base address
    -shared_cache            print all shared cache options
```

**Examples:**

```sh
# List all installed shared caches with their UUIDs and base addresses
$ dyld_inspect -shared_cache -all_installed_caches
/.../dyld_shared_cache_arm64e  87B95872-5E6E-3168-A703-5F00925E0AB2  0x180000000
/.../dyld_shared_cache_x86_64  E990F6A4-056D-3A06-A97F-7C5D45647D3B  0x7ff800000000

# Get shared cache info for a specific process
$ dyld_inspect -shared_cache -p <pid>
```

---

### dyld_usage

Trace dyld events via kernel tracing. Requires root and `libktrace`.

```sh
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_usage -o /usr/local/bin/dyld_usage && chmod +x /usr/local/bin/dyld_usage
```

```sh
# Trace all dyld events (must be run as root)
$ sudo dyld_usage
```

---

## Why?

Starting with macOS Big Sur, most system libraries live exclusively in the dyld shared cache and no longer exist as files on disk. Standard tools like `nm`, `otool`, and `file` can't inspect them:

```
$ nm /System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote
nm: error: No such file or directory

$ dyld_nm -g /System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote
/System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote [arm64e]:
0000000000062220 T _MRAVAirPlaySecuritySettingsCreate
...
```

## Building from source

Requires Xcode command line tools on macOS. Works on both arm64 and x86_64.

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The first configure fetches corecrypto headers from [apple-oss-distributions/xnu](https://github.com/apple-oss-distributions/xnu) (shallow clone, cached in `build/`). Binaries are in `build/`.

## License

The dyld source code is copyright Apple Inc., licensed under the [Apple Public Source License 2.0](APPLE_LICENSE). The CMake build system and stub headers in `stubs/` are additions to enable building outside of Apple's internal build environment.

# dyld tools

Pre-built macOS (arm64) CLI utilities from Apple's open-source [dyld](https://github.com/apple-oss-distributions/dyld) project. These tools inspect Mach-O binaries and the dyld shared cache — including libraries that have been removed from disk and merged into the cache, where standard tools like `nm` and `otool` fail.

Built from **dyld-1340** source with a CMake build system and stub headers for private SDK dependencies.

## Install

Download all tools:

```sh
cd /usr/local/bin && for tool in dyld_info dyld_nm dyld_shared_cache_util dyld_closure_util machocheck dyld_inspect dyld_usage; do
  curl -fLO "https://github.com/jclay/dyld-tools/releases/latest/download/$tool" && chmod +x "$tool"
done
```

Or grab just the ones you need:

```sh
# dyld_info — Mach-O binary inspector (platform, segments, fixups, exports, etc.)
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_info -o /usr/local/bin/dyld_info && chmod +x /usr/local/bin/dyld_info

# dyld_nm — symbol table viewer (works on shared cache dylibs)
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_nm -o /usr/local/bin/dyld_nm && chmod +x /usr/local/bin/dyld_nm

# dyld_shared_cache_util — shared cache inspector/extractor
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_shared_cache_util -o /usr/local/bin/dyld_shared_cache_util && chmod +x /usr/local/bin/dyld_shared_cache_util

# dyld_closure_util — closure viewer/creator
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_closure_util -o /usr/local/bin/dyld_closure_util && chmod +x /usr/local/bin/dyld_closure_util

# machocheck — Mach-O validation
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/machocheck -o /usr/local/bin/machocheck && chmod +x /usr/local/bin/machocheck

# dyld_inspect — shared cache info for running processes
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_inspect -o /usr/local/bin/dyld_inspect && chmod +x /usr/local/bin/dyld_inspect

# dyld_usage — kernel trace-based dyld event viewer
curl -fL https://github.com/jclay/dyld-tools/releases/latest/download/dyld_usage -o /usr/local/bin/dyld_usage && chmod +x /usr/local/bin/dyld_usage
```

> **Note:** Pre-built binaries are arm64 (Apple Silicon) only. x86_64 users can [build from source](#building-from-source).

## Tools

| Tool | Description |
|------|-------------|
| **dyld_info** | Inspect Mach-O binaries: platforms, segments, fixups, exports, linked dylibs |
| **dyld_nm** | Symbol table viewer that works on shared-cache dylibs (where `nm` fails) |
| **dyld_shared_cache_util** | List, inspect, and extract dylibs from the dyld shared cache |
| **dyld_closure_util** | View and create dyld launch closures |
| **machocheck** | Validate Mach-O binaries against format rules |
| **dyld_inspect** | Query shared cache info (UUID, address) for running processes |
| **dyld_usage** | Trace dyld events via kernel tracing (requires `libktrace`) |

## Examples

```sh
# List all dylibs in the shared cache
dyld_shared_cache_util -list

# Dump symbols from a framework merged into the shared cache
dyld_nm -g /System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote

# Show platform/SDK info for a binary
dyld_info -platform /usr/lib/dyld

# List exports from a shared-cache dylib
dyld_info -exports /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation
```

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

Binaries are in `build/`.

## License

The dyld source code is copyright Apple Inc., licensed under the [Apple Public Source License 2.0](APPLE_LICENSE). The CMake build system and stub headers in `stubs/` are additions to enable building outside of Apple's internal build environment.

/* Stub for private dyld/VersionMap.h
 * Provides the version mapping table used by DyldProcessConfig.cpp
 * to convert between platform versions (macOS 13 <-> iOS 16, etc.)
 */
#ifndef _DYLD_VERSIONMAP_H_STUB
#define _DYLD_VERSIONMAP_H_STUB

#include <stdint.h>

namespace dyld3 {

struct VersionSetEntry {
    uint32_t set;       // version set identifier
    uint32_t macos;
    uint32_t ios;
    uint32_t watchos;
    uint32_t tvos;
    uint32_t bridgeos;
    uint32_t visionos;
};

// Minimal version map table — covers major macOS/iOS alignment points.
// Format: { set, macOS, iOS, watchOS, tvOS, bridgeOS, visionOS }
static const VersionSetEntry sVersionMap[] = {
    { 0x000A0F00, 0x000A0F00, 0x000D0000, 0x00060000, 0x000D0000, 0x00030000, 0x00000000 }, // 10.15 / 13
    { 0x000B0000, 0x000B0000, 0x000E0000, 0x00070000, 0x000E0000, 0x00040000, 0x00000000 }, // 11.0 / 14
    { 0x000C0000, 0x000C0000, 0x000F0000, 0x00080000, 0x000F0000, 0x00050000, 0x00000000 }, // 12.0 / 15
    { 0x000D0000, 0x000D0000, 0x00100000, 0x00090000, 0x00100000, 0x00060000, 0x00000000 }, // 13.0 / 16
    { 0x000E0000, 0x000E0000, 0x00110000, 0x000A0000, 0x00110000, 0x00070000, 0x00010000 }, // 14.0 / 17
    { 0x000F0000, 0x000F0000, 0x00120000, 0x000B0000, 0x00120000, 0x00080000, 0x00020000 }, // 15.0 / 18
    { 0x00100000, 0x00100000, 0x00130000, 0x000C0000, 0x00130000, 0x00090000, 0x00030000 }, // 16.0 / 19
};

} // namespace dyld3

#endif

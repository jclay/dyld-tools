/* Stub for private System/mach/dyld_pager.h */
#ifndef _SYSTEM_MACH_DYLD_PAGER_H_STUB
#define _SYSTEM_MACH_DYLD_PAGER_H_STUB

#include <stdint.h>
#include <mach/mach.h>
#include <mach/shared_region.h>

/* Page-in linking regions */
typedef struct mwl_region {
    int               mwlr_fd;
    vm_prot_t         mwlr_protections;
    mach_vm_offset_t  mwlr_file_offset;
    mach_vm_address_t mwlr_address;
    mach_vm_size_t    mwlr_size;
} mwl_region;

typedef struct mwl_info_hdr {
    uint32_t          mwli_version;
    uint32_t          mwli_page_size;
    uint32_t          mwli_pointer_format;
    uint32_t          mwli_binds_offset;
    uint32_t          mwli_binds_count;
    uint32_t          mwli_chains_offset;
    uint32_t          mwli_chains_size;
    uint64_t          mwli_slide;
    uint64_t          mwli_image_address;
    uint32_t          mwli_count;
} mwl_info_hdr;

/* __map_with_linking_np — private syscall for page-in linking */
static inline int __map_with_linking_np(const mwl_region *regions __attribute__((unused)),
                                         uint32_t region_count __attribute__((unused)),
                                         const mwl_info_hdr *info __attribute__((unused)),
                                         uint32_t info_size __attribute__((unused))) {
    return -1; /* stub */
}

#endif

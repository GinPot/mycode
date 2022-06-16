/* =========================================================================
Copyright (c) 2010 - 2011 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
$Header$
============================================================================ */
#ifndef _VDEV_MMU_DCMD_H
#define _VDEV_MMU_DCMD_H

#include <devctl.h>
#include <stdint.h>
#include <inttypes.h>

/**
 *  Memory mapping attributes
 */
typedef uint32_t map_attr_t;

#define MAP_ATTR_READ       (1 << 0)
#define MAP_ATTR_WRITE      (1 << 1)

typedef struct _msg_vm_mmu_map {
    union {
        struct
        {
            uint64_t ipaddr;
            uint64_t paddr;
            uint32_t size;
            uint32_t attr;
        }cmd;
        struct
        {
            int retcode;
        }reply;
    };
} msg_vm_mmu_map_t;

typedef struct _msg_vm_mmu_unmap {
    union {
        struct
        {
            uint64_t ipaddr;
            uint32_t size;
        }cmd;
        struct
        {
            int retcode;
        }reply;
    };
} msg_vm_mmu_unmap_t;

#define _DCMD_VM_MMU_MAP                __DIOTF(_DCMD_MISC, 1, msg_vm_mmu_map_t)
#define _DCMD_VM_MMU_UNMAP              __DIOTF(_DCMD_MISC, 2, msg_vm_mmu_unmap_t)

/**
 * Maps a given (IPA, PA) pair into the stage 2 table of a VM.
 *
 * @param [in]  fd       fd 
 * @param [in]  ipaddr   IPA
 * @param [in]  paddr    PA
 * @param [in]  size     Memory region size
 * @param [in]  attr     Mapping attribues
 *
 * @return Zero on success, otherwise error code.
 */
static inline int vm_s2_map(int fd, uint64_t ipaddr, uint64_t paddr, uint32_t size, map_attr_t attr)
{
    int retval;
    msg_vm_mmu_map_t msg;

    msg.cmd.ipaddr = ipaddr;
    msg.cmd.paddr = paddr;
    msg.cmd.size = size;
    msg.cmd.attr = attr;

    if(devctl(fd, _DCMD_VM_MMU_MAP, &msg, sizeof(msg), &retval)!=0){
        return -1;
    }

    return 0;
}

/**
 * Unmaps a given (IPA, PA) pair from the stage 2 table of a VM.
 *
 * @param [in]  fd       fd
 * @param [in]  ipaddr   IPA
 * @param [in]  size     Memory region size
 *
 * @return Zero on success, otherwise error code.
 */
static inline int vm_s2_unmap(int fd, uint64_t ipaddr, uint32_t size)
{
    int retval;
    msg_vm_mmu_unmap_t msg;

    msg.cmd.ipaddr = ipaddr;
    msg.cmd.size = size;

    if(devctl(fd, _DCMD_VM_MMU_UNMAP, &msg, sizeof(msg), &retval)!=0){
        return -1;
    }

    return 0;
}
#endif

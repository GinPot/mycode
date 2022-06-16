/* =========================================================================
Copyright (c) 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
$Header$
============================================================================ */
#ifndef _VDEV_IO_H_
#define _VDEV_IO_H_

#include <devctl.h>
#include <stdint.h>
#include <inttypes.h>

struct _achunk {
    /* Number of continuous 4k pages in this chunk*/
    unsigned n;
    /* Physical address of the first chunk*/
    uint64_t pa;
};

#define _DCMD_REG_IO_FAULT  __DIOT(_DCMD_MISC, 1, void*)

/*
 * _DCMD_REG_IO_FAULT takes 3 vectors.
 * 1.  Channel ID for callback reception. 
 * 2.  Number of chunks.
 * 3.  Array of chunks of type struct _achunk 
 */
static inline int vdev_register_io_fault ( int fd , int chid , 
        unsigned nchunks, const struct _achunk *chunks_p ) 
{
    iov_t wiov[3];

    SETIOV ( &wiov[0] , &chid, sizeof (int));  
    SETIOV ( &wiov[1] , &nchunks, sizeof (unsigned));
    SETIOV ( &wiov[2] , chunks_p , sizeof (struct _achunk) * nchunks);
    return devctlv ( fd , _DCMD_REG_IO_FAULT , 3 , 0 , wiov , NULL , NULL );
}

typedef struct {
    uint64_t addr;
    uint32_t val;
}wio_req_t;

typedef uint64_t rio_req_t;

typedef struct {
    int read;
    union {
        wio_req_t w;
        rio_req_t r;
    };
}io_req_t;

typedef uint32_t rio_resp_t;

#endif

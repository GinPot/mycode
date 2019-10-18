#ifndef _LIBDISCOM_H_
#define _LIBDISCOM_H_

#include <inttypes.h>
#include <devctl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <screen/screen.h>

/*
    Define the actual devctl commnads
    there are four flavors of commands
    __DIOF      Data IO From the driver
    __DIOT      Data IO To the driver
    __DIOTF     Data IO To and From the driver
    __DION      a command with No associated data
*/

#define _DCMD_DISCOM   _DCMD_MISC


#define DCMD_DISCOM_ATTACH              __DIOT (_DCMD_DISCOM, 1, dc_data_packet_t)
#define DCMD_DISCOM_DETACH              __DIOT (_DCMD_DISCOM, 2, dc_data_packet_t)
#define DCMD_DISCOM_SET_EXPECTED_CRC    __DIOT (_DCMD_DISCOM, 3, dc_data_packet_t)
#define DCMD_DISCOM_GET_CURRENT_CRC     __DIOTF (_DCMD_DISCOM, 4, dc_data_packet_t)
#define DCMD_DISCOM_COMPARE_VALUE       __DIOTF (_DCMD_DISCOM, 5, dc_data_packet_t)
#define DCMD_DISCOM_SOURCE_RECTANGLE    __DIOT (_DCMD_DISCOM, 6, dc_data_packet_t)
#define DCMD_DISCOM_START               __DIOT (_DCMD_DISCOM, 7, dc_data_packet_t)
#define DCMD_DISCOM_STOP                __DIOT (_DCMD_DISCOM, 8, dc_data_packet_t)

typedef struct {
    uint32_t    hor_off;    // the horizontal offset
    uint32_t    ver_off;    // the vertical offset 
    uint32_t    hor_size;   // the horizontal size
    uint32_t    ver_size;   // the vertical size
} rect_t;                   // the CRC calculation area in the image


typedef struct
{
    int         pipe;
    rect_t      src;
    uint32_t    exp_crc;    // the expectation value of the CRC code
    uint32_t    cur_crc;    // the CRC value of current image
    _Bool       compare_result;   // the compare result of the CRC code,0->fail;1->success
} dc_data_packet_t;


void Discom_Exit(void);
int Discom_Init(int display_id, int pipeline);
int Get_Discom_CRC(int posx, int posy, int sizex, int sizey);
int Check_Discom_CRC(int posx, int posy, int sizex, int sizey, int expectation); //0->fail;1->success


#endif /* _LIBDISCOM_H_ */

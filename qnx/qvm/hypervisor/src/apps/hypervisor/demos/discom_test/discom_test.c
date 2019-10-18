/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2018, Renesas Electronics Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include "discom_test.h"

#define DEV_NAME "/dev/discom/dpy-"

/* Color prefixes */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

void usage()
{
    fprintf(stderr, "==========================DISCOM_TEST==================\n");
    fprintf(stderr, "Syntax:\n");
    fprintf(stderr, "    # discom_test -cmd=[command] -display=[display_id] -pipeline=[pipe_id] -expectation=[CRC] -pos=[XxY] -size=[HxV]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    Command: 1 - Get CRC current \t 0 - Set CRC expectation\n");
    fprintf(stderr, "Launch examples: \n");
    fprintf(stderr, "    Set CRC expectation and get comparation result: \n");
    fprintf(stderr, "    # discom_test -cmd=0 -display=1 -pipeline=2 -expectation=0xffffffff\n");
    fprintf(stderr, "    Get CRC value: \n");
    fprintf(stderr, "    # discom_test -cmd=1 -display=1 -pipeline=2\n");
}

int main(int argc, char **argv)
{
    int         fd,status = 0, error, i;
    int pipeline=6,display_id=2,cmd=1;
    uint32_t exp_crc=0;
    char *hexValue,*p;
    int size[2] = { 1920, 1080 };
    int pos[2] = { 0, 0 };
    char    name[80];
    char     *tok = NULL;
    int sts;
    dc_data_packet_t data={
        .pipe = pipeline,
        .src.hor_off = pos[0],
        .src.ver_off = pos[1],
        .src.hor_size = size[0],
        .src.ver_size = size[1],
        .exp_crc = 0xffffffff,
        .cur_crc = 0,
        .compare_result = false
    };

    screen_context_t screen_ctx = NULL;
    screen_display_t display = NULL;
    screen_display_t *screen_displays = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            usage();
            return 1;
        } else if (strncmp(argv[i], "-pos=", strlen("-pos=")) == 0) {
            tok = argv[i] + strlen("-pos=");
            pos[0] = atoi(tok);
            while (*tok >= '0' && *tok <= '9') {
                tok++;
            }
            pos[1] = atoi(tok+1);
        } else if (strncmp(argv[i], "-size=", strlen("-size=")) == 0) {
            tok = argv[i] + strlen("-size=");
            size[0] = atoi(tok);
            while (*tok >= '0' && *tok <= '9') {
                tok++;
            }
            size[1] = atoi(tok+1);
        } else if (strncmp(argv[i], "-pipeline=", strlen("-pipeline=")) == 0) {
            tok = argv[i] + strlen("-pipeline=");
            pipeline = atoi(tok);
            data.pipe = pipeline;
        } else if (strncmp(argv[i], "-display=", strlen("-display=")) == 0) {
            tok = argv[i] + strlen("-display=");
            display_id = atoi(tok);
        } else if (strncmp(argv[i], "-cmd=", strlen("-cmd=")) == 0) {
            tok = argv[i] + strlen("-cmd=");
            cmd = atoi(tok);
        } else if (strncmp(argv[i], "-expectation=", strlen("-expectation=")) == 0) {
            tok = argv[i] + strlen("-expectation=");
            data.exp_crc=strtoul(tok, &p, 16);
        } else {
            fprintf(stderr, "invalid command line option: %s\n", argv[i]);
            fprintf(stderr, "Please follow correct syntax\n");
            usage();
            goto fail;
        }
    }

    data.src.hor_size = size[0];
    data.src.ver_size = size[1];
    data.src.hor_off = pos[0];
    data.src.ver_off = pos[1];
    data.pipe = pipeline;

    sts = screen_create_context(&screen_ctx, SCREEN_DISPLAY_MANAGER_CONTEXT);
    if (sts) {
        perror("screen_create_context");
        goto fail;
    }

    int ndisplays = 0;
    sts = screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &ndisplays);
    if (sts) {
        perror("SCREEN_PROPERTY_DISPLAY_COUNT");
        goto fail;
    }

    if ((1 <= display_id) && (display_id <= ndisplays))
    {
        sprintf(name, "%s%d", DEV_NAME, display_id);
    } else {
        fprintf(stderr, "invalid -display_id argument command\n");
        goto fail;
    }
    fd = open( name, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "open dev (%s) failed errno\n",name);
        return -1;
    }

    screen_displays = calloc(ndisplays, sizeof(*screen_displays));
    if (!screen_displays) {
        perror( "could not allocate memory for display list");
        sts = -1;
        goto fail;
    }

    sts = screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)screen_displays);
    if (sts) {
        perror( "SCREE_PROPERTY_DISPLAYS");
        goto fail;
    }

    for (i = 0; i < ndisplays; i++) {
        int val = 0;
        screen_get_display_property_iv(screen_displays[i], SCREEN_PROPERTY_ID, &val);

        if (val == display_id) {
            display = screen_displays[i];
        }
    }

    if ((size[0] == -1) || (size[1] == -1)) {
        screen_get_display_property_iv (display, SCREEN_PROPERTY_SIZE, size);
        data.src.hor_size = size[0];
        data.src.ver_size = size[1];
    }

    printf("\n");
    printf("************************************\n");
    printf("*         Discom test              *\n");
    printf("************************************\n");
    printf("    Display   : %d                  \n",display_id);
    printf("    Pipeline  : %d                  \n",pipeline);
    printf("    Rectangle : [%d, %d, %d, %d]    \n",pos[0],pos[1],size[0],size[1]);
    printf("    Open      : %s                  \n",name);

    /*Attach DISCOM unit*/
    error = devctl (fd, DCMD_DISCOM_ATTACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status )
    {
        fprintf(stderr, "Error setting RTS: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // wait for DPR router to UIF
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /* Set rectangle */
    error = devctl (fd, DCMD_DISCOM_SOURCE_RECTANGLE, &data, sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_SET_RECTANGLE: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    if (cmd == 0)
    {
        /*Set the expectation value of the CRC code*/
        error = devctl (fd, DCMD_DISCOM_SET_EXPECTED_CRC, &data,sizeof(dc_data_packet_t), &status);
        if (error || status)
        {
            fprintf(stderr, "DCMD_DISCOM_SET_EXPECTED_CRC: %s\n", strerror ( error ));
            exit(EXIT_FAILURE);
        }
    }

    /*Execute DISCOM*/
    error = devctl (fd, DCMD_DISCOM_START, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_START: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the beginning of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /*Stop DISCOM */
    error = devctl (fd, DCMD_DISCOM_STOP, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_STOP: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the end of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    if (cmd == 0)
    {
        /*Compare result between the CRC and its expectation*/
        error = devctl (fd, DCMD_DISCOM_COMPARE_VALUE, &data,sizeof(dc_data_packet_t), &status);
        if (error || status)
        {
            fprintf(stderr, "DCMD_DISCOM_COMPARE_VALUE: %s\n", strerror ( error ));
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "    Expect CRC      : %s%#x \n"KWHT, KGRN, data.exp_crc);
        fprintf(stderr, "    Compare result     : %s%s \n"KWHT, KGRN, (data.compare_result) ? "Same" : "Not same");

    } else if (cmd == 1) {
        /*get CRC value of current image*/
        error = devctl (fd, DCMD_DISCOM_GET_CURRENT_CRC, &data,sizeof(dc_data_packet_t), &status);
        if (error || status)
        {
            fprintf(stderr, "DCMD_DISCOM_GET_CURRENT_CRC: %s\n", strerror ( error ));
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "    Current CRC     : %s%#x\n"KWHT, KGRN, data.cur_crc);

    } else {
        close(fd);
        fprintf(stderr, "invalid -cmd argument command\n");
        goto fail;
    }

    /*Detach DISCOM unit*/
    error = devctl (fd, DCMD_DISCOM_DETACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_DETACH: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 1;
fail:
    return -1;
}



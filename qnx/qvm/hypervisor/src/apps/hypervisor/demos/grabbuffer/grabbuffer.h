#ifndef GRABBUFFER_H_
#define GRABBUFFER_H_

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>


typedef enum {
    DISPLAY_BUDDER_ONE,
    DISPLAY_BUDDER_TWO,
    DISPLAY_BUDDER_TOP
} BUF_INDEX;

void *memory_monitor_thread(void *data);
void sendsignal(void);
void* getDisplayBuffer(unsigned int idx);
BUF_INDEX getDisplayBufferIndex(void * pionter);

#endif

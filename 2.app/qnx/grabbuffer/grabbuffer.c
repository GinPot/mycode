/**************************************************************

    <liujm@thundersoft.com> developed 2018年 10月 31日 星期三 14:40:15 CST
    for grab buffer from shared memory ,display on window

**************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/time.h>
//ljm
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>
#define NBUFFERS 2

#include "grabbuffer.h"
static int fps = 0;
struct timeval time1;
struct timeval time2;

struct screenwithaddr {
    screen_buffer_t sb;
    screen_buffer_t grabbuffersb;
    void*  addr;
};

struct screenwithaddr swa[2];

void *qnx_current_front_buffer;

static pthread_cond_t mCond;

void sendsignal(void) {
    pthread_cond_signal(&mCond);
}

void outPutFpsInfo(void) {
    struct  timezone   tz;
    float usecxx = 0, secxx = 0, time = 0;

    if((fps%300) == 0) {
        fps = 1;
        gettimeofday(&time1, &tz);
    } else {
        fps += 1;
        if(300 == fps) {
            gettimeofday(&time2, &tz);
            if(time2.tv_usec >= time1.tv_usec) {
                usecxx = time2.tv_usec-time1.tv_usec;
                secxx = time2.tv_sec - time1.tv_sec;
            } else {
                usecxx = 1000000 + time2.tv_usec-time1.tv_usec;
                secxx = time2.tv_sec - time1.tv_sec -1;
            }
            time = secxx + usecxx/1000000;
            //printf("the fps is%f\n", 300/time);
        }
    }
}

static void initScreenBuffer(void) {
    int index=0;
    for (index = 0; index < DISPLAY_BUDDER_TOP; index++) {
        swa[index].addr = NULL;
        swa[index].sb = NULL;
        swa[index].grabbuffersb = NULL;
    }
}

static void updateScreenBuffer(int index, void* addr, screen_buffer_t buf) {
    if (index >= DISPLAY_BUDDER_TOP) {
        return;
    }
    //printf("update screen buf addr = %p, screen = %p\n", addr, buf);
    swa[index].addr = addr;
    swa[index].sb = buf;
}

static void updategScreenBuffer(int index, screen_buffer_t buf) {
    if (index >= DISPLAY_BUDDER_TOP) {
        return;
    }
    //printf("update grabbuffer index = %d, screen = %p\n", index, buf);
    swa[index].grabbuffersb = buf;
}

static screen_buffer_t getScreenBufferFromAddr(void* addr) {
    int index=0;
    for (index = 0; index < DISPLAY_BUDDER_TOP; index++) {
        if (swa[index].addr == addr) {
            return swa[index].grabbuffersb;
        }
    }
    return NULL;
}

int main (int argc, char *argv[]) {
    int len, index=0;
    unsigned long paddr;
    void *virt_addr = NULL;
    void *old_virt_addr = NULL;
    void *block_start;

    int sizeS[2];
    int size[2];
    int rect[4] = { 0, 0, 100, 100 };
    int rc = -1;
    screen_context_t screen_ctx;
    screen_window_t screen_win;
    screen_buffer_t  frountScreen;
	sleep(13);
    sizeS[0] = 1920;
    sizeS[1] = 1080;
    pthread_cond_init(&mCond, NULL);
    pthread_t thread;
    pthread_create(&thread, NULL, memory_monitor_thread, (void *)NULL);
    sleep(1);
    pthread_cond_wait(&mCond,NULL); //wait for mmap
    initScreenBuffer();

    screen_buffer_t screenbuf[DISPLAY_BUDDER_TOP];

    for(index = 0; index < DISPLAY_BUDDER_TOP; index++) {
        const int width=1920, height=1080, stride=7680;
        const int nbytes = stride*height;

        screen_create_buffer(&(screenbuf[index]));

        screen_set_buffer_property_iv(screenbuf[index], SCREEN_PROPERTY_FORMAT, (const int[]){ SCREEN_FORMAT_RGBA8888 });
        //printf("SCREEN_PROPERTY_FORMAT Erro = %d\n", errno);
        screen_set_buffer_property_iv(screenbuf[index], SCREEN_PROPERTY_BUFFER_SIZE, (const int[]){ width, height });
        //printf("SCREEN_PROPERTY_BUFFER_SIZE Erro = %d\n", errno);
        screen_set_buffer_property_iv(screenbuf[index], SCREEN_PROPERTY_SIZE, (const int[]){ nbytes });
        //printf("SCREEN_PROPERTY_SIZE Erro = %d\n", errno);
        screen_set_buffer_property_iv(screenbuf[index], SCREEN_PROPERTY_STRIDE, (const int[]){ stride });
        //printf("SCREEN_PROPERTY_STRIDE Erro = %d\n", errno);

        void* addr = getDisplayBuffer(index);
        while(NULL == addr) {
            addr = getDisplayBuffer(index);
            if(NULL != addr) {
                break;
            }
            sleep(1);
        }
        updateScreenBuffer(index, addr, screenbuf[index]);
        screen_set_buffer_property_pv(screenbuf[index], SCREEN_PROPERTY_POINTER, (void**)&addr);
        //printf("SCREEN_PROPERTY_POINTER Erro = %d\n", errno);
    }


    if (screen_create_context(&screen_ctx, (SCREEN_APPLICATION_CONTEXT | SCREEN_BUFFER_PROVIDER_CONTEXT))) {
        //printf("screen_context_create %d", errno);
        return EXIT_FAILURE;
    }

    if (screen_create_window(&screen_win, screen_ctx)) {
        //printf("screen_create_window%d", errno);
        return EXIT_FAILURE;
    }

    int usage = SCREEN_USAGE_NATIVE | SCREEN_USAGE_READ | SCREEN_USAGE_WRITE;
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
    //printf("SCREEN_PROPERTY_USAGE Erro = %d\n", errno);

    screen_set_window_property_cv(screen_win,
            SCREEN_PROPERTY_ID_STRING, strlen("sw-vsync-win"), "sw-vsync-win");
    //printf("SCREEN_PROPERTY_ID_STRING Erro = %d\n", errno);


    screen_attach_window_buffers(screen_win, DISPLAY_BUDDER_TOP, screenbuf);
    //printf("screen_attach_window_buffers Erro = %d\n", errno);

    {
        int num_bufs = 0;
        screen_buffer_t win_buf[2] = {NULL};
        screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_RENDER_BUFFER_COUNT, &num_bufs);
        screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void**)win_buf);
        int ii = 0;
        for (ii = 0; ii < num_bufs; ii++) {
            updategScreenBuffer(ii, win_buf[ii]);
        }
    }

    int rects[2] = {1920, 1080};

    while (1) {

        pthread_cond_wait(&mCond,NULL);
        //draw
        virt_addr = qnx_current_front_buffer;
        if ((virt_addr == NULL)||(old_virt_addr == virt_addr))
        {
            continue;
        } else {
            old_virt_addr = virt_addr;
        }
        // outPutFpsInfo();

        frountScreen = getScreenBufferFromAddr(virt_addr);

        screen_post_window(screen_win, frountScreen, 1, rects, SCREEN_WAIT_IDLE);
    }

    pthread_cond_destroy(&mCond);
    for(index = 0; index < DISPLAY_BUDDER_TOP; index++) {
        screen_destroy_buffer(screenbuf[index]);
    }
    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);
    return 0;
}

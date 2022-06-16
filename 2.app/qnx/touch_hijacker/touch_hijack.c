/**
 ** gles2-gears
 ** Windowed gears that uses OpenGL ES 2.X for rendering API.
 **
 ** Features:
 **  - configurable window size through the -size=[width]x[height] option
 **  - adjustable swap interval through the -interval=[n] option;
 **    a swap interval of 0 lets the app run as fast as possible
 **    numbers of 1 or more limit the rate to the number of vsync periods
 **  - application creates itself on the default EGL display
 **  - configurable pixel format
 **  - application exits if power or mode event invalidates EGL surface
 **
 ** Copyright 2010, QNX Software Systems Ltd. All Rights Reserved
 **
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted,
 ** provided that the above copyright notice appear in all copies and that
 ** both that copyright notice and this permission notice appear in
 ** supporting documentation.
 **
 ** This file is provided AS IS with no warranties of any kind.  The author
 ** shall have no liability with respect to the infringement of copyrights,
 ** trade secrets or any patents by this file or any part thereof.  In no
 ** event will the author be liable for any lost revenue or profits or
 ** other special, indirect and consequential damages.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>          /* Header file for isdigit */
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <screen/screen.h>


#include <sys/neutrino.h>
#define MAX_EGL_ATTRIBUTES 64
static screen_window_t create_bg_window(int dims[2], screen_context_t screen_ctx);

//----------------------------------------------------------------------------------
//#define DUMP_IMAGES
#define USE_RGBA888_SURFACE
//#define USE_PBUFFERSURFACE

//-----------------------------------------------------------------------------------

static screen_window_t create_bg_window(int dims[2], screen_context_t screen_ctx)
{
    /* Start by creating the application window and window group. */
    screen_window_t screen_win;
    screen_create_window(&screen_win, screen_ctx);

    //int vis = 0;
    int vis = 1;
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
    int sensitivity = SCREEN_SENSITIVITY_FULLSCREEN;
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SENSITIVITY, &sensitivity );


    int color = 0xffffff00;
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_COLOR, &color);

    // Make the window transparent
    int trans = SCREEN_TRANSPARENCY_SOURCE_OVER;
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_TRANSPARENCY, &trans);

    int rect[4] = { 0, 0, 1, 1 };
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, rect+2);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_SIZE, dims);


    int pos[2] = { -dims[0], -dims[1] };
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_POSITION, pos);

    screen_buffer_t screen_buf;
    screen_create_window_buffers(screen_win, 1);
    screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_buf);
    screen_post_window(screen_win, screen_buf, 1, rect, 0);

    return screen_win;
}

int main(int argc, char *argv[])
{
    screen_context_t screen_ctx;
    screen_event_t screen_ev;
    int val;
    int rc;
    // Note this is display enumeration index, 0 is the first display, 1 the second, 2, the third...not display id like 1, 2, 4
    int disp_id = argc>1?atoi(argv[1]):0; 
    int touch_id = 0;
    int pos[2];
    int src_pos[2];
    int dir;

    rc = screen_create_context(&screen_ctx, SCREEN_DISPLAY_MANAGER_CONTEXT );
    if (rc) {
        perror("screen_create_window");
        return -1;
    }

    int count = 0;
    screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &count);
    screen_display_t *screen_disps = calloc(count, sizeof(screen_display_t));
    if(NULL == screen_disps) {
        perror("calloc failed");
        return -1;
    }
    screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)screen_disps);
    screen_display_t screen_disp = screen_disps[disp_id];
    free(screen_disps);

    int dims[2] = { 0, 0 };
    screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, dims);
    printf("display %d, x:%d, y:%d\n", disp_id, dims[0], dims[1]);

    //int focus = 0;
    //screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_MTOUCH_FOCUS, &focus);
    //printf("focus %s\n", focus?"YES":"NO");

    screen_window_t screen_win = create_bg_window(dims, screen_ctx);
    //printf("create window OK\n");

    // bind window to display
    screen_set_window_property_pv(screen_win, SCREEN_PROPERTY_DISPLAY, (void**)&screen_disp);

    screen_flush_context(screen_ctx, 0);

    //int zorder = 6;
    //rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ZORDER, &zorder);

    rc = screen_create_event(&screen_ev);
    if (rc) {
        perror("screen_create_event");
        return -1;
    }

    while (1) {
        rc = screen_get_event(screen_ctx, screen_ev, -1); // -1 means block until there is event returned
        if (!rc) {
            //printf("got an screen event\n");
            screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TYPE, &val);
            //printf("parsed screen event %d\n", val);
            switch(val) {
                //case SCREEN_EVENT_POINTER:
                case SCREEN_EVENT_MTOUCH_TOUCH:
                case SCREEN_EVENT_MTOUCH_MOVE:
                case SCREEN_EVENT_MTOUCH_RELEASE:
                    screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TOUCH_ID, &touch_id);
                    screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_POSITION, pos);
                    screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_SOURCE_POSITION, src_pos);
                    screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TOUCH_ORIENTATION, &dir);
                    printf("touch_id : %d, pos:(%d,%d), src_pos:(%d, %d), dir:%d\n", touch_id, pos[0], pos[1], src_pos[0], src_pos[1], dir);

                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}




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

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>
#include "SocketFrameListener.h"


#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <sys/time.h>

#define GRABBUFFER_NAME "sw-vsync-win"
#define CLUSTER_NAME "Kanzi"
#define CAMERA_NAME "vcapture-demo"
#define DIAGNOSIS_NAME "Diagnosis"
#define WM_GROUP_NAME "wmgroup"

screen_context_t screen_ctx = NULL;
screen_window_t sw_vsync_win = NULL;
screen_window_t gles_win = NULL;
screen_window_t camera_win = NULL;
screen_window_t diagnosis_win = NULL;
screen_window_t root_screen_win = NULL;
bool screen_mode_full = false;
bool kanzi_screen_mode_full =false;
int win_size_S[2] = { 1920, 1080 };


bool first_post_cluster = true;


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t mCond = PTHREAD_COND_INITIALIZER;
static int condition = 0;


SocketFrameListener *testServer;
void getData(void* data, int len) {
    printf("mainServer data %s\n", (char*)data);
    printf("mainServer len %d\n", len);
    if(NULL == sw_vsync_win) {
        printf("grabbuffer not create!\n");
        return;
    }
    if(memcmp("full_screen", (char*)data, strlen("full_screen")) == 0){
        // full screen mode
        int pos1[2] = {0,0};
        int win_size1[2] = { 1920, 1080 };
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_POSITION, pos1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SIZE, win_size1);

        screen_mode_full = true;

        int visiable =0;
        if(gles_win) {
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_VISIBLE, &visiable);
        }
        if(camera_win){
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
        }


    } else if(memcmp("split_screen", (char*)data, strlen("split_screen")) == 0){
        int pos1[2] = {0,540};
        int win_size1[2] = { 1920, 540 };
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_POSITION, pos1);
        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SIZE, win_size1);

        screen_mode_full = false;

        int visiable =1;
        if(gles_win) {
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_VISIBLE, &visiable);
        }
        if(camera_win){
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
        }
    } else if(memcmp("full_cluster", (char*)data, strlen("full_cluster")) == 0){
        if(!screen_mode_full){
            kanzi_screen_mode_full = true;
            int pos1[2] = {0,0};
            int win_size1[2] = { 1920, 720 };
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_POSITION, pos1);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SIZE, win_size1);

            int camer_win_size[2] = {546, 366 };         /* size of your window */
            int camer_pos2[2] = {687, 177 };             /* position of your window */
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_POSITION, camer_pos2);
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_SIZE, camer_win_size);
            int visiable =0;
            if(sw_vsync_win) {
                screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_VISIBLE, &visiable);
            }
            screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
            return;
        }
    } else if(memcmp("half_cluster", (char*)data, strlen("half_cluster")) == 0){
        if(!screen_mode_full){
            kanzi_screen_mode_full = false;
            int pos[2] = {0, 0};
            int win_size[2] = { 1920, 540 };
            int pos2[2] = {0, 0};
            int win_size2[2] = { 1920, 540 };
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_POSITION, pos);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_POSITION, pos2);
            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SIZE, win_size2);

            int camer_win_size[2] = {410, 275 };         /* size of your window */
            int camer_pos2[2] = {755, 133 };               /* position of your window */
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_POSITION, camer_pos2);
            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_SIZE, camer_win_size);

            int visiable =1;
            if(sw_vsync_win) {
                screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_VISIBLE, &visiable);
            }
            screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
            return;
        }
    } else {
        //do nothing , return
        return;
    }

    int num_bufs = 0;
    screen_buffer_t win_buf[2] = {NULL};
    screen_get_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_RENDER_BUFFER_COUNT, &num_bufs);
    screen_get_window_property_pv(sw_vsync_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void**)win_buf);
    screen_post_window(sw_vsync_win, win_buf[0], 0, NULL, 0);
    screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
}

int getState(int state) {
    return state;
}

static screen_window_t create_app_window(const char *group, int dims[2], screen_context_t screen_ctx)
{
    /* Start by creating the application window and window group. */
    screen_window_t screen_win;

	int err = 0;
	err = screen_create_window(&screen_win, screen_ctx);
	if(err != 0){
			printf("Failed to create screen window\n");
	}
    screen_create_window_group(screen_win, group);

	/* Set window properties
	 *
	 * Set the R/W usage properties since the buffer will be accessed by the CPU.
	 * */
	int usage = SCREEN_USAGE_READ | SCREEN_USAGE_WRITE;
	err = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
	if(err != 0){
			printf("Failed to set usage property\n");
	}

	/* Create window buffer
	 * This is the buffer object where our graphics will go to.
	 * */
	err = screen_create_window_buffers(screen_win, 1);
	if(err != 0){
			printf("Failed to create window buffer\n");
	}

	/* Get buffer dimensions */
	int buffer[2];
	err = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer);
	if(err != 0){
			printf("Failed to get window buffer size\n");
	}

	/* Get the buffer handle */
	screen_buffer_t screen_buf;
	err = screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_buf);
	if(err != 0){
			printf("Failed to get window buffer\n");
	}

	/* Get the buffer pointer
	 *
	 * First we got the buffer object handle, in order to render we need to get
	 * the pointer to the actual buffer where the data to be displayed is stored.
	 * */
	int *ptr = NULL;
	err = screen_get_buffer_property_pv(screen_buf, SCREEN_PROPERTY_POINTER, (void **)&ptr);
	if(err != 0){
			printf("Failed to get buffer pointer\n");
	}

	int stride = 0;
	err = screen_get_buffer_property_iv(screen_buf, SCREEN_PROPERTY_STRIDE, &stride);
	if(err != 0){
			printf("Failed to get buffer stride\n");
	}

	for(int i = 0; i < buffer[1]; i++ , ptr+=(stride/4)){
		for(int j = 0; j < buffer[0]; j++ ){
			ptr[j] = 0x00000000;
		}
	}

	err = screen_post_window(screen_win, screen_buf, 0, NULL, 0);
	if(err != 0){
			printf("Failed to post window\n");
	}

    return screen_win;
}

void *waitNewEvent(void *) {
    printf("enter wait new event thread\n");
    screen_window_t screen_win;
    if (screen_create_context(&screen_ctx, SCREEN_WINDOW_MANAGER_CONTEXT|SCREEN_APPLICATION_CONTEXT)) {
        perror("screen_context_create\n");
        return NULL;
    }

    int val, object_type;

    screen_event_t screen_ev;
    screen_create_event(&screen_ev);
    int i;
    while (1) {
        usleep(10000);
        i++;
        if (1000 < i) {
            i = 2;
        }
        while (!screen_get_event(screen_ctx, screen_ev,0)){

            screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TYPE, &val);
            if (val == SCREEN_EVENT_NONE) {
                break;
            }
            screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_OBJECT_TYPE, &object_type);

            switch (val) {
            case SCREEN_EVENT_CREATE:
            {
                if(SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                    screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, (void **)&screen_win);

                    char window_name[64] = {0};
                    screen_get_window_property_cv(screen_win, SCREEN_PROPERTY_ID_STRING, sizeof(window_name), window_name );
                }
                break;
            }
            case SCREEN_EVENT_CLOSE:
            {
                if(SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                     screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, (void **)&screen_win);

                      if(sw_vsync_win == screen_win) {
                          sw_vsync_win = NULL;
                      }  else if (gles_win == screen_win) {
                          gles_win = NULL;
                      }  else if (camera_win == screen_win) {
                          camera_win = NULL;
                      }  else if (diagnosis_win == screen_win) {
                          diagnosis_win = NULL;
                      }
                     screen_destroy_window(screen_win);
                }
                break;
            }
            case SCREEN_EVENT_POST:
            {
                int flag_value = 0;
                //printf("SCREEN_EVENT_POST\n");
                screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, (void **)&screen_win);

                char window_name[64] = {0};
                screen_get_window_property_cv(screen_win, SCREEN_PROPERTY_ID_STRING, sizeof(window_name), window_name );
                //printf("post window name: %s\n", window_name);

                if(SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                    if(memcmp(GRABBUFFER_NAME, window_name, strlen(GRABBUFFER_NAME)) == 0) {
                        flag_value += 1;
                        sw_vsync_win = screen_win;
                        // screen_join_window_group(sw_vsync_win, WM_GROUP_NAME);
                    } else if (memcmp(CLUSTER_NAME, window_name, strlen(CLUSTER_NAME)) == 0) {
                        flag_value += 1;
                        gles_win = screen_win;
                        // screen_join_window_group(gles_win, WM_GROUP_NAME);
                    }  else if (memcmp(CAMERA_NAME, window_name, strlen(CAMERA_NAME)) == 0) {
                        flag_value += 1;
                        camera_win = screen_win;
                        int visiable = 0;
                        screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                        // screen_join_window_group(camera_win, WM_GROUP_NAME);
                    }  else if (memcmp(DIAGNOSIS_NAME, window_name, strlen(DIAGNOSIS_NAME)) == 0) {
                        flag_value += 1;
                        diagnosis_win = screen_win;
                    }
                    // when i ==1 , screen_get_event is old event , we do not care these event.
                    // just when new event is comming and sw_vsync_win|gles_win|camera_win at lest exist one, exit this thread, composeWindow agin
                    if (1 == i) {
                        break;
                    }
                    if (0 == flag_value) {
                        break;
                    }

                    goto stop;


                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
stop: printf("goto stop , exit\n");
    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);

    pthread_mutex_lock(&mutex);
    printf("condition %d\n", condition);
    condition = 0;
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mutex);

}


void composeWindow() {
    printf("enter composeWindow function\n");
    screen_window_t screen_win;
    if (screen_create_context(&screen_ctx, SCREEN_WINDOW_MANAGER_CONTEXT|SCREEN_APPLICATION_CONTEXT)) {
        perror("screen_context_create\n");
        return;
    }

    int ndisplays = 0;

    screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &ndisplays);

    printf("The display number is %d\n", ndisplays);

    screen_display_t *screen_dpy = (screen_display_t *)calloc(ndisplays, sizeof(screen_display_t));

    screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void**)screen_dpy);

    root_screen_win = create_app_window(WM_GROUP_NAME, win_size_S, screen_ctx);

    int val, object_type;

    screen_event_t screen_ev;
    screen_create_event(&screen_ev);

    while (!screen_get_event(screen_ctx, screen_ev, 0)) {
        screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TYPE, &val);
        if (val == SCREEN_EVENT_NONE) {
            break;
        }
        screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_OBJECT_TYPE, &object_type);

        switch (val) {
        case SCREEN_EVENT_CREATE:
        {
            //printf("SCREEN_EVENT_CREATE\n");
            if (SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, reinterpret_cast<void **>(&screen_win));

                char window_name[64] = {0};
                screen_get_window_property_cv(screen_win, SCREEN_PROPERTY_ID_STRING, sizeof(window_name), window_name );
                //printf("create window name: %s\n", window_name);
            }
            break;
        }
        case SCREEN_EVENT_CLOSE:
        {
            printf("SCREEN_EVENT_CLOSE\n");
            if (SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                 screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, reinterpret_cast<void **>(&screen_win));

                  if (sw_vsync_win == screen_win) {
                      sw_vsync_win = NULL;
                  }  else if (gles_win == screen_win) {
                      gles_win = NULL;
                  }  else if (camera_win == screen_win) {
                      camera_win = NULL;
                  }  else if (diagnosis_win == screen_win) {
                      diagnosis_win = NULL;
                  }
                 screen_destroy_window(screen_win);
            }
            break;
        }
        case SCREEN_EVENT_POST:
        {
            int flag_value = 0;
            printf("SCREEN_EVENT_POST\n");
            screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, reinterpret_cast<void **>(&screen_win));

            char window_name[64] = {0};
            screen_get_window_property_cv(screen_win, SCREEN_PROPERTY_ID_STRING, sizeof(window_name), window_name );
            //printf("post window name: %s\n", window_name);

            if (SCREEN_OBJECT_TYPE_WINDOW == object_type) {
                if (memcmp(GRABBUFFER_NAME, window_name, strlen(GRABBUFFER_NAME)) == 0) {
                    flag_value += 1;
                    sw_vsync_win = screen_win;
                    screen_join_window_group(sw_vsync_win, WM_GROUP_NAME);
                } else if (memcmp(CLUSTER_NAME, window_name, strlen(CLUSTER_NAME)) == 0) {
                    flag_value += 1;
                    gles_win = screen_win;
                    screen_join_window_group(gles_win, WM_GROUP_NAME);
                }  else if (memcmp(CAMERA_NAME, window_name, strlen(CAMERA_NAME)) == 0) {
                    flag_value += 1;
                    camera_win = screen_win;
                    int visiable = 0;
                    screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    screen_join_window_group(camera_win, WM_GROUP_NAME);
                }  else if (memcmp(DIAGNOSIS_NAME, window_name, strlen(DIAGNOSIS_NAME)) == 0) {
                    diagnosis_win = screen_win;

                    if(first_post_cluster){
                        int visiable = 0;
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                        screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
                        usleep(500*1000);
                        first_post_cluster = false;
                        screen_set_window_property_pv(diagnosis_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_dpy[2]);
                        int pos1[2] = {0, 0};
                        int win_size1[2] = { 1920, 1080 };
                        int pos2[2] = {0, 0};
                        int win_size2[2] = { 1920, 1080 };
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_POSITION, pos2);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SIZE, win_size2);

                        visiable = 1;
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                        screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
                    } else {

                        screen_set_window_property_pv(diagnosis_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_dpy[2]);

                        int pos1[2] = {0, 0};
                        int win_size1[2] = { 1920, 1080 };
                        int pos2[2] = {0, 0};
                        int win_size2[2] = { 1920, 1080 };
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_POSITION, pos2);
                        screen_set_window_property_iv(diagnosis_win, SCREEN_PROPERTY_SIZE, win_size2);
                    }
                }

                //  if not the window of cluster grabbuffer and camera then not care
                if (0 == flag_value) {
                    break;
                }

                if (sw_vsync_win || gles_win || camera_win) {
                    int visiable = 0;
                    if (sw_vsync_win) {
                        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }
                    /*screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    if (gles_win) {
                        screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }
                    if (sw_vsync_win) {
                        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }
                    if (camera_win) {
                        screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }*/

                    if (gles_win) {
                        int pos[2] = {0, 0};
                        screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_POSITION, pos);
                        screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_SIZE, win_size_S);
                        if(kanzi_screen_mode_full){
                            printf("...gles_win.....kanzi full\n");
                            int pos2[2] = {0, 0};
                            int win_size2[2] = { 1920, 720 };
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_POSITION, pos2);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size2);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_POSITION, pos2);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SIZE, win_size2);
                        } else {
                            printf("...gles_win.....kanzi not full\n");
                            int pos[2] = {0, 0};
                            int win_size[2] = { 1920, 540 };
                            int pos2[2] = {0, 0};
                            int win_size2[2] = { 1920, 540 };
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_POSITION, pos);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_POSITION, pos2);
                            screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_SIZE, win_size2);
                        }
                    }

                    if (sw_vsync_win) {
                        int pos[2] = {0, 0};
                        screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_POSITION, pos);
                        screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_SIZE, win_size_S);

                        if (screen_mode_full) {
                            int pos1[2] = {0, 0};
                            int win_size1[2] = {1920, 1080 };
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_POSITION, pos1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SIZE, win_size1);
                        } else {
                            int pos1[2] = {0, 540};
                            int win_size1[2] = {1920, 540 };
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_POSITION, pos1);
                            screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_SIZE, win_size1);
                        }
                    }

                    if (camera_win) {

                            //screen_set_window_property_pv(camera_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_dpy[0]);
                            int camer_win_size[2] = {480, 320 };         /* size of your window */
                            int camer_pos2[2] = { 720, 110 };             /* position of your window */

                            int pos1[2] = {0, 0};
                            int win_size1[2] = {720, 480 };
                            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_SOURCE_POSITION, pos1);
                            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_SOURCE_SIZE, win_size1);

                            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_POSITION, camer_pos2);
                            screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_SIZE, camer_win_size);

                    }

                    int pipeline = 5;
                    screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
                    if (gles_win) {
                        pipeline++;
                        screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
                    }
                    if (sw_vsync_win) {
                        pipeline++;
                        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
                    }
                    if (camera_win) {
                        pipeline = 6;
                        screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
                    }

                    int zorder = 0;
                    screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_ZORDER, &zorder);
                    if (gles_win) {
                        zorder++;
                        screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_ZORDER, &zorder);
                    }
                    if (sw_vsync_win) {
                        zorder++;
                        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_ZORDER, &zorder);
                    }
                    if (camera_win) {
                        zorder++;
                        screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_ZORDER, &zorder);
                    }

                    visiable = 1;
                    screen_set_window_property_iv(root_screen_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    if (gles_win) {
                        screen_set_window_property_iv(gles_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }
                    if (sw_vsync_win && !kanzi_screen_mode_full) {
                        screen_set_window_property_iv(sw_vsync_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }
                    if (camera_win != NULL && !screen_mode_full && !kanzi_screen_mode_full) {
                        screen_set_window_property_iv(camera_win, SCREEN_PROPERTY_VISIBLE, &visiable);
                    }

                }
            }
            break;
        }
        default: {
            break;
        }
        }
    }
    screen_flush_context(screen_ctx, SCREEN_WAIT_IDLE);
    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);
}



int
main(int argc, char *argv[]) {
    int rc = -1;
    char root_window_name[64] = {0};
    int grepbuffer_first_flag = 1;

    SocketCallback callback;
    callback.callbackData = getData;
    callback.callbackState = getState;
    testServer = new SocketFrameListener(nullptr, true, callback);
    if (testServer->startListener()) {
        exit(1);
    }

    pthread_cond_init(&mCond, NULL);
    sleep(1);
    pthread_cond_wait(&mCond, NULL);

    while (1) {
        pthread_mutex_lock(&mutex);
        while (condition == 1)
            pthread_cond_wait(&mCond, &mutex);
        condition = 1;
        pthread_mutex_unlock(&mutex);
        composeWindow();
        pthread_create(NULL, NULL, &waitNewEvent, NULL);
    }

    pthread_cond_destroy(&mCond);
    return 0;
}

#include <sys/iomgr.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <screen/screen.h>
#include <sys/dispatch.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>


#define ATTACH_POINT "myname"

struct message_s
{
    uint16_t        val;
};

int (*CameraOnapp)(void);
void (*CameraOffapp)(void);

int main(int argc, char *argv[]) 
{
	name_attach_t *attach;
    /* Create a local name (/dev/name/local/...) */
    if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		
        return EXIT_FAILURE;
    }
   
	printf("Server is ready, chid = %d\n", attach->chid);

	
	void *handle = dlopen( "/lib-camera_lib.so", RTLD_LOCAL );	
	if( NULL == (CameraOnapp = dlsym( handle, "CameraOn" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOn" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose( handle ); 
		return -1; 
	}
	if( NULL == (CameraOffapp = dlsym( handle, "CameraOff" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOffapp" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose( handle ); 
		return -1; 
	}
	
	while(1)
	{
        int                 rcvid;
		struct message_s    msg;

        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
        if (rcvid < 0) {
            perror("MsgReceive");
            break;
        }
		
		if(msg.val == 1)
			CameraOnapp();
		else
			CameraOffapp();
		
		
		printf("msg.val = %d\n", msg.val);
		MsgReply(rcvid, 0, &msg, sizeof(msg));
	}	
	

	
    return 0;
}

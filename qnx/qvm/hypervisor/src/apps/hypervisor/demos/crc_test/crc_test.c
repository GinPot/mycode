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
#include <sys/time.h>


int (*Discom_Init_)(int,int);
int (*Check_Discom_CRC_)(int,int,int,int,int);
int (*Get_Discom_CRC_)(int,int,int,int);
void (*Discom_Exit_)(void);


int main(int argc, char *argv[])
{
	int crc_value;
	void *handle = dlopen( "/root/lib-discom.so", RTLD_LOCAL );	///usr/lib/lib-performancedisplay.so
	if( NULL == (Discom_Init_ = dlsym( handle, "Discom_Init" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOn" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}
	if( NULL == (Check_Discom_CRC_ = dlsym( handle, "Check_Discom_CRC" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOffapp" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}
	if( NULL == (Get_Discom_CRC_ = dlsym( handle, "Get_Discom_CRC" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOn" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}
	if( NULL == (Discom_Exit_ = dlsym( handle, "Discom_Exit" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOffapp" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}

	
	Discom_Init_(2,6);
	
	
	while(1)
	{
		crc_value = Get_Discom_CRC_(496, 1, 948, 53);
		printf("crc_value=%x    ",crc_value);
		
		printf("Compare result     : %d \n", Check_Discom_CRC_(496, 1, 948, 53, crc_value));

		usleep(1000*1000);
	}	
	

	Discom_Exit_();
    return 0;
}

/******************************************************************************
@file    dmesg.c
@brief   Application, dumps dmesg
===========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <hw/inout.h>
#include <stdint.h>
#include <fcntl.h>




struct printk_log {
	_Uint64t ts_nsec;		/* timestamp in nanoseconds */
	_Uint16t len;			/* length of entire record */
	_Uint16t text_len;		/* length of text buffer */
	_Uint16t dict_len;		/* length of dictionary buffer */
	_Uint8t facility;		/* syslog facility */
	_Uint8t flags:5;		/* internal record flags */
	_Uint8t level:3;		/* syslog level */
};

struct char_tmp{
	char c;
};

int main(int argc, char** argv)
{
    int i,j;
    size_t len=1;
    uintptr_t va;
    _Uint64t addr=0;
	struct printk_log *printklog;
	int printloglen = sizeof(struct printk_log);


	if((1 != argc) && (3 != argc)){
        fprintf(stderr,"dmesg <paddr> <len>\n");
		fprintf(stderr,"	eg: dmesg 0xDFF92E180 1048576\n");
        return 0;
    }

	if(1 == argc){
		addr = 0xDFF92E180;
		len = 1048576;
	}else{
		addr = strtoul(argv[1],NULL,0);
		len = strtoul(argv[2],NULL,10);
	}



    va =  mmap_device_io(len, addr);
    if(MAP_DEVICE_FAILED==va){
        fprintf(stderr,"Couldn't map 0x%08x : %s\n",(unsigned int)addr,strerror(errno));
        return -1;
    }
    
    for(i = 0; i < len; ){
		printklog = (struct printk_log *)((uintptr_t)va + i);
		if(i > 8888){
			if((printklog->ts_nsec != 0) && (printklog->text_len != 0))
				printf("[%05ld.%06ld] ", printklog->ts_nsec/1000000000,(printklog->ts_nsec%1000000000)/1000);
			else
			{
				i += printklog->len;
				usleep(88);
				continue;
			}
		}else{
			printf("[%05ld.%06ld] ", printklog->ts_nsec/1000000000,(printklog->ts_nsec%1000000000)/1000);
		}

		for(j=0; j<printklog->text_len; j++)
			printf("%c", ((struct char_tmp *)((uintptr_t)va+i+printloglen+j))->c);
		i += printklog->len;
		printf("\n");
		//printf("%d",i);
	}


    munmap_device_io(va, len);

    return 0;
}

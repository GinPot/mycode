/**************************************************************

  This program connects to the hypervisor shared memory region "testing" from
  the host system. Anything typed at the terminal will be sent to all other
  clients connected to the region and anything typed on the terminals of
  the other clients will be sent to us and displayed.
  <liujm@thundersoft.com> developed 2018年 10月 31日 星期三 14:40:15 CST

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
//ljm
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int
main(int argc, char *argv[]) {
    int fd = -1;
    int len;
    unsigned long paddr;
    void *virt_addr;


    paddr = strtoul(argv[1], 0, 0);
    len = strtoul(argv[2], 0, 0);
    printf("use dump addre len dumpname\n");
    if ((fd = open(argv[3], O_WRONLY | O_CREAT, 0)) < 0) {
        printf("%s open failed\n", argv[3]);
        return -1;
    }
    virt_addr = mmap_device_memory( 0, len, PROT_READ|PROT_WRITE|PROT_NOCACHE, 0, paddr );
    if ( virt_addr == MAP_FAILED ) {
        perror( "mmap_device_memory for physical address 0xb8000 failed" );
        exit( EXIT_FAILURE );
    }

    if (write(fd, virt_addr, len) != len)
        printf("write failed\n");

    printf("destroyed!\n");
    munmap_device_memory( paddr,
            len );
    close(fd);

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int
main(int argc, char **argv)
{
    int const fd = shm_open("test", O_RDWR | O_CREAT, 0600);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    size_t const size = 100 * 1024 * 1024;
    if (ftruncate(fd, size) == -1) {
        perror("shm_open");
        return 1;
    }

    void * const ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    memset(ptr, 'a', size);
    return 0;
}


/*
cat /proc/vm/stats
cache_swap=0x11e (1.117MB)
pages_swap=0x37c (3.484MB)

./cacheswap_test
cat /proc/vm/stats
cache_swap=0x651e (101.117MB)
pages_swap=0x677c (103.484MB)
*/
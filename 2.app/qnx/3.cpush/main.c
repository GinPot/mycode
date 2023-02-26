#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>

extern void aarch64_cache_flush ( void ) ;

int main(int argc, char **argv) {
	unsigned	i;
	unsigned	runmask;

	ThreadCtl(_NTO_TCTL_IO_PRIV, 0);

	for(i = 0; i < _syspage_ptr->num_cpu; ++i ) {
		runmask = 1 << i;
		ThreadCtl(_NTO_TCTL_RUNMASK, (void *)(uintptr_t)runmask);
		aarch64_cache_flush  ( ) ;
	}
	return 0;
}

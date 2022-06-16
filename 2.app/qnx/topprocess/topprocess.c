/*
 * This is a utility program to demonstrate where _all_ of the memory of the system is being used
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "showmem.h"

int debugfd = -1;
int verbose = 0;



int main(int argc, char **argv) {
	struct showmem_opts opts;
	system_entry_t		*system;
	int 				*pidlist= NULL;
	int 				pidnum= 0; 


	system = build_block_list(pidlist, pidnum);

	display_overview(system, &opts);
	
	return 0;
}

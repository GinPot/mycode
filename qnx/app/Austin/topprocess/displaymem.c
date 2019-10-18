/**
 * This file is responsible for providing the various types of display
 * feedback to the user about the mappings in the system.
 */
 
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>

#include "showmem.h"

struct io_data {
 	FILE 				*output;
	system_entry_t 		*system;
	struct showmem_opts *opts;
	uint64_t			total_private;
	uint64_t			total_shared;
};

char *formatValue(long value) {
	static char data[20];
	static char endfix;
	
	if(value > (1024 * 1024)) {
		value /= (1024 * 1024);
		endfix = 'M';
	} else if (value > 1024) {
		value /= 1024;
		endfix = 'k';
	} else {
		endfix = 'b';
	}
	
	sprintf(data, "%ld%c", value, endfix); 	

	return data;
}

static char *get_basename(char *name) {
	char *p;

	if((p = strrchr(name, '/')) != NULL) {
		p++;
	} else {
		p = name;
	}
	return p;
}

/**
 * Dump the buffer content in a consistant manner.  Return the total value accumulated
 * by the line. Current formatting is:
 *  Total | Code | Data | Heap | Stack | Other
 *
 * This also has a flag to indicate if the RAM was system ram or not
 */
static uint64_t dump_totals(FILE *fp, uint64_t *totals, int *isram, int len, int accumulate, int pid, char *name, char *subname) {
	uint64_t 	sum = 0LL;
	int 		i;
	char		cs, ce;
		
	for(i = 0; i < len; i++) {
		sum += totals[i];
	}

	if(fp == NULL) {
		return sum;
	}

	return sum;
}

static shared_memblock_t *get_shared_block(system_entry_t *system, int index) {
	if(index < 0 || index >= system->shared_blocks.block_count) {
		return NULL;
	}
	
	return &system->shared_blocks.memblocks[index];
}

/**
 * This reports the data structure associated with a particular process.  It
 * provides the report in a two fold manner:
 * [Process Summary: Total | Code | Data | Heap | Stack | Other]
 * [Per Library:     ----- | XXXX | YYYY | ---- | ----- | -----]
 * 
 * The Process Summary, for the code/data portion should represent the
 * sum of the library (and executable) components underneath them.  The
 * "billing" of code/data is reported for the first instance of a library
 * or executable but is not billed for future reported processes.
 * 
 * The Total column is the summary of the Code/Data/Heap/Stack/Other
 * entries
 */ 
int walk_process_overview_callback(pid_entry_t *pmb, void *data) {
	uint64_t		totals[TYPE_MAX];
	uint64_t		ret;
	int				isram[TYPE_MAX];
	struct io_data 	*io = (struct io_data *)data;
	priv_memblock_t		*mi;
	shared_memblock_t 	*smi;
	char			*name = NULL;
	char 			*unknown = "unknown";
	char 			*procnto = "procnto";
	int 			type, i = 0;
	
	name = pmb->name;
	if(name == NULL) {
		name = (pmb->pid == 1) ? procnto : unknown;
	} else {
		name = get_basename(name);
	}
	
	memset(totals, 0, TYPE_MAX * sizeof(*totals));

	//Add all of the private donations this process has made
	for(i = 0; i < pmb->block_count; i++) {
		mi = &pmb->memblocks[i];

		if(!IS_RAM_CONSUMING(mi->memblock.flags)) {
			continue;
		}

		type = (mi->memblock.type < TYPE_MAX) ? mi->memblock.type : TYPE_UNKNOWN;
		totals[type] += mi->memblock.size;
	}
	
	//Include the unique shared library components (shared but not really)
	for(i = 0; i < io->system->shared_blocks.block_count; i++) {
		smi = &io->system->shared_blocks.memblocks[i];
		if(smi->pid != pmb->pid) {
			continue;
		}

		if(!IS_RAM_CONSUMING(smi->memblock.flags)) {
			continue;
		}

		type = (smi->memblock.type < TYPE_MAX) ? smi->memblock.type : TYPE_UNKNOWN;
		totals[type] += smi->memblock.size;
	}
	
	//Summarize the total process information 

	ret = dump_totals(io->output, totals, NULL, TYPE_MAX, 1, pmb->pid, name, NULL);

	if(ret > procedata[0].total)
	{
		procedata[2].total = procedata[1].total;
		procedata[2].pid =   procedata[1].pid; 
		procedata[2].name =	 procedata[1].name;			
		
		procedata[1].total = procedata[0].total;
		procedata[1].pid =   procedata[0].pid; 
		procedata[1].name =	 procedata[0].name;		
		
		procedata[0].total = ret;
		procedata[0].pid = pmb->pid;
		procedata[0].name = name;
	}
	else
	if(ret > procedata[1].total)
	{
		procedata[2].total = procedata[1].total;
		procedata[2].pid =   procedata[1].pid; 
		procedata[2].name =	 procedata[1].name; 
		
		procedata[1].total = ret;
		procedata[1].pid = pmb->pid;
		procedata[1].name = name;
	}
	else
	if(ret > procedata[2].total)
	{
		procedata[2].total = ret;
		procedata[2].pid = pmb->pid;
		procedata[2].name = name;
	}	
	
	//printf("%d  		%d  		%s\n",ret,pmb->pid,name);	
	return 0;
}




void display_overview(system_entry_t *root, struct showmem_opts *opts) {
	struct io_data io;
	int i;
	
	memset(&io, 0, sizeof(io));
	io.output = stdout;
	io.system = root;
	io.opts = opts;
	

	//fprintf(stdout, "Process listing (Total, Code, Data, Heap, Stack, Other)\n");

	iterate_processes(root, walk_process_overview_callback, &io);	
	for(i=0; i<3; i++)
		printf("-------%d  		%d  		%s\n",procedata[i].total,procedata[i].pid,procedata[i].name);

}

void iterate_processes(system_entry_t *root, 
                       int (* func)(pid_entry_t *block, void *data), void *data) {
	int ret;
	pid_entry_t *item;
	
	for(item = root->pid_list; item != NULL; item = item->next) {
		ret = func(item, data);
		if(ret < 0) {
			break;
		}
	}
}

              

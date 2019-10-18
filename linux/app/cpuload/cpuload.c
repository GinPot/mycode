#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int util_get_cpu_usage(double *cpu_usage)
{
	static FILE *fp = NULL;
	char buf[256];
	char charcpu[4];
	uint64_t tot;
	uint64_t u, n, s, i, w, x, y, z;
	static uint64_t last_i[5] = {0}, last_total[5] = {0};
	int j;


	if (!fp) {
		if (!(fp = fopen("/proc/stat", "r"))){
			fprintf(stderr, "Failed /proc/stat open: %s", strerror(errno));
			return -1;
		}
	}
	
	rewind(fp);
	fflush(fp);
	for(j=0; j<5; j++)
	{
		if (!fgets(buf, sizeof(buf), fp)) {
			fprintf(stderr, "failed /proc/stat read\n");
			return -1;
		} else {
			sscanf(buf, "%s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
					charcpu,
					&u,
					&n,
					&s,
					&i,
					&w,
					&x,
					&y,
					&z
				  );
			if (last_total[j] == 0) {
				last_total[j] = u+n+s+i+w+x+y+z;
				last_i[j] = i;
				usleep(30000);
			} else {
				tot = u+n+s+i+w+x+y+z;
				cpu_usage[j] = (1.0 - ((double)(i-last_i[j])/(double)(tot-last_total[j])));
				last_i[j] = i;
				last_total[j] = tot;
			}
		}
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	double cpu_load[5];
	int j,ret;

	while(1)
	{
		ret = util_get_cpu_usage(cpu_load);
		if(ret == 0){
			printf("\n cpuload = %.1f\n", ((1-cpu_load[0])*100));
			for(j=1; j<5; j++)
				printf("	cpu[%d] = %.1f\n", j,((1-cpu_load[j])*100));
		}

		sleep(3); 
	}

	return 0;
}

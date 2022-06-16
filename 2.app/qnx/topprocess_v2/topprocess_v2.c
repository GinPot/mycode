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

struct QnxTopProce {
	char *name;
	float total;
};
struct QnxTopProce qnxprocedata[50],qnxprocedata_tmp;
char str1[10], str2[50][30], buff[120];

int main(int argc, char *argv[]) 
{
	int j,i,fd,top_cn=0;
		
	system("ps -A -o \"sz,args=Command (with args)\" > /root/topprocess.txt");
	fd = fopen("/root/topprocess.txt","r");
	
	while(1)
	{
		if (!fgets(buff, sizeof(buff), fd))
			break;
			//printf("Failed fgets /home/0/topprocess.txt\n");
		
		sscanf(buff,"%s%s", str1, str2[top_cn]);
		qnxprocedata[top_cn].total = atoi(str1);
		qnxprocedata[top_cn].name = str2[top_cn];

		top_cn++;
	}
	
	for(i=0;i<top_cn;i++)
	{
		for(j=i+1;j<top_cn;j++)
		{
			if(qnxprocedata[i].total<qnxprocedata[j].total)
			{
				qnxprocedata_tmp = qnxprocedata[i];
				qnxprocedata[i] = qnxprocedata[j];
				qnxprocedata[j] = qnxprocedata_tmp;
			}
		}
	}	
	
	
	for(i=0; i<5; i++)
		printf("%.1fM  %s\n",qnxprocedata[i].total/1024,qnxprocedata[i].name);
	
	//printf("end\n");
		

	fclose(fd);
    return 0;
}

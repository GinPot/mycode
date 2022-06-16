#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "topprocess.h"


int TopCpuPro(struct TopProce *procedata)
{
	FILE * fd;
	int i;
	char str4buff[100];
	char buff[120], *procname=NULL;
	char str1[10], str2[20], str3[10], str4[20], str5[200];
	
    if((fd = popen("ps aux | awk '{print $3\"   \"$6/1024 \" MB\t\" $2\"   \"$11}' | sort -rn | head", "r")) == NULL) {
      printf("popen error");
      return -1;
    }
	
	for(i=0; i<PROCESSNUM; i++){
		if (!fgets(buff, sizeof(buff), fd)){
			if(i == 0){
				printf("fgets fail\n");
				pclose(fd);
				return -1;
			}
		}
		//printf("%s\n",buff);

		sscanf(buff,"%s %s %s %s %s", str1, str2, str3, str4, str5);
		
		procedata[i].pid = atoi(str4);
		procedata[i].cpuload = atof(str1);
		procedata[i].memoryload = atof(str2);

		procname = strrchr(str5, '/');
		if(procname){
			procname += 1;
			memset(procedata[i].name, 0, sizeof(procedata[i].name));
			if(strlen(procname) >= 15){
				memcpy(procedata[i].name, procname, sizeof(procedata[i].name));
				procedata[i].name[14] = '\0';
			}else{
				memcpy(procedata[i].name, procname, strlen(procname));
				procedata[i].name[strlen(procname)] = '\0';
			}
		}else{
			memset(procedata[i].name, 0, sizeof(procedata[i].name));
			if(strlen(str5) >= 15){
				memcpy(procedata[i].name, str5, sizeof(procedata[i].name));
				procedata[i].name[14] = '\0';
			}else{
				memcpy(procedata[i].name, str5, strlen(str5));
				procedata[i].name[strlen(str5)] = '\0';
			}
		}

		//printf("%.1f %.1fMB %d %s \n",procedata[i].cpuload,procedata[i].memoryload,procedata[i].pid,procedata[i].name);
	}




	pclose(fd);
	return 0;
}

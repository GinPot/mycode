#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "topprocess.h"




int GetTopPro(struct ToQnxProce *qnxtopdata) 
{
	FILE* fd;
	int i,j,top_cn=0;
	struct ToQnxProce qnxprocedata_tmp;
	char buff[150], str1[10], str2[10], str3[5], str4[20];
	

	memset(qnxtopdata, 0, 7*sizeof(qnxtopdata[0]));
	
    if((fd = popen("ps -A -o \"pid,sz,pcpu,args\"", "r")) == NULL) {
      perror("popen error");
      return -1;
    }	
	
	while(1)
	{
		if (!fgets(buff, sizeof(buff), fd))
		{
			if(top_cn == 0){
				printf("fgets fail\n");
				pclose(fd);
				return -1;
			}
			else{
				break;
			}
		}
		
		if(top_cn > 1)
		{
			//printf("%s\n", buff);
			sscanf(buff,"%s %s %s %s", str1, str2, str3, str4);
            
			qnxtopdata[6].pid = atoi(str1);
			qnxtopdata[6].cpuload = (float)atoi(str3)/6;
			qnxtopdata[6].memoryload = (float)atoi(str2)/1024;
			memset(qnxtopdata[6].name, 0, strlen(qnxtopdata[6].name));
			if(strlen(str4) > 15){
				memcpy(qnxtopdata[6].name, str4, sizeof(qnxtopdata[6].name));
			}else{
				memcpy(qnxtopdata[6].name, str4, strlen(str4));
			}
			
			
			//printf("%d       %.1f       %.1f       %s\n", qnxtopdata[6].pid, qnxtopdata[6].cpuload, qnxtopdata[6].memoryload, qnxtopdata[6].name);
			
			for(i=0;i<7;i++)
			{
				for(j=i+1;j<7;j++)
				{
					if(qnxtopdata[i].cpuload<qnxtopdata[j].cpuload)
					{
						memcpy(&qnxprocedata_tmp, &qnxtopdata[i], sizeof(struct ToQnxProce));
						memcpy(&qnxtopdata[i], &qnxtopdata[j], sizeof(struct ToQnxProce));
						memcpy(&qnxtopdata[j], &qnxprocedata_tmp, sizeof(struct ToQnxProce));
					}
				}
			}			
		}else{
			top_cn++;
		}
	}

	pclose(fd);
	return 0;
}

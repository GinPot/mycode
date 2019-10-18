#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROCESSNUM	5

struct TopProce {
	int pid;
	float cpuload;
	float memoryload;
	char name[15];
};

struct TopProce procedata[PROCESSNUM];

static void trim_string(char *str)
{
	int len = strlen(str);
 
	//去掉最后的换行符
	if(str[len-1] == '\n')
	{
		len--;		//字符串长度减一
		str[len] = 0;	//给字符串最后一个字符赋0，即结束符
	}
}

int main(int argc, char *argv[])
{
	FILE * fd;
	int i;
	char buff[120], *procname;
	char str1[5], str2[10], str3[2], str4[10], str5[100];
	
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
		procname = str5;

		procname = strrchr(procname, '/')+1;
		trim_string(procname);
		memset(procedata[i].name, 0, sizeof(procedata[i].name));
		memcpy(procedata[i].name, procname, strlen(procname));
		
		printf("%.1f %.1fMB %d %s \n",procedata[i].cpuload,procedata[i].memoryload,procedata[i].pid,procedata[i].name);
	}




	pclose(fd);
	return 0;
}
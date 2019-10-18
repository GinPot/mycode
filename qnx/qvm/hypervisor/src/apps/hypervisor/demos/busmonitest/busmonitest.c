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


struct BusMoniMemb {
	char name[35];
	int rdata;
	char rdatacom[4];
	int wdata;
	char wdatacom[4];
};
struct BusMoniMemb busmonidata[9];

int main(int argc, char *argv[]) 
{
	int len,i,fd,fdddr,status,top_cn=0;
	char str1[10],str2[35],str3[10],str4[10],str5[10],str6[10],str7[10],str8[10],str9[10],str10[10], buff[100];
	FILE* f;

	while(1){
	top_cn = 0;
    if((f = popen("/usr/bin/busmoni_app -p UC22 > /root/busmoni.txt;/usr/bin/busmoni_app -p UC11  > /root/busmoniddr.txt", "r")) == NULL) {
      perror( "popen error" );
      return -1;
    }
	pclose(f);
	
    //if((f = popen("/usr/bin/busmoni_app -p UC11  > /root/busmoniddr.txt", "r")) == NULL) {
    //  perror( "popen error" );
    //  return -1;
    //}
	//pclose(f);	

	fd = fopen("/root/busmoni.txt","r");
    if( fd == NULL ) {
		printf("fopen fail\n");	
        return -1; 
    }

	fdddr = fopen("/root/busmoniddr.txt","r");
    if( fd == NULL ) {
		printf("fopen fail\n");
		fclose(fd);
        return -1; 
    }

		while(1)
		{
			if (!fgets(buff, sizeof(buff), fd))
			{
				if(top_cn == 0){
					printf("fgets fail\n");	
					fclose(fd);
					fclose(fdddr);
					return -1;
				}
				else{
					break;
				}
			}
			
			if((top_cn == 4) || (top_cn == 5) || (top_cn == 12) || (top_cn == 18) || (top_cn == 31) || (top_cn == 21) || (top_cn == 22) || (top_cn == 26))
			{
				sscanf(buff,"%s%s%s%s%s%s", str1, str2, str3, str4, str5, str6);
				//printf("%s %s %s %s %s\n", str2, str3, str4, str5, str6);
				
				len = strlen(str2);
				memcpy(busmonidata[top_cn].name, str2, len);
				busmonidata[top_cn].name[len] = '\0';
				//printf("%s=%d %s=%d\n\n", busmonidata[top_cn].name,strlen(busmonidata[top_cn].name), str2,strlen(str2));
				
				if(str3[0] == '<'){
					busmonidata[top_cn].rdata = 0;
				}
				else{
					busmonidata[top_cn].rdata = atoi(str3);
				}
				
				if(str4[0] == 'M'){
					memcpy(busmonidata[top_cn].rdatacom, "M/s", 3);
					busmonidata[top_cn].rdatacom[3] = '\0';
				}else{
					if(str4[0] == 'K'){
						memcpy(busmonidata[top_cn].rdatacom, "K/s", 3);
						busmonidata[top_cn].rdatacom[3] = '\0';
					}
				}
				
				if(str5[0] == '<'){
					busmonidata[top_cn].wdata = 0;
				}
				else{
					busmonidata[top_cn].wdata = atoi(str5);
				}
	
				if(str6[0] == 'M'){
					memcpy(busmonidata[top_cn].wdatacom, "M/s", 3);
					busmonidata[top_cn].wdatacom[3] = '\0';
				}else{
					if(str6[0] == 'K'){
						memcpy(busmonidata[top_cn].wdatacom, "K/s", 3);
						busmonidata[top_cn].wdatacom[3] = '\0';
					}
				}
				
				printf("%s %d %s  %d %s\n",busmonidata[top_cn].name, busmonidata[top_cn].rdata, busmonidata[top_cn].rdatacom, busmonidata[top_cn].wdata, busmonidata[top_cn].wdatacom);
			}
	
			top_cn++;
		}
		
		if (!fgets(buff, sizeof(buff), fdddr))
		{
			if(top_cn == 0){
				printf("fdddr fgets fail\n");	
				fclose(fd);
				fclose(fdddr);
				return -1;
			}
		}
		if (!fgets(buff, sizeof(buff), fdddr))
		{
			printf("Failed fgets /proc/busmoniddr.txt!\n");
		}
		sscanf(buff,"%s%s%s%s%s%s%s%s%s%s", str1, str2, str3, str4, str5, str6, str7, str8, str9, str10);
		//printf("%s %s %s %s %s %s %s %s %s\n", str2, str3, str4, str5, str6, str7, str8, str9, str10);
		
		len = strlen(str2);
		memcpy(busmonidata[top_cn].name, str2, len);
		busmonidata[top_cn].name[len] = '\0';
		
		busmonidata[top_cn].rdata = atoi(str5);
		memcpy(busmonidata[top_cn].rdatacom, "M/s", 3);
		busmonidata[top_cn].rdatacom[3] = '\0';
	
		busmonidata[top_cn].wdata = atoi(str9);
		memcpy(busmonidata[top_cn].wdatacom, "M/s", 3);
		busmonidata[top_cn].wdatacom[3] = '\0';
	
		printf("%s %d %s  %d %s\n\n",busmonidata[top_cn].name, busmonidata[top_cn].rdata, busmonidata[top_cn].rdatacom, busmonidata[top_cn].wdata, busmonidata[top_cn].wdatacom);
		
		
		fclose(fd);
		fclose(fdddr);
		
		sleep(2);
	}
    return 0;
}

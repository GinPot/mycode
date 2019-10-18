#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "busmoni.h"



int GetBusMoni(struct BusMoniMems *busmonidata) 
{
	FILE* busf;
	int bus_cn=0, i=0, len;
	char buffbus[100],strbus1[10],strbus2[35],strbus3[10],strbus4[10],strbus5[10],strbus6[10],strbus7[10],strbus8[10],strbus9[10],strbus10[10];
	
    if((busf = popen("/usr/bin/busmoni_app -p UC11 UC22", "r")) == NULL) {
      perror( "popen error" );
      return -1;
    }
	
	
	while(1)
	{
		if (!fgets(buffbus, sizeof(buffbus), busf)){
			if(bus_cn == 0){
				printf("fgets fail\n");	
				pclose(busf);
				return -1;
			}
			else{
				break;
			}
		}
		if(bus_cn > 0){

			//printf("%s\n", buffbus);

			if(bus_cn == 1){
				sscanf(buffbus,"%s %s %s %s %s %s %s %s %s %s", strbus1, strbus2, strbus3, strbus4, strbus5, strbus6, strbus7, strbus8, strbus9, strbus10);
				//printf("%s %s %s %s %s %s %s %s %s\n", strbus2, strbus3, strbus4, strbus5, strbus6, strbus7, strbus8, strbus9, strbus10);
				
				memcpy(busmonidata[9].name, strbus2, strlen(strbus2));
				busmonidata[9].rdata = atoi(strbus5);
				memcpy(busmonidata[9].rdatacom, "M/s", 3);
				busmonidata[9].wdata = atoi(strbus9);
				memcpy(busmonidata[9].wdatacom, "M/s", 3);

				//printf("%s %d %s  %d %s\n",busmonidata[9].name, busmonidata[9].rdata, busmonidata[9].rdatacom, busmonidata[9].wdata, busmonidata[9].wdatacom);
			}
			
			if((bus_cn == 5) || (bus_cn == 6) || (bus_cn == 13) || (bus_cn == 19) || (bus_cn == 32) || (bus_cn == 22) || (bus_cn == 23) || (bus_cn == 27) || (bus_cn == 29))
			{
				sscanf(buffbus,"%s%s%s%s%s%s", strbus1, strbus2, strbus3, strbus4, strbus5, strbus6);
				//printf("%s %s %s %s %s\n", strbus2, strbus3, strbus4, strbus5, strbus6);
				
				len = strlen(strbus2);
				if(bus_cn == 29){
					memcpy(busmonidata[i].name, strbus2, 8);
					busmonidata[i].name[8] = '\0';
				}
				else{
					memcpy(busmonidata[i].name, strbus2, len);
					busmonidata[i].name[len] = '\0';
				}
				
				if(strbus3[0] == '<'){
					busmonidata[i].rdata = 0;
				}
				else{
					busmonidata[i].rdata = atoi(strbus3);
				}
				
				if(strbus4[0] == 'M'){
					memcpy(busmonidata[i].rdatacom, "M/s", 3);
					busmonidata[i].rdatacom[3] = '\0';
				}else{
					if(strbus4[0] == 'K'){
						memcpy(busmonidata[i].rdatacom, "K/s", 3);
						busmonidata[i].rdatacom[3] = '\0';
					}
				}
				
				if(strbus5[0] == '<'){
					busmonidata[i].wdata = 0;
				}
				else{
					busmonidata[i].wdata = atoi(strbus5);
				}
				
				if(strbus6[0] == 'M'){
					memcpy(busmonidata[i].wdatacom, "M/s", 3);
					busmonidata[i].wdatacom[3] = '\0';
				}else{
					if(strbus6[0] == 'K'){
						memcpy(busmonidata[i].wdatacom, "K/s", 3);
						busmonidata[i].wdatacom[3] = '\0';
					}
				}
				//printf("%s %d %s  %d %s\n",busmonidata[i].name, busmonidata[i].rdata, busmonidata[i].rdatacom, busmonidata[i].wdata, busmonidata[i].wdatacom);
				i++;
			}
			
		}
		bus_cn++;
	}

	pclose(busf);
	return 0;
}

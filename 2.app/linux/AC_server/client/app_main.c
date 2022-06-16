#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include <pthread.h>
#include "client.h"


int KeyProcessing(struct RecvKey *recvkeytmp)			//按键处理函数
{
	if(strcmp(recvkeytmp->name, "ReAirKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "ACKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "OffKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "AutoKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "FrontKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "HumKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "FanKeyUp") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "FanKeyDown") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "RearKey") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "TemKeyUp") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
	if(strcmp(recvkeytmp->name, "TemKeyDown") == 0){
		printf("%s=%d\n", recvkeytmp->name, recvkeytmp->KeyValue); 
		
		return 0;
	}
}

void printMark(struct MarkMem *MarkMemValuetmp)
{
	printf("MarkMemValue.ReAirLamp=%d\n",MarkMemValuetmp->ReAirLamp);
	printf("MarkMemValue.ACLamp=%d\n",MarkMemValuetmp->ACLamp);
	printf("MarkMemValue.FrontLamp=%d\n",MarkMemValuetmp->FrontLamp);
	printf("MarkMemValue.RearLamp=%d\n",MarkMemValuetmp->RearLamp);
	printf("MarkMemValue.ACMark=%d\n",MarkMemValuetmp->ACMark);
	printf("MarkMemValue.AutoMark=%d\n",MarkMemValuetmp->AutoMark);
	printf("MarkMemValue.HumMark=%d\n",MarkMemValuetmp->HumMark);
	printf("MarkMemValue.DfrMark=%d\n",MarkMemValuetmp->DfrMark);
	printf("MarkMemValue.VentMark=%d\n",MarkMemValuetmp->VentMark);
	printf("MarkMemValue.FootMark=%d\n",MarkMemValuetmp->FootMark);
	printf("MarkMemValue.FanGradeMark=%d\n",MarkMemValuetmp->FanGradeMark);
	printf("MarkMemValue.FanMark=%d\n",MarkMemValuetmp->FanMark);
	printf("MarkMemValue.BackLight=%d\n",MarkMemValuetmp->BackLight);
	printf("MarkMemValue.IGOFFSignal=%d\n",MarkMemValuetmp->IGOFFSignal);
	printf("MarkMemValue.TemNum=%.1f\n",MarkMemValuetmp->TemNum);
}

int main(int argc, char **argv)
{
	char cmd[15];
	float value;
	struct MarkMem markstatustest;
	
	if(ClientInit() != 0){
		printf("Client Init Fail!\n"); 
	}
	RegisterKeyFun(KeyProcessing);		//注册按键处理函数
	

	

	while(1)
	{
		scanf("%s %f",cmd,&value);
		//printf("%s %d\n",cmd,value); 
		
		
		if(strcmp(cmd, "ReAirLamp") == 0){
			printf("%s=%d\n", cmd, value); 
			SetReAirLamp(value);
		}
		if(strcmp(cmd, "ACLamp") == 0){
			printf("%s=%d\n", cmd, value); 
			SetACLamp(value);
		}
		if(strcmp(cmd, "FrontLamp") == 0){
			printf("%s=%d\n", cmd, value); 
			SetFrontLamp(value);
		}
		if(strcmp(cmd, "RearLamp") == 0){
			printf("%s=%d\n", cmd, value); 
			SetRearLamp(value);
		}
		
		
		if(strcmp(cmd, "ACMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetACMark(value);
		}
		if(strcmp(cmd, "AutoMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetAutoMark(value);
		}
		if(strcmp(cmd, "HumMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetHumMark(value);
		}
		if(strcmp(cmd, "DfrMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetDfrMark(value);
		}
		if(strcmp(cmd, "VentMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetVentMark(value);
		}
		if(strcmp(cmd, "FootMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetFootMark(value);
		}
		
		
		if(strcmp(cmd, "FanGradeMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetFanGradeMark(value);
		}
		if(strcmp(cmd, "FanMark") == 0){
			printf("%s=%d\n", cmd, value); 
			SetFanMark(value);
		}
		
		
		if(strcmp(cmd, "BackLight") == 0){
			printf("%s=%d\n", cmd, value); 
			SetBackLight(value);
		}
		if(strcmp(cmd, "IGOFFSignal") == 0){
			printf("%s=%d\n", cmd, value); 
			SetIGOFFSignal(value);
		}
		if(strcmp(cmd, "TemNum") == 0){
			printf("%s=%f\n", cmd, value); 
			SetTemNum(value);
		}	
		
		if(strcmp(cmd, "GetMarkStatus") == 0){		//获取显示状态
			printf("%s=%f\n", cmd, value); 
			GetMarkStatus(&markstatustest);
			printMark(&markstatustest);
		}		
	}
	
	return 0;
}
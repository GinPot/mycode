#ifndef _LIN_AURT_H_
#define _LIN_AURT_H_

struct RecvKey{
	char name[15];
	float KeyValue;		//1表示按下,2表示松开
};

struct RecvMem{				//值为1代表按下，需要自己清零
	int ReAirKey;			//下面7个按键
	int ACKey;
	int OffKey;
	int AutoKey;
	int FrontKey;
	int RearKey;
	int HumKey;
	
	int FanKeyUp;			//左档
	int FanKeyDown;
	
	int TemKeyUp;			//右档
	int TemKeyDown;
	
	//int SensorTemp;
	//int SensorVolt;
};

struct MarkMem{				//值为1代表显示(灯亮)，0位不显示(灯灭)
	int ReAirLamp;			//循环空气灯
	int ACLamp;				//AC灯
	int FrontLamp;			//前除雾器指示灯
	int RearLamp;			//后除雾器指示灯

	int ACMark;				//A/C 图标
	int AutoMark;			//Auto 图标
	int HumMark;			//小人 图标
	int DfrMark;			//除雾器图标
	int VentMark;			//向左箭头图标
	int FootMark;			//向下箭头图标
	
	int FanGradeMark;		//风扇档位(0~8)
	int FanMark;			//小风扇图标
	
	int BackLight;			//背光
	int IGOFFSignal;		//开关机
	//int TemPoint;
	float TemNum;			//设置温度值(00.0~99.9)
};


//extern int LinInit(void);
//extern int GetKeyValue(struct RecvMem *KeyValuetmp);
//extern int RecvAppData(struct MarkMem *MarkMemtmp);

#endif /* _LIN_AURT_H_ */
#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_

struct RecvKey{
	char name[15];
	int KeyValue;			//1表示按下,2表示松开
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

extern int ClientInit(void);												//client初始化，内部创建个client接收线程
extern void RegisterKeyFun(int (*KeyProcesstemp)(struct RecvKey *));		//注册按键处理函数
extern int GetMarkStatus(struct MarkMem *markstatus);						//获取目前的显示状态


//值为1代表显示(灯亮),0位不显示(灯灭),特殊情况下面会说明
extern int SetReAirLamp(int acdata);			//循环空气灯
extern int SetACLamp(int acdata);				//AC灯
extern int SetFrontLamp(int acdata);			//前除雾器指示灯
extern int SetRearLamp(int acdata);				//后除雾器指示灯

extern int SetACMark(int acdata);				//A/C 图标
extern int SetAutoMark(int acdata);				//Auto 图标
extern int SetHumMark(int acdata);				//小人 图标
extern int SetDfrMark(int acdata);				//除雾器图标
extern int SetVentMark(int acdata);				//向左箭头图标
extern int SetFootMark(int acdata);				//向下箭头图标

extern int SetFanGradeMark(int acdata);			//风扇档位(0~8)
extern int SetFanMark(int acdata);				//小风扇图标

extern int SetBackLight(int acdata);			//背光
extern int SetIGOFFSignal(int acdata);			//开关机
extern int SetTemNum(float acdata);				//设置温度值(00.0~99.9)


#endif /* _SOCKET_CLIENT_H_ */
#ifndef __AURT_H_
#define __AURT_H_



#define FALSE  -1  
#define TRUE   0  


extern int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
extern int UART0_Send(int fd, char *send_buf,int data_len);
extern int UART0_Open(int fd,char* port);
extern int UART0_Recv(int fd, char *rcv_buf,int data_len);
extern void UART0_Close(int fd);

#endif /* __AURT_H_ */
#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_


extern void *SocketServerPthread(void *arg);
extern void SocketServerSend(char *keyname, int keyvalue);
extern int ClientProcess(struct RecvKey *recvkeytmp);


#endif /* _SOCKET_SERVER_H_ */
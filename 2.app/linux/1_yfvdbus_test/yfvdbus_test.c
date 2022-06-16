#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "yfv_dbus.h"

const char * rules_list[] = {"interface='ipcl.out.to.droid',member='bb_knob_rx_event'"};
YFV_DBusConnection* conn = NULL;
	
static DBusHandlerResult filter_func (YFV_DBusConnection * conn,YFV_DBusMessage * msg,void * user_data)
{
	const unsigned char * buff = NULL;
	int len = 0,i;
	int type;
	const char* interface = NULL;
	const char* member = NULL;
	const char* dbus_err_msg = NULL;
	YFV_DBUS_ERR ret = YFV_DBUS_OK;
	
	//printf("filter_func\n");
	
	if(msg != NULL)
	{
		if(YFV_DBUS_OK != (yfv_dbus_get_message_info(msg, &type, &interface, &member)))
		{
			printf("get message info error\n");
			// error handle
		}
		else
		{
			//printf("yfv_dbus_get_message_info :%d\n",type);
			if(YFV_DBUS_MESSAGE_TYPE_SIGNAL == type)
			{
				//printf("interface   %s\n",interface);
				if(!strcmp(interface,"ipcl.out.to.droid"))
				{
					//printf("member   %s\n",member);
					if(!strcmp(member,"bb_knob_rx_event"))
					{
						if(YFV_DBUS_OK != (yfv_dbus_get_message(msg, &buff, &len)))
						{ 
							printf("get signal message payload  error\n");
							// error handle
						}
						else
						{ 
							printf("receive signal message ok :");
							for(i=0;i<len;i++)
								printf(" 0x%x", buff[i]);
							printf("\n");
							//message handle
						}
					}
				}
			}
			else if(YFV_DBUS_MESSAGE_TYPE_METHOD_CALL == type)
			{ 
				return YFV_DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
			} 
			else if(YFV_DBUS_MESSAGE_TYPE_ERROR == type)
			{ 
				if(YFV_DBUS_OK !=(ret = yfv_dbus_get_err_message_info(msg, &dbus_err_msg)))
				{
					printf("test get error %d \n",ret);
				}
				else
				{
					printf("test error: %s\n",dbus_err_msg);
					printf("interface = %s,member = %s\n",interface,member);
				}
			}
			else
			{
				printf("unknown message type\n");
			}
		}
	}
	else
	{ 
		printf("test this message is NULL\n");
	}
	return YFV_DBUS_HANDLER_RESULT_HANDLED;
}


void signalSendTest(void* arg)
{  
	int len = 100;
	unsigned char dbus_msg[4096] = {0,0,0,0,0,0,1,2,3,4,5};

	YFV_DBUS_ERR ret = YFV_DBUS_OK;
	while(1){
		if(YFV_DBUS_OK !=(ret = yfv_dbus_signal_send(conn, "ipcl.out.to.droid", "af_ar_rx_event", dbus_msg, 10)))
		{					
			;//printf("signal send error %d \n",ret);
		} 
		else
		{ 
			;//printf("signal send ok\n");
		}
		usleep(50000);
	}

}

int main()
{
	static pthread_t thread;
	
	YFV_DBUS_ERR ret = YFV_DBUS_OK;
	if(YFV_DBUS_OK !=(ret = yfv_dbus_connect(&conn, "AI.QX.send")))
	{
		printf("connection error %d\n",ret);
		return 0;
	}

	//receive
	if(YFV_DBUS_OK !=(ret = yfv_dbus_add_match(conn, rules_list, sizeof(rules_list)/8)))
	{
		printf("add match error %d\n",ret);
		return 0;
	}
	if(YFV_DBUS_OK !=(ret = yfv_dbus_add_filter(conn, (YFV_DBusHandleMessageFunction)filter_func)))
	{
		printf("add filter error\n");
	}

	//send
	ret = pthread_create(&thread, NULL, signalSendTest, NULL);
		if (ret)
			perror("pthread_create TopPro failed");
		

	yfv_dbus_wait_message(conn);
} 

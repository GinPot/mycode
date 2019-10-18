#include <hyp_shm.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/dispatch.h>
#include <dlfcn.h>

#include "i2c2.h"

#define PULSE_CODE_NOTIFY   0
#define BUFF_SIZE           0x1000
#define I2C_ADDR 			0x49
#define ATTACH_POINT 		"myname"

void (*CameraInit)(void);

struct KeyVal {
	int statu;
	int vala;
};
enum DAY
{
    NONE=0, 
	KEY_LEFT_CAMERA, 
	KEY_LEFT_UP = 103,
	KEY_LEFT_DOWN = 108,
	KEY_LEFT_MUTE = 113,
	KEY_LEFT_RIGHT = 105,
	KEY_LEFT_LEFT = 106,
	KEY_RIGHT_CALL = 169,
	KEY_RIGHT_CALLING = 167,
	KEY_RIGHT_CALLEND = 128
};
struct message_s
{
    uint16_t        val;
};
struct KeyVal keyvala_left, keyvala_right;
struct KeyVal keyvala_left_old, keyvala_right_old;
struct hyp_shm *hsp_ads1015;
char *shmdata;
char *keybuff;
int coid;
static int numerr;
static struct message_s msg_mute_long;

static int ads1015_i2c_write ( uint8_t reg, uint8_t valmsb, uint8_t vallab)
{
    int status = EOK;

    status = i2c_write ( I2C_ADDR, reg, valmsb, vallab, 0 );
    if ( status != EOK )
        printf( "ads1015 i2c write failed\n" );

    return status;
}

static int ads1015_i2c_read ( uint8_t reg, unsigned short* val)
{
    int status = EOK;

    status = i2c_read ( I2C_ADDR, reg, val, 0 );
    if ( status != EOK ) 
        printf( "ads1015 i2c read failed\n" );

    return status;
}

/*
val:	0、1对应mode键短按
		2、3对应mode键长按
		4、5、6对应mute键长按
*/

static int val_filter(unsigned short left_val, unsigned short right_val)
{
	static struct message_s msg,msg_long;
	static int camerkey = 0, mutelogkey = 0;
	static struct timeval time1;
	static int modeflag = 0, mutelogflag = 0;
	struct  timezone   tz;
	static float nowtime, oldtime;
	
	
	//left key
	if(left_val >= 1341 && left_val <= 1441)
		keyvala_left.vala = NONE;
	if(left_val >= 168 && left_val <= 268)
		keyvala_left.vala = KEY_LEFT_MUTE;
	if(left_val >= 262 && left_val <= 362)
		keyvala_left.vala = KEY_LEFT_UP;
	if(left_val >= 380 && left_val <= 480)
		keyvala_left.vala = KEY_LEFT_DOWN;
	if(left_val >= 533 && left_val <= 633)
		keyvala_left.vala = KEY_LEFT_CAMERA;
	if(left_val >= 731 && left_val <= 831)
		keyvala_left.vala = KEY_LEFT_RIGHT;
	if(left_val >= 1003 && left_val <= 1103)
		keyvala_left.vala = KEY_LEFT_LEFT;

	if(keyvala_left.vala != keyvala_left_old.vala)
	{
		switch(keyvala_left.vala)
		{
			case NONE:
				if((camerkey != 1) && (mutelogkey != 1))
				{
					keybuff[3] = 0;
					hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
				}else{
					if(camerkey == 1){
						camerkey = 0;
						modeflag = 0;
						
						if(nowtime < oldtime + 1){
							if(msg.val == 1)
								msg.val = 0;
							else
								msg.val = 1;
							if (MsgSend(coid, &msg, sizeof(msg), NULL, 0) < 0) {
								printf("ads1015 MsgSend error\n");
								return 1;
							}
							//printf("ads1015 Short mode keys: %d\n",msg.val);
						}
					}
					
					if(mutelogkey == 1){
						mutelogkey = 0;
						mutelogflag = 0;
						
						if(nowtime < oldtime + 3){
							keybuff[3] = 1;
							hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
							//printf("ads1015 Short mute keysvalue: %d  value:%d\n",keybuff[2],keybuff[3]);
							usleep(10000);
							keybuff[3] = 0;
							hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
							//printf("ads1015 Short mute keysvalue: %d  value:%d\n",keybuff[2],keybuff[3]);
						}
					}
				}
				
				//printf( "key releasa\n" );
			break;
			
			case KEY_LEFT_CAMERA:
				camerkey = 1;
				modeflag = 1;
				gettimeofday(&time1, &tz);
				oldtime = time1.tv_sec + (float)time1.tv_usec/1000000;
				//printf( "key camera :%f\n",oldtime);
			break;
			case KEY_LEFT_MUTE:
				mutelogflag = 1;
				mutelogkey = 1;
				gettimeofday(&time1, &tz);
				oldtime = time1.tv_sec + (float)time1.tv_usec/1000000;
				keybuff[2] = keyvala_left.vala;
			break;
			case KEY_LEFT_UP:
			case KEY_LEFT_DOWN:
			case KEY_LEFT_RIGHT:
			case KEY_LEFT_LEFT:
				keybuff[2] = keyvala_left.vala;
				keybuff[3] = 1;
				hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
				//printf( "key up or down keyvala_left.vala=%d\n",keyvala_left.vala );
			break;
		}
	}
	keyvala_left_old = keyvala_left;
	
	
	if(mutelogflag == 1){
		gettimeofday(&time1, &tz);
		nowtime = time1.tv_sec + (float)time1.tv_usec/1000000;
		if(nowtime >= oldtime + 3){
			mutelogflag = 0;
			
			if (MsgSend(coid, &msg_mute_long, sizeof(msg_mute_long), NULL, 0) < 0) {
				printf("ads1015 Long MsgSend error\n");
				return 1;
			}
			//printf("ads1015 Long mute keys :%d\n",msg_mute_long.val);

			msg_mute_long.val++;
			if(msg_mute_long.val > 6){
				msg_mute_long.val = 4;
			}	
		}
	}
	
	if(modeflag == 1){
		gettimeofday(&time1, &tz);
		nowtime = time1.tv_sec + (float)time1.tv_usec/1000000;
		if(nowtime >= oldtime + 1){
			modeflag = 0;
			
			if(msg_long.val == 2)
				msg_long.val = 3;
			else
				msg_long.val = 2;
			if (MsgSend(coid, &msg_long, sizeof(msg_long), NULL, 0) < 0) {
				printf("ads1015 Long MsgSend error\n");
				return 1;
			}
			//printf("ads1015 Long mode keys :%d\n",msg_long.val);
		}
	}
	
	//right key
	if(right_val >= 1341 && right_val <= 1441)
		keyvala_right.vala = NONE;
	if(right_val >= 166 && right_val <= 266)
		keyvala_right.vala = KEY_RIGHT_CALL;
	if(right_val >= 379 && right_val <= 479)
		keyvala_right.vala = KEY_RIGHT_CALLING;
	if(right_val >= 731 && right_val <= 831)
		keyvala_right.vala = KEY_RIGHT_CALLEND;	
	
	if(keyvala_right.vala != keyvala_right_old.vala)
	{
		switch(keyvala_right.vala)
		{
			case NONE:
				keybuff[3] = 0;
				hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
				//printf( "key releasa\n" );
			break;
			
			case KEY_RIGHT_CALL:
			case KEY_RIGHT_CALLING:
			case KEY_RIGHT_CALLEND:
				keybuff[2] = keyvala_right.vala;
				keybuff[3] = 1;
				hyp_shm_poke(hsp_ads1015, HYP_SHM_POKE_ALL_BUT_ME);
				//printf( "key up or down keyvala_right.vala=%d\n",keyvala_right.vala );
			break;
		}
	}
	keyvala_right_old = keyvala_right;
	
	return 0;
}

int main(int argc, char *argv[]) 
{
	int status = EOK;
	unsigned short left_val,right_val;
    int channel;
	
	sleep(10);
	
	msg_mute_long.val = 4;

	void *handle = dlopen( "/apps/cluster/lib-camera_lib.so", RTLD_LOCAL );	
	if( NULL == (CameraInit = dlsym( handle, "CameraRunTest" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOn" "' in '%s'\n", "/apps/cluster/lib-camera_lib.so" );
		dlclose( handle ); 
		return -1; 
	}
	CameraInit();
	
    while ((coid = name_open(ATTACH_POINT, 0)) == -1) 
		sleep(1);

	//adc init
	status = open_i2c_fd ( "/dev/i2c6" );
    if ( status != EOK ) {
        printf( "ads1015 open failed\n" );
		return 0;
    }//0x52

	
    channel = ChannelCreate_r(0);
    if (channel < 0) {
        printf("channel failure: %d\n", -channel);
        goto done;
    }	
	printf("channel = %d\n", channel);
    //
    // Allocate the memory of the hyperivsor shared memory control structure
    //
    hsp_ads1015 = hyp_shm_create(0);
    if (hsp_ads1015 == NULL) {
        printf("no memory for shared memor control structure\n");
        goto done;
    }
	
    //
    // Attach to the shared memory region. If the region status (see below)
    // changes, a pulse will be delivered to the channel 'channel' with
    // a code of 'PULSE_CODE_NOTIFY'. A pointer can be passed in the
    // pulse value field as well, though we're not using it here.
    //
    int err = hyp_shm_attach(hsp_ads1015, "testing", HYP_SHM_MAX_CLIENTS,
            channel, -1, PULSE_CODE_NOTIFY, NULL);
    if (err != EOK) {
        printf("shmem create failure: %d\n", err);
        goto fail;
    }	

    //
    // The first client that attempts to create the shared memory name
    // gets to choose the actual size of the shared memory region. Since you
    // can't tell if you were the first or not, you need to check the 'size'
    // field after the creation - it will have the actual number of pages
    // of the region. This may be larger, smaller, or the same size as what
    // you requested.
    //
    unsigned const size = hyp_shm_size(hsp_ads1015);
    if (size != HYP_SHM_MAX_CLIENTS) {
        printf("unexpected shared memory size (%d != %d)\n", size, HYP_SHM_MAX_CLIENTS);
        goto fail;
    }
	
    //
    // Each client that connects to shared memory region gets a unique
    // connection index number (0 through 15). This index is to form various
    // bitsets. The hyp_shm_idx() API tells you what your connection index
    // number is.
    //
    unsigned myidx = hyp_shm_idx(hsp_ads1015);
    printf("shared memory index %u\n", myidx);
	
    //
    // The hyp_shm_data() API returns a pointer to the start of the
    // shared memory region data
    //
    shmdata = hyp_shm_data(hsp_ads1015);
	
	keybuff = &shmdata[myidx*BUFF_SIZE];
	keybuff[0] = 0xA1;		//The first two bytes of validation
	keybuff[1] = 0x1A;
	//keybuff[2]			//Key value
	//keybuff[3]			//Key state
	keybuff[4] = '\0';		//End symbol
				
	while(1)
	{
		status = ads1015_i2c_write(0x01,0x42,0x83);		//A0 channal
		if ( status != EOK )
		{
			printf("write A0 fail \n");
			numerr++;
			goto again;	
		}
		usleep(1000);
		status = ads1015_i2c_read(0x0,&left_val);
		if ( status != EOK )
		{
			printf("read A0 fail \n");
			numerr++;
			goto again;	
		}

		status = ads1015_i2c_write(0x01,0x52,0x83);		//A1 channal
		if ( status != EOK )
		{
			printf("write A1 fail \n");
			numerr++;
			goto again;	
		}
		usleep(1000);		
		status = ads1015_i2c_read(0x0,&right_val);
		if ( status != EOK )
		{
			printf("read A1 fail \n");
			numerr++;
			goto again;	
		}
		
		right_val = right_val >> 4;
		left_val = left_val >> 4;
		val_filter(left_val, right_val);
		
		//printf("left_val =  %d      right_val = %d\n",left_val,right_val);
again:	//Read and write errors, abandon this data, and allow continued operation
		
		usleep(50000);
		if(numerr > 6)
			goto fail;	
	}	
	
fail:
	hyp_shm_detach(hsp_ads1015);
done:
	close_i2c_fd();	
    return 0;
}

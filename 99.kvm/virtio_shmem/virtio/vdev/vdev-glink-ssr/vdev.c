/**===========================================================================
  @copyright
  	Copyright (c) 2018 QUALCOMM Technologies Incorporated.
  	All rights reserved. Licensed Material - Restricted Use.
  	Qualcomm Confidential and Proprietary.
===========================================================================*/
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <qvm/vdev.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <pthread.h>
#include <hw/inout.h>
#include <stdbool.h>
#include <sys/resmgr.h>
#include <sys/mman.h>
#include <sys/dispatch.h>
#include <dll_utils_i.h>

#include <glink_ssr_xport.h>
#include <ssr_events.h>
#include <ssr_api.h>
#include <amss/fdt_utils.h>
#include "libfdt.h"
#include "guest_ssr_msg.h"

#ifdef WLAN_SSR
#include <wcnss_xport.h>
#endif

#define OFF_WLAN_CMD     0x208 

/*offsets when subsystem is in host*/
#define OFF_LPASS_PVM_EVENT 0x0
#define OFF_LPASS_PVM_ACK   0x4
#define OFF_MPSS_PVM_EVENT  0x100
#define OFF_MPSS_PVM_ACK    0x104

/*offsets when subsystem is in guest*/
#define OFF_LPASS_GVM_EVENT 0x8
#define OFF_MPSS_GVM_EVENT  0x108

#define LPASS_INTR 75
#define MPSS_INTR  76
#define GVM_ACK_PULSE_CODE _PULSE_CODE_MINAVAIL+1

typedef struct ssr_ss_intr_s {
	struct qvm_intr s_intr;
	int intr_num;
} ssr_ss_intr_t;

/*Device specific state information*/
typedef struct _glink_ssr_virt_dev_s {
	uint32_t          glink_ssr_virt_base;
	int               glink_coid;
	int               wcnss_coid;
	int               subsystem_count;
	ssr_ss_intr_t     *ss_intr;
	int 			  ssr_event_coid;
	pthread_cond_t	  cond;
	pthread_mutex_t	  mutex;
	uint32_t		  event;
} glink_ssr_virt_dev_t;


/*IO mem region index*/
enum {
	REGION_GLINK_SSR_VIRT_IDX,
};

/*vdev-glink_ssr-virt command line options*/
enum {
	OPTION_GLINK_SSR_VIRT_BASE,
	OPTION_GLINK_SSR_VIRT_WLAN,
	OPTION_GLINK_SSR_VIRT_VERBOSE,
	OPTION_GLINK_INTR,
};

enum subsys_notif_type {
	SUBSYS_BEFORE_SHUTDOWN,
	SUBSYS_AFTER_SHUTDOWN,
	SUBSYS_BEFORE_POWERUP,
	SUBSYS_AFTER_POWERUP,
	SUBSYS_RAMDUMP_NOTIFICATION,
	SUBSYS_POWERUP_FAILURE,
	SUBSYS_PROXY_VOTE,
	SUBSYS_PROXY_UNVOTE,
	SUBSYS_SOC_RESET,
	SUBSYS_NOTIF_TYPE_COUNT
};

/*IO mem regions for trap and emulate*/
static struct qvm_region trap_regions [] = {
	[ REGION_GLINK_SSR_VIRT_IDX ] = {
		.flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
		.guest.location = 0,
		.guest.length   = __PAGESIZE,
		.guest.type     = QST_MEMORY,
		.guest.flags    = QSF_NONE,
	},
};

/*glink_ssr_virt log macro*/
#define QVM_LOG(severity,fmt,...)\
	do { \
		if ((severity) <= g_verbose) { \
			(void) qvm_report("%s: " fmt "\n", __func__, ##__VA_ARGS__);\
		} \
	} while (0)

#define OFFSET(_a) (_a & (0x1000-1))

/* Wrapper definitions for severity levels*/
enum _LOGGER_SEVERITY{
	GLINK_SSRVIRTLOG_ERROR,          //unexpected recoverable error
	GLINK_SSRVIRTLOG_WARNING,        //expected error
	GLINK_SSRVIRTLOG_INFO,           //information
	GLINK_SSRVIRTLOG_DEBUG,          //debug messages (e.g., normal detail).
};

enum vdev_notif_type {
	VDEV_NOTIFICATION_NONE,
	VDEV_GLINK_NOTIFICATION,
	VDEV_WCNSS_NOTIFICATION,
	VDEV_EVENT_NOTIFICATION
};

/*log verbosity for qvm_report*/
static uint8_t g_verbose = GLINK_SSRVIRTLOG_DEBUG;

#ifdef WLAN_SSR
/* Command line parameter for PCIe */
static uint8_t g_wlan_ssr = 0;
#endif
static int vdev_pulse_handler(qvm_vdev_t * const vdev, int8_t code)
{
    glink_ssr_virt_dev_t *mtx_dev = vdev->v_device;
	if (code == GVM_ACK_PULSE_CODE) {
		pthread_mutex_lock(&mtx_dev->mutex);
		pthread_cond_signal(&mtx_dev->cond);
		pthread_mutex_unlock(&mtx_dev->mutex);
		QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"Guest Acknolwdgement received");
	}
	return EOK;

} 

static int host_guest_mapping(int event)
{
	int guest_event = 0;
	switch (event) {
		case SSR_EVENT_RESTART_COMPLETE:
			guest_event = SUBSYS_AFTER_POWERUP;
			break;
		case SSR_EVENT_POWERDOWN:
			guest_event = SUBSYS_BEFORE_SHUTDOWN;
			break;
		case SSR_EVENT_POWER_UP:
			guest_event = SUBSYS_BEFORE_POWERUP;
			break;
		case SSR_EVENT_RESTART_FAILED:
			guest_event = SUBSYS_POWERUP_FAILURE;
			break;
		case SSR_SOC_RESTART:
			guest_event = SUBSYS_SOC_RESET;
			break;
	}
	return guest_event;
}

static int guest_host_mapping(int event)
{
	int host_event = 0;
	switch (event) {
		case SUBSYS_AFTER_POWERUP:
			host_event = SSR_EVENT_RESTART_COMPLETE;
			break;
		case SUBSYS_BEFORE_SHUTDOWN:
			host_event = SSR_EVENT_POWERDOWN;
			break;
		case SUBSYS_BEFORE_POWERUP:
			host_event = SSR_EVENT_POWER_UP;
			break;
		case SUBSYS_POWERUP_FAILURE:
			host_event = SSR_EVENT_RESTART_FAILED;
			break;
		case SUBSYS_SOC_RESET:
			host_event = SSR_SOC_RESTART;
			break;
	}
	return host_event;
}

static int send_pulse_msg (const void *msg, int notif_type, int coid)
{
	int ret = EOK;
	const struct glink_ssr_msg *glink_msg;
#ifdef WLAN_SSR
    const union wcnss_msg *wmsg;
#endif
	const struct ssr_event_msg *event_msg;
	const char *ssr_type;

	switch (notif_type) {
#ifdef WLAN_SSR
		case VDEV_WCNSS_NOTIFICATION:
			QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Sending notification to wlan_ssr");
			wmsg = (union wcnss_msg *)msg;
			ssr_type = "wlan_ssr";
			ret = MsgSend(coid, wmsg, sizeof(union wcnss_msg), NULL, 0);
			break;
#endif
		case VDEV_GLINK_NOTIFICATION:
			QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Sending notification to glink_ssr");
			ssr_type = "glink_ssr";
			glink_msg = (struct glink_ssr_msg *)msg;
			ret = MsgSend(coid, glink_msg, sizeof(struct glink_ssr_msg), NULL, 0);
			break;

		case VDEV_EVENT_NOTIFICATION:
			ssr_type = "event_ssr";
			event_msg = (struct ssr_event_msg *)msg;
			QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Sending notification to event_ssr");
			ret = MsgSend(coid, event_msg, sizeof(struct ssr_event_msg), NULL, 0);
			break;

		default:
			QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "Invalid notification type"); 	
	}
	if (ret == -1)
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Failed to send pulse message for type: %s",ssr_type);
	return ret;
}

static int handle_guest_event(uint32_t wval, const glink_ssr_virt_dev_t *dev,
							   const struct glink_ssr_msg *glink_msg, 
							   struct ssr_event_msg *event_msg)
{
	int i, ret = EOK;
	const int from_before_shutdown[] = { SSR_EVENT_FAULT_NOTIFY, SSR_EVENT_RESTART_START, SSR_EVENT_POWERDOWN};
	int arr_size = sizeof(from_before_shutdown)/sizeof(from_before_shutdown[0]);
	
	if ( wval == SUBSYS_AFTER_SHUTDOWN ) {
		ret = send_pulse_msg(glink_msg, VDEV_GLINK_NOTIFICATION, dev->glink_coid);

	} else if (wval == SUBSYS_RAMDUMP_NOTIFICATION || wval == SUBSYS_PROXY_VOTE 
			   || wval == SUBSYS_PROXY_UNVOTE) {
		QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Ignoring SSR state of 0x%x", wval);

	} else if (wval == SUBSYS_BEFORE_SHUTDOWN) {
		for( i=0; i<arr_size; i++) {
			event_msg->data = from_before_shutdown[i];
			ret = send_pulse_msg(event_msg, VDEV_EVENT_NOTIFICATION, dev->ssr_event_coid);
		}
	} else {
		event_msg->data = guest_host_mapping(wval);
		ret = send_pulse_msg(event_msg, VDEV_EVENT_NOTIFICATION, dev->ssr_event_coid);
	}
	return ret;
}

#ifdef WLAN_SSR
static int handle_wlan_event(uint32_t wval, const glink_ssr_virt_dev_t *dev)
{
    union wcnss_msg wmsg;
    int coid, ret = EOK;

    coid = ((glink_ssr_virt_dev_t *)dev)->wcnss_coid;
    if (g_wlan_ssr && wval == SUBSYS_AFTER_SHUTDOWN)
    {
        if(coid == -1)
        {
           coid = name_open(WCNSS_ATTACH_POINT, 0);
           if ( coid == -1 )
           {
               QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"name_open(%s) failed, rc=%d",WCNSS_ATTACH_POINT, errno);
               return QVRS_BUS_ERROR;
           }
        }
        memset ( &wmsg , 0x0, sizeof ( union wcnss_msg ));
        wmsg.type = WCNSS_WLAN_SSR;
        send_pulse_msg(&wmsg, VDEV_WCNSS_NOTIFICATION, coid);
    }

    return ret;
}
#endif

static enum qvm_vdev_ref_status vdev_vread(qvm_vdev_t *const vdev,
										   unsigned cookie , const struct qvm_state_block *const vopnd,
										   const struct qvm_state_block *const oopnd,
										   struct guest_cpu *const gcp)
{
	uint32_t rval = ~0u;
	const glink_ssr_virt_dev_t *dev = ( glink_ssr_virt_dev_t* ) vdev->v_device;
	pthread_mutex_lock(&vdev->v_mtx);
	const uint64_t base =  vopnd->location & (~(__PAGESIZE - 1));
	const uint32_t off  =  vopnd->location & (__PAGESIZE-1);
	switch (off) {
		case OFF_LPASS_PVM_EVENT:
		case OFF_MPSS_PVM_EVENT:
			rval = dev->event;
			break;
	}

	QVM_LOG(GLINK_SSRVIRTLOG_INFO,"%s, read on loc=%lx , cookie = %d", __func__, vopnd->location, cookie ) ;
	QVM_LOG(GLINK_SSRVIRTLOG_INFO,"%s, base =%lx :: off =%08x :: rval =%d", __func__, base, off,rval) ;

	pthread_mutex_unlock(&vdev->v_mtx);
	qvm_cpu_write(gcp, QXF_NONE,  oopnd, 1, &rval, sizeof(rval));
	return QVRS_NORMAL;
}

/**
 * Perform a vdev write operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp             guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status vdev_vwrite(qvm_vdev_t *const vdev,
		unsigned cookie, const struct qvm_state_block *const vopnd,
		const struct qvm_state_block *const oopnd,
		struct guest_cpu *const gcp)
{
	uint32_t wval;
	int ret;
	bool event = 0;
	const glink_ssr_virt_dev_t *dev = ( glink_ssr_virt_dev_t* ) vdev->v_device;
	int rc = qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, &wval, sizeof(wval));
	if (rc != EOK) {
		return QVRS_ERRNO + rc;
	}

	QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"Guest writes 0x%x to 0x%lx", wval ,vopnd->location);

	struct glink_ssr_msg glink_msg;
	struct ssr_event_msg event_msg;

	const uint32_t offset = OFFSET(vopnd->location);
	struct sigevent gvm_ack_pulseevent;
    struct sched_param  params;

	memset ( &glink_msg , 0x0, sizeof ( struct glink_ssr_msg ));
	memset ( &event_msg, 0x0, sizeof ( struct ssr_event_msg));
	glink_msg.hdr.type = GLINK_MSG_HEADER;
	event_msg.hdr.type = SSR_EVENT_HEADER;

	switch ( offset ) {
#ifdef WLAN_SSR
		case OFF_WLAN_CMD:
            ret = handle_wlan_event(wval, dev);
			break;
#endif
		case OFF_LPASS_PVM_ACK:
		case OFF_MPSS_PVM_ACK:
			qvm_vdev_pulse (vdev,GVM_ACK_PULSE_CODE,params.sched_curpriority+1,&gvm_ack_pulseevent);
			MsgDeliverEvent(0, &gvm_ack_pulseevent);
			break;

		case OFF_LPASS_GVM_EVENT:
			event_msg.hdr.subtype = SS_ID_LPASS;
			glink_msg.hdr.subtype = 0x0;
			event = 1;
			if (wval == SUBSYS_AFTER_POWERUP) {
				QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Guest lpass bootup done");
			}
			break;
		case OFF_MPSS_GVM_EVENT:
			event_msg.hdr.subtype = SS_ID_MODEM;
			glink_msg.hdr.subtype = 0x1;
			event = 1;
			if (wval == SUBSYS_AFTER_POWERUP) {
				QVM_LOG(GLINK_SSRVIRTLOG_INFO,"Guest modem bootup done");
			}
			break;
		default:
			QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Invalid offset %d, abort guest", offset);
			return QVRS_BUS_ERROR;
	}
	if (event) {
		
		if (dev->ssr_event_coid == -1) {
			((glink_ssr_virt_dev_t*)dev)->ssr_event_coid = name_open(SSR_EVENT_ATTACH_POINT, 0);
			if (dev->ssr_event_coid == -1) {
				QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"name_open(%s) failed, rc=%d",SSR_EVENT_ATTACH_POINT, errno);
				return QVRS_BUS_ERROR;
			}
		}

		ret = handle_guest_event(wval, dev, &glink_msg, &event_msg);
		if (ret != EOK)
			return QVRS_ERRNO + ret;
	}

	return QVRS_NORMAL;
}

static void wait_for_guest_ack(pthread_mutex_t *mutex, pthread_cond_t *cond) 
{
	pthread_mutex_lock(mutex);
	pthread_cond_wait(cond, mutex);
	pthread_mutex_unlock(mutex);
}

static int ssr_callback_function (enum ssr_ss_id  ss_id, enum ssr_events event, void *data)
{
	int i;
	int idx = 0;
	int errno;
	ssr_ss_intr_t *ss_intr = NULL;
	glink_ssr_virt_dev_t *dev = (glink_ssr_virt_dev_t *)data;	
	const int from_powerdown[] = { SUBSYS_BEFORE_SHUTDOWN, SUBSYS_AFTER_SHUTDOWN, SUBSYS_RAMDUMP_NOTIFICATION};
	int arr_size = sizeof(from_powerdown)/sizeof(from_powerdown[0]);

	QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"Host ssr_callback_function vdev, host event: %d", event); 
	if ( ss_id == SS_ID_LPASS ) {
		idx = LPASS_INTR;
	} else if ( ss_id == SS_ID_MODEM) {
		idx = MPSS_INTR;
	} else { 
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "Invalid ss_id");
		return -1;
	}

	for ( i=0; i<dev->subsystem_count; i++) {
		if (dev->ss_intr[i].intr_num == idx) {
			ss_intr = &dev->ss_intr[i];
			break;
		}
	}

	if (ss_intr == NULL) {
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "Cannot find the right interrupt");
		return -1;
	}

	if (event == SSR_EVENT_POWERDOWN ) {
		for (i=0; i<arr_size; i++) {
			dev->event = from_powerdown[i];
			qvm_signal_intr(&ss_intr->s_intr, QIS_ON);
			wait_for_guest_ack(&dev->mutex, &dev->cond);
			qvm_signal_intr(&ss_intr->s_intr, QIS_OFF);
		}
	} else {
		dev->event = host_guest_mapping(event);
		qvm_signal_intr(&ss_intr->s_intr, QIS_ON);
		wait_for_guest_ack(&dev->mutex, &dev->cond);
		qvm_signal_intr(&ss_intr->s_intr, QIS_OFF);
	}

	QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"SSR host event %d acknowledged by guest",dev->event);
	return 0;
}

static int get_ss_name(const void *fdt, int offset, uint32_t *mask)
{
	int proplen;
	const char *propname;
	const void *propdata;
	int enabled = -1;
	int ss_id = -1;

	for ( offset = fdt_first_property_offset (fdt, offset); offset > 0;
		  offset = fdt_next_property_offset(fdt, offset ) )
	{
		propdata = fdt_getprop_by_offset(fdt, offset, &propname, &proplen);
		if ( propdata == NULL ) {
			QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "No entry found under /glink-ssr");
			return -1;
		}

		if ( !strncmp (propname , "enabled", sizeof("enabled"))) {
			enabled = fdt32_to_cpu(*((uint32_t*)propdata));

		} else if ( !strncmp (propname, "ss_name", sizeof("ss_name"))) {
			if ( !strncmp (propdata, "lpass", sizeof("lpass")))
				ss_id = SS_ID_LPASS;
			else if ( !strncmp (propdata, "mpss", sizeof("mpss")))
				ss_id = SS_ID_MODEM;				
			else
				QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Ignoring subsystem %s", (char *)propdata);
		}
	}

	if (enabled == 1)
		*mask = *mask | ss_id;

	return EOK;
}

static int register_for_ssr_events(glink_ssr_virt_dev_t *dev)
{
	uint64_t client_magic = 0;
	uint32_t event_mask = 0;
	char * client_name = "vdev_glink_ssr";
	void *handle;
	const void *fdt;
	int offset;
	int ret = EOK;

	fdt = fdt_get_root();
    if (!fdt) {
        QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "/glink-ssr fdt_get_root failed");
        return -1;
    }
	offset = fdt_path_offset(fdt, "/glink-ssr");

	if (offset < 0) {
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "/glink-ssr not found in device tree");
		return -1;
	}
	for ( offset = fdt_first_subnode (fdt, offset); offset > 0;
		  offset = fdt_next_subnode(fdt, offset ) ) {
		ret = get_ss_name(fdt, offset, &event_mask);
		if ( ret != EOK )
			return -1;
	}

	if (event_mask != 0) {
		event_mask = event_mask << SS_ID_SHIFT |
			SSR_EVENT_POWERDOWN | SSR_EVENT_POWER_UP | SSR_EVENT_RESTART_COMPLETE |
			SSR_SOC_RESTART | SSR_EVENT_RESTART_FAILED;
		
		ret = ssr_register_callback_with_ctx(client_magic, &ssr_callback_function, event_mask,
										&handle, client_name, dev);
	}
	return ret;
									
}

/**
 * Perform a glink_ssr_virt device specific state initialization
 * @param           dev     glink_ssr_virt device
 * @returns         0 if glink_ssr virt addr is set, else -1
 */
static int init_glink_ssr_virt_device ( glink_ssr_virt_dev_t *dev ) 
{
	int ret;

	if(!dev->glink_ssr_virt_base)
	{
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"glink_virt_addr not set");
		return -1;
	}
	trap_regions[REGION_GLINK_SSR_VIRT_IDX].guest.location = dev->glink_ssr_virt_base;

	dev->glink_coid = name_open(GLINK_SSR_ATTACH_POINT, 0);
	if ( dev->glink_coid == -1 )
	{
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"name_open(%s) failed, rc=%d", GLINK_SSR_ATTACH_POINT, errno);
		return -1;
	}

	
	dev->ssr_event_coid = -1;

    dev->wcnss_coid = -1;

	ret = pthread_cond_init(&dev->cond, NULL);
	if (ret != EOK) {
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "pthread_cond_init() failed");
		return -1;
	}
	ret = pthread_mutex_init(&dev->mutex, NULL);
	if (ret != EOK) {
		QVM_LOG(GLINK_SSRVIRTLOG_ERROR, "pthread_mutex_init() failed");
		return -1;
	}

	return 0;
}

/**
 * Handle process-container operations.
 * @param       vdp               virtual device
 * @param       ctrl                operation
 * @param       arg                parameters for the operation
 * @return          EOK on success, an error otherwise.
 */
static int glink_ssr_virt_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
{

	int rc = EOK;
	glink_ssr_virt_dev_t *dev = vdp->v_device;
	const char* arg_str;
	ssr_ss_intr_t *old_ss_intr = NULL;
	
	switch (ctrl) {
		case VDEV_CTRL_OPTIONS_START:
			/*initialize vdev state*/
			vdp->v_block.type = QST_DEFAULT_TYPE_IO;
			break;

		case VDEV_CTRL_OPTIONS_END:
			if ( EOK != ( rc = init_glink_ssr_virt_device  ( dev )) )
			{
				QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Couldn't initialize glink_virt_device");
				break;
			}

			trap_regions[REGION_GLINK_SSR_VIRT_IDX].vdev = vdp;
			//setup 'cookie' to array index for quick lookup in vread/vwrite
			trap_regions[REGION_GLINK_SSR_VIRT_IDX].cookie = REGION_GLINK_SSR_VIRT_IDX;
			qvm_region_set(vdp->v_gsp, 1, &trap_regions[0]);

			rc = register_for_ssr_events(dev);
			if ( EOK != rc)
				QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Failed to register for SSR events");

			break;

		case (VDEV_CTRL_FIRST_OPTIDX + OPTION_GLINK_INTR):

			arg_str = arg;
			for ( ;; ) {

				old_ss_intr = dev->ss_intr;
				dev->ss_intr = realloc(dev->ss_intr, (dev->subsystem_count+1) * sizeof(*dev->ss_intr));
				if(dev->ss_intr == NULL) {
					QVM_LOG(GLINK_SSRVIRTLOG_ERROR,"Failed to allocate memory to dev->ss_intr");
					free(old_ss_intr);
					break;
				}
				ssr_ss_intr_t * const intr = &dev->ss_intr[dev->subsystem_count];
				arg_str = parse_intr(arg_str, &intr->s_intr);
				qvm_intr_assign(intr->s_intr.pic, intr->s_intr.param, QIAT_EXCLUSIVE, &intr->s_intr);
				intr->intr_num = intr->s_intr.param;

				dev->subsystem_count++;
				if (*arg_str != ',') 
					break;
				arg_str++;
			}
			break;

		case (VDEV_CTRL_FIRST_OPTIDX + OPTION_GLINK_SSR_VIRT_BASE):
			dev->glink_ssr_virt_base = strtoul(arg, NULL, 16);
			QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"glink_virt_base = 0x%08x", dev->glink_ssr_virt_base);
			break;
#ifdef WLAN_SSR
        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_GLINK_SSR_VIRT_WLAN):
            g_wlan_ssr = strtoul(arg, NULL, 0);
            QVM_LOG(GLINK_SSRVIRTLOG_DEBUG, "wlan_ssr = %u", g_wlan_ssr);
            break;
#endif
		case (VDEV_CTRL_FIRST_OPTIDX + OPTION_GLINK_SSR_VIRT_VERBOSE):
			g_verbose = strtoul(arg, NULL, 16);

			if(GLINK_SSRVIRTLOG_DEBUG < g_verbose)
			{
				g_verbose = GLINK_SSRVIRTLOG_DEBUG;
			}
			QVM_LOG(GLINK_SSRVIRTLOG_DEBUG,"g_verbose = %u", g_verbose);
			break;

		case VDEV_CTRL_GEN_FDT:
		case VDEV_CTRL_BOOTUP:
		case VDEV_CTRL_RESET:
		case VDEV_CTRL_TERMINATE:
		case VDEV_CTRL_STATUS:
		case VDEV_CTRL_GEN_ACPI:
		case VDEV_CTRL_POWER:
		default:
			break;
	}

	return rc ;
}

/**
 * Register the virtual device into the framework.
 */
static void __attribute__((constructor)) glink_ssr_virt_register(void)
{
	static const char * const glink_ssr_virt_options[] = { "base", "wlan_ssr", "verbose", "intr", NULL };

	static struct qvm_vdev_factory glink_ssr_virt_factory = {
		.next = NULL, // patched
		.control = glink_ssr_virt_control,
		.vread = vdev_vread,
		.vwrite = vdev_vwrite,
		.pulse = vdev_pulse_handler, 
		.option_list = glink_ssr_virt_options,
		.name = NULL, // patched
		.factory_flags = QVFF_NONE,
		.acc_sizes = 1u << sizeof(uint32_t),
		.extra_space = sizeof(glink_ssr_virt_dev_t),
	};
	qvm_register_vdev_factory(&glink_ssr_virt_factory, QVM_VDEV_ABI);
}

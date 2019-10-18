#include "libdiscom.h"


#define DEV_NAME "/dev/discom/dpy-"
static screen_display_t display = NULL;
static screen_context_t screen_ctx = NULL;
static screen_display_t *screen_displays = NULL;
static int discom_fd;

static dc_data_packet_t data={
    .pipe = 0,
    .src.hor_off = 0,
    .src.ver_off = 0,
    .src.hor_size = 0,
    .src.ver_size = 0,
    .exp_crc = 0,
    .cur_crc = 0,
    .compare_result = 0
};

void Discom_Exit(void)
{
	close(discom_fd);
}

int Discom_Init(int display_id, int pipeline)
{
	int sts,i;
	char    name[80];

    sts = screen_create_context(&screen_ctx, SCREEN_DISPLAY_MANAGER_CONTEXT);
    if (sts) {
        perror("screen_create_context");
        goto fail;
    }

    int ndisplays = 0;
    sts = screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &ndisplays);
    if (sts) {
        perror("SCREEN_PROPERTY_DISPLAY_COUNT");
        goto fail;
    }

    if ((1 <= display_id) && (display_id <= ndisplays))
    {
        sprintf(name, "%s%d", DEV_NAME, display_id);
    } else {
        fprintf(stderr, "invalid -display_id argument command\n");
        goto fail;
    }

    screen_displays = calloc(ndisplays, sizeof(*screen_displays));
    if (!screen_displays) {
        perror( "could not allocate memory for display list");
        sts = -1;
        goto fail;
    }

    sts = screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)screen_displays);
    if (sts) {
        perror( "SCREE_PROPERTY_DISPLAYS");
        goto fail;
    }

    for (i = 0; i < ndisplays; i++) {
        int val = 0;
        screen_get_display_property_iv(screen_displays[i], SCREEN_PROPERTY_ID, &val);

        if (val == display_id) {
            display = screen_displays[i];
        }
    }
	
    discom_fd = open( name, O_RDWR);
    if (discom_fd < 0)
    {
        fprintf(stderr, "open dev (%s) failed errno\n",name);
        return -1;
    }
	data.pipe = pipeline;
	
	return 0;
fail:
	return -1;
}

int Get_Discom_CRC(int posx, int posy, int sizex, int sizey)
{
	int status = 0,sts,error;
    data.src.hor_size = sizex;
    data.src.ver_size = sizey;
    data.src.hor_off = posx;
    data.src.ver_off = posy;
	data.exp_crc = 0xffffffff;

    /*Attach DISCOM unit*/
    error = devctl (discom_fd, DCMD_DISCOM_ATTACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status )
    {
        fprintf(stderr, "Error setting RTS: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // wait for DPR router to UIF
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /* Set rectangle */
    error = devctl (discom_fd, DCMD_DISCOM_SOURCE_RECTANGLE, &data, sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_SET_RECTANGLE: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }
	
    /*Execute DISCOM*/
    error = devctl (discom_fd, DCMD_DISCOM_START, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_START: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the beginning of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /*Stop DISCOM */
    error = devctl (discom_fd, DCMD_DISCOM_STOP, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_STOP: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the end of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }
	
    /*get CRC value of current image*/
    error = devctl (discom_fd, DCMD_DISCOM_GET_CURRENT_CRC, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_GET_CURRENT_CRC: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    //fprintf(stderr, "    Current CRC     : %x\n", data.cur_crc);
	
	
    /*Detach DISCOM unit*/
    error = devctl (discom_fd, DCMD_DISCOM_DETACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_DETACH: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

	return data.cur_crc;	
fail:
	return -1;	
}


int Check_Discom_CRC(int posx, int posy, int sizex, int sizey, int expectation)
{
	int status = 0,sts,error;
    data.src.hor_size = sizex;
    data.src.ver_size = sizey;
    data.src.hor_off = posx;
    data.src.ver_off = posy;
	data.exp_crc = expectation;

	
    /*Attach DISCOM unit*/
    error = devctl (discom_fd, DCMD_DISCOM_ATTACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status )
    {
        fprintf(stderr, "Error setting RTS: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // wait for DPR router to UIF
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /* Set rectangle */
    error = devctl (discom_fd, DCMD_DISCOM_SOURCE_RECTANGLE, &data, sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_SET_RECTANGLE: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }
	
    /*Set the expectation value of the CRC code*/
    error = devctl (discom_fd, DCMD_DISCOM_SET_EXPECTED_CRC, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_SET_EXPECTED_CRC: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }
	
    /*Execute DISCOM*/
    error = devctl (discom_fd, DCMD_DISCOM_START, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_START: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the beginning of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }

    /*Stop DISCOM */
    error = devctl (discom_fd, DCMD_DISCOM_STOP, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_STOP: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    sts = screen_wait_vsync(display);       // the end of the frame processing
    if (sts) {
        perror( "SCREEN WAIT VSYNC");
        goto fail;
    }
	
    /*Compare result between the CRC and its expectation*/
    error = devctl (discom_fd, DCMD_DISCOM_COMPARE_VALUE, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_COMPARE_VALUE: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

    //fprintf(stderr, "    Expect CRC      : %x \n", data.exp_crc);
    //fprintf(stderr, "    Compare result     : %s \n", (data.compare_result) ? "Same" : "Not same");
	
	
    /*Detach DISCOM unit*/
    error = devctl (discom_fd, DCMD_DISCOM_DETACH, &data,sizeof(dc_data_packet_t), &status);
    if (error || status)
    {
        fprintf(stderr, "DCMD_DISCOM_DETACH: %s\n", strerror ( error ));
        exit(EXIT_FAILURE);
    }

	return data.compare_result;	
fail:
	return -1;	
}



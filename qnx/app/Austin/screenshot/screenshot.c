#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>

void write_bitmap_header(int nbytes, int fd, const int size[])
{
	char header[54];
	
	/* Set standard bitmap header */
	header[0] = 'B';
	header[1] = 'M';
	header[2] = nbytes & 0xff;
	header[3] = (nbytes >> 8) & 0xff;
	header[4] = (nbytes >> 16) & 0xff;
	header[5] = (nbytes >> 24) & 0xff;
	header[6] = 0;
	header[7] = 0;
	header[8] = 0;
	header[9] = 0;
	header[10] = 54;
	header[11] = 0;
	header[12] = 0;
	header[13] = 0;
	header[14] = 40;
	header[15] = 0;
	header[16] = 0;
	header[17] = 0;
	header[18] = size[0] & 0xff;
	header[19] = (size[0] >> 8) & 0xff;
	header[20] = (size[0] >> 16) & 0xff;
	header[21] = (size[0] >> 24) & 0xff;
	header[22] = -size[1] & 0xff;
	header[23] = (-size[1] >> 8) & 0xff;
	header[24] = (-size[1] >> 16) & 0xff;
	header[25] = (-size[1] >> 24) & 0xff;
	header[26] = 1;
	header[27] = 0;
	header[28] = 32;
	header[29] = 0;
	header[30] = 0;
	header[31] = 0;
	header[32] = 0;
	header[33] = 0;
	header[34] = 0; /* image size*/
	header[35] = 0;
	header[36] = 0;
	header[37] = 0;
	header[38] = 0x9;
	header[39] = 0x88;
	header[40] = 0;
	header[41] = 0;
	header[42] = 0x9l;
	header[43] = 0x88;
	header[44] = 0;
	header[45] = 0;
	header[46] = 0;
	header[47] = 0;
	header[48] = 0;
	header[49] = 0;
	header[50] = 0;
	header[51] = 0;
	header[52] = 0;
	header[53] = 0;

	/* Write bitmap header to file */
	write(fd, header, sizeof(header));
}

void write_bitmap_file(const int size[], const char* screenshot_ptr, const int screenshot_stride)
{
	int nbytes;							/* number of bytes of the bimap */
	int fd;								/* file descriptor */
	int i;								/* iterator to iterate over the screenshot buffer */
	char *fname = 	"screenshot.bmp";	/* bitmap filename */

	/* Calculate the size of the bitmap */
	nbytes = size[0] * size[1] * 4;

	/* Open file*/
	fd = creat(fname, S_IRUSR | S_IWUSR);

	 /* Write the standard bitmap header */
	write_bitmap_header(nbytes, fd, size);

	/* Write screenshot buffer contents to file */
	for (i = 0; i < size[1]; i++) {
		write(fd, screenshot_ptr + i * screenshot_stride, size[0] * 4);
	}

}


int main(int argc, char **argv)
{
	screen_context_t screen_ctx;
	screen_display_t screen_disp = NULL;
	screen_display_t *screen_displays;
	screen_pixmap_t screen_pix;
	screen_buffer_t screen_buf;
	char *fname = "screenshot.bmp";
	char *disp = NULL;
	char *tok;
	char header[54];
	int size[2] = { 0, 0 };
	int val;
	void *pointer;
	int stride;
	int nbytes;
	int count, id, type;
	int i, fd, rc;

	/* Parse command-line input. */
	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-size=", strlen("-size=")) == 0) {
			tok = argv[i] + strlen("-size=");
			size[0] = atoi(tok);
			while (*tok >= '0' && *tok <= '9') {
				tok++;
			}
			size[1] = atoi(tok+1);
		} else if (strncmp(argv[i], "-display=", strlen("-display=")) == 0) {
			disp = argv[i] + strlen("-display=");
		} else if (strncmp(argv[i], "-file=", strlen("-file=")) == 0) {
			fname = argv[i] + strlen("-file=");
		} else {
			fprintf(stderr, "invalid command line option: %s\n", argv[i]);
		}
	}

	/* Create the privileged context so that the display properties can be accessed. */
	screen_create_context(&screen_ctx, SCREEN_DISPLAY_MANAGER_CONTEXT);

	/* Get the number of supported displays with this context. */
	count = 0;
	screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, 
	                                    &count);

	/* Get the displays for this context. */
	if (count > 0) {
		screen_displays = calloc(count, sizeof(screen_display_t));
		screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS,
		                                    (void **)screen_displays);

		/* If no display was specified, use the first supported display available for this context. */
		if (!disp) {
			screen_disp = screen_displays[0];
		/* Otherwise, determine which display has been requested for the screen shot. */
		} else {
			if (isdigit(*disp)) {
				id = strtoul(disp, 0, NULL);
				for (i = 0; i < count; i++) {
					screen_get_display_property_iv(screen_displays[i], SCREEN_PROPERTY_ID,
					                                    &val);
					if (val == id) {
						screen_disp = screen_displays[i];
						break;
					}
				}
			} else {
				if (!strcmp(disp, "internal")) {
					type = SCREEN_DISPLAY_TYPE_INTERNAL;
				} else if (!strcmp(disp, "rgb")) {
					type = SCREEN_DISPLAY_TYPE_COMPONENT_RGB;
				} else if (!strcmp(disp, "dvi")) {
					type = SCREEN_DISPLAY_TYPE_DVI;
				} else if (!strcmp(disp, "hdmi")) {
					type = SCREEN_DISPLAY_TYPE_HDMI;
				} else {
					type = SCREEN_DISPLAY_TYPE_OTHER;
				}
				for (i = 0; i < count; i++) {
					screen_get_display_property_iv(screen_displays[i], SCREEN_PROPERTY_TYPE,
					                                    &val);
					if (val == type) {
						screen_disp = screen_displays[i];
						break;
					}
				}
			}
		}

		free(screen_displays);
	}

	if (!screen_disp) {
		fputs("no displays\n", stderr);
		return 1;
	}

	/* Create pixmap for the screen shot. */
	screen_create_pixmap(&screen_pix, screen_ctx);

	/* Set usage flag. */
	val = SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE;
	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_USAGE, &val);


	/* Set format. */
	val = SCREEN_FORMAT_RGBA8888;
	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_FORMAT, &val);
	
	/* If size is not specified, get the size from the display. */
	if (size[0] <= 0 || size[1] <= 0) {
		screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, size);
	}

	/* Set the pixmap buffer size. */
	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_BUFFER_SIZE, size);

	/* Create the pixmap buffer and get a handle to the buffer. */
	screen_create_pixmap_buffer(screen_pix);

	screen_get_pixmap_property_pv(screen_pix, SCREEN_PROPERTY_RENDER_BUFFERS,
	                                   (void **)&screen_buf);

	/* Get the pointer to the buffer. */
	screen_get_buffer_property_pv(screen_buf, SCREEN_PROPERTY_POINTER, &pointer);

	/* Get the stride. */
	screen_get_buffer_property_iv(screen_buf, SCREEN_PROPERTY_STRIDE, &stride);

	/* Take the display screen shot. */
	screen_read_display(screen_disp, screen_buf, 0, NULL, 0);

    /* Write the screenshot buffer to a bitmap file*/
	write_bitmap_file(size, pointer, stride);

	/* Perform necessary clean-up. */
	screen_destroy_pixmap(screen_pix);
	screen_destroy_context(screen_ctx);

	return 0;
}

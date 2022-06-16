#include "yuv_to_jpg.h"


#if YUVTOJPGFILE

#define OUTPUT_BUF_SIZE  4096
typedef struct 
{
  struct jpeg_destination_mgr pub; /* public fields */
  JOCTET * buffer;    /* start of buffer */
  unsigned char *outbuffer;
  int outbuffer_size;
  unsigned char *outbuffer_cursor;
  int *written;
 
} mjpg_destination_mgr;
typedef mjpg_destination_mgr *mjpg_dest_ptr;
 
/******************************************************************************
函数功能: 初始化输出的目的地
******************************************************************************/
METHODDEF(void) init_destination(j_compress_ptr cinfo) 
{
  mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;
 
  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE, OUTPUT_BUF_SIZE * sizeof(JOCTET));
 
  *(dest->written) = 0;
 
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}
 
/******************************************************************************
函数功能: 当jpeg缓冲区填满时调用
******************************************************************************/
METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo) 
{
  mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;
 
  memcpy(dest->outbuffer_cursor, dest->buffer, OUTPUT_BUF_SIZE);
  dest->outbuffer_cursor += OUTPUT_BUF_SIZE;
  *(dest->written) += OUTPUT_BUF_SIZE;
 
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
 
  return TRUE;
}
 
/******************************************************************************
函数功能:在写入所有数据之后，由jpeg_finish_compress调用。通常需要刷新缓冲区。
******************************************************************************/
METHODDEF(void) term_destination(j_compress_ptr cinfo)
{
  mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
 
  /* Write any data remaining in the buffer */
  memcpy(dest->outbuffer_cursor, dest->buffer, datacount);
  dest->outbuffer_cursor += datacount;
  *(dest->written) += datacount;
}
 
/******************************************************************************
功能描述：初始化输出流
函数参数:
		j_compress_ptr cinfo  :保存JPG图像压缩信息的结构体地址
		unsigned char *buffer :存放压缩之后的JPG图片的缓冲区首地址
		int size              :源图像字节总大小
		int *written          :存放压缩之后的JPG图像字节大小
******************************************************************************/
GLOBAL(void) dest_buffer(j_compress_ptr cinfo, unsigned char *buffer, int size, int *written)
{
  mjpg_dest_ptr dest;
 
  if (cinfo->dest == NULL) {
    cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(mjpg_destination_mgr));
  }
 
  dest = (mjpg_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->outbuffer = buffer;
  dest->outbuffer_size = size;
  dest->outbuffer_cursor = buffer;
  dest->written = written;
}
 
/************************************************
功能描述：将YUV格式的数据转为JPG格式。
函数参数:
	int Width    源图像宽度
	int Height   源图像高度
	int size   	 源图像字节总大小
	unsigned char *yuv_buffer :存放YUV源图像数据缓冲区的首地址
	unsigned char *jpg_buffer :存放转换之后的JPG格式数据缓冲区首地址
	int quality               :jpg图像的压缩质量(值越大质量越好,图片就越清晰,占用的内存也就越大)
							一般取值范围是: 10 ~ 100 。 填10图片就有些模糊了，一般的JPG图片都是质量都是80。
返回值：压缩之后的JPG图像大小
**************************************************************/
int yuv_to_jpeg(int Width,int Height,int size,unsigned char *yuv_buffer, unsigned char *jpg_buffer, int quality) 
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];
  unsigned char *line_buffer, *yuyv;
  int z;
  static int written;
	
  /*1. 解压之前的初始化*/
  line_buffer =(unsigned char *)calloc(Width*3,1);
  yuyv=yuv_buffer; //得到图像源数据
 
  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_compress (&cinfo);
  
  /* 原版jpeglib库的标准输出初始化函数,只能填文件指针: jpeg_stdio_dest (&cinfo, file); */
  /* 修改之后的标准输出初始化函数,将输出指向内存空间*/
  dest_buffer(&cinfo, jpg_buffer, size, &written);
	
  cinfo.image_width = Width;
  cinfo.image_height =Height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
 
  jpeg_set_defaults (&cinfo);
  jpeg_set_quality (&cinfo, quality, TRUE);
 
  jpeg_start_compress (&cinfo, TRUE);
 
  /*2. YUV转RGB格式*/
  z = 0;
  while (cinfo.next_scanline < Height) 
  {
    int x;
    unsigned char *ptr = line_buffer;
 
    for (x = 0; x < Width; x++) {
      int r, g, b;
      int y, u, v;
 
      if (!z)
        y = yuyv[0] << 8;
      else
        y = yuyv[2] << 8;
      u = yuyv[1] - 128;
      v = yuyv[3] - 128;
 
      r = (y + (359 * v)) >> 8;
      g = (y - (88 * u) - (183 * v)) >> 8;
      b = (y + (454 * u)) >> 8;
 
      *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
      *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
      *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);
 
      if (z++) {
        z = 0;
        yuyv += 4;
      }
    }
	/*3.进行JPG图像压缩(一行一行压缩)*/
    row_pointer[0] = line_buffer;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
   
   /*4. 释放压缩时占用的内存空间*/
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  free (line_buffer);
  
   /*5. 返回压缩之后JPG图片大小*/
  return (written);
}

void Savejpegfile(struct MainInfo *maininfo)
{
	static int jpg_cnt = 0;
	int jpg_size;
	char jpg_file_name[30];
	FILE *jpg_file1;
	unsigned char *jpg_p = malloc(IMAGE_WIDTH*IMAGE_HEIGHT*3);
	
	/*YUV数据转JPEG格式*/
	jpg_size=yuv_to_jpeg(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_HEIGHT*IMAGE_WIDTH*3, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer, jpg_p, 80);
	sprintf(jpg_file_name,"./jpg/%d.jpg",jpg_cnt++);
	printf("图片名称:%s,字节大小:%d\n",jpg_file_name, jpg_size);
	jpg_file1=fopen(jpg_file_name, "wb");
	fwrite(jpg_p, jpg_size, 1, jpg_file1);
	fclose(jpg_file1);
	
	free(jpg_p);
}

#endif

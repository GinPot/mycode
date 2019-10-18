#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <linux/fb.h>
#include <jpeglib.h>
#include <jerror.h>

struct fb_dev
{
        //for frame buffer
        int fb;
        void *fb_mem;   //frame buffer mmap
        int fb_width, fb_height, fb_line_len, fb_size;
        int fb_bpp;
} fbdev;

//得到framebuffer的长、宽和位宽，成功则返回0，失败返回－1 
int fb_stat(int fd)
{
        struct fb_fix_screeninfo fb_finfo;
        struct fb_var_screeninfo fb_vinfo;

        if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo))
        {
                perror(__func__);
                return (-1);
        }

        if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo))
        {
                perror(__func__);
                return (-1);
        }

        fbdev.fb_width = fb_vinfo.xres;
        fbdev.fb_height = fb_vinfo.yres;
        fbdev.fb_bpp = fb_vinfo.bits_per_pixel;
        fbdev.fb_line_len = fb_finfo.line_length;
        fbdev.fb_size = fb_finfo.smem_len;

        return (0);
}

//转换RGB888为RGB565（因为当前LCD是采用的RGB565显示的）
unsigned short RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
        unsigned short B = (blue >> 3) & 0x001F;
        unsigned short G = ((green >> 2) << 5) & 0x07E0;
        unsigned short R = ((red >> 3) << 11) & 0xF800;

        return (unsigned short) (R | G | B);
}
//释放framebuffer的映射
int fb_munmap(void *start, size_t length)
{
        return (munmap(start, length));
}

//显示一个像素点的图像到framebuffer上
int fb_pixel(void *fbmem, int width, int height, int x, int y, unsigned short color)
{
        if ((x > width) || (y > height))
                return (-1);

        unsigned short *dst = ((unsigned short *) fbmem + y * width + x);

        *dst = color;
        return 0;
}

int main(int argc, char **argv)
{
     int fb;
     FILE *infile;
     struct jpeg_decompress_struct cinfo;
     int x,y;
     unsigned char *buffer;
     char s[15];
     struct jpeg_error_mgr jerr;

      if ((fb = open("/dev/fb0", O_RDWR)) < 0)                        //打开显卡设备
      {
                perror(__func__);
                return (-1);
      }

        //获取framebuffer的状态
        fb_stat(fb);                                                    //获取显卡驱动中的长、宽和显示位宽
        printf("frame buffer: %dx%d,  %dbpp, 0x%xbyte= %d\n",
                fbdev.fb_width, fbdev.fb_height, fbdev.fb_bpp, fbdev.fb_size, fbdev.fb_size);

        //映射framebuffer的地址
        fbdev.fb_mem = mmap (NULL, fbdev.fb_size, PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);

        if ((infile = fopen("lcd.jpg", "rb")) == NULL)
        {
                fprintf(stderr, "open %s failed\n", s);
                exit(-1);
        }
        ioctl(fb, FBIOBLANK,0);                        //打开LCD背光

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        //导入要解压的Jpeg文件infile
        jpeg_stdio_src(&cinfo, infile);

        //读取jpeg文件的文件头
        jpeg_read_header(&cinfo, TRUE);

        //开始解压Jpeg文件，解压后将分配给scanline缓冲区，
        jpeg_start_decompress(&cinfo);

        buffer = (unsigned char *) malloc(cinfo.output_width
                                        * cinfo.output_components);
        y = 0;
		
		printf("cinfo.output_scanline = %d ,cinfo.output_height = %d\n",cinfo.output_scanline,cinfo.output_height);
        while (cinfo.output_scanline < cinfo.output_height)
        {
                jpeg_read_scanlines(&cinfo, &buffer, 1);
                if(fbdev.fb_bpp == 32)
                {
                       unsigned short color;
                       for (x = 0; x < cinfo.output_width; x++)
                       {
                                color = RGB888toRGB565(buffer[x * 3],
                                                            buffer[x * 3 + 1], buffer[x * 3 + 2]);
                                fb_pixel(fbdev.fb_mem, fbdev.fb_width, fbdev.fb_height, x, y, color);
                       }
                 }
                 else if(fbdev.fb_bpp == 24)
                 {
                       memcpy((unsigned char *)fbdev.fb_mem + y * fbdev.fb_width * 3, buffer,
                                                        cinfo.output_width * cinfo.output_components);
                 }
                y++;
        }
        
        //完成Jpeg解码，释放Jpeg文件
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        //释放帧缓冲区
        free(buffer);

        //关闭Jpeg输入文件
        fclose(infile);

       fb_munmap(fbdev.fb_mem, fbdev.fb_size);                                 //释放framebuffer映射

       close(fb); 

}
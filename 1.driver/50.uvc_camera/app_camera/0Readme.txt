export LD_LIBRARY_PATH=/root/work/linux/driver/50.uvc_camera/app_camera/lib/libcedarc/cedarc:/root/work/linux/driver/50.uvc_camera/app_camera/lib/libcedarc/toolchain-sunxi-aarch64-glibc:/root/work/linux/driver/50.uvc_camera/app_camera/lib/libjpeg:/root/work/linux/driver/50.uvc_camera/app_camera/lib/librtmp


/*******************************************************************************************************************************/

cedarcx编译:
	./configure --prefix=/root/work/linux/driver/50.uvc_camera/app_camera/opensource/libcedarc-master/install --host=aarch64-linux-gnu LDFLAGS="-L/root/work/linux/driver/50.uvc_camera/app_camera/opensource/libcedarc-master/library/toolchain-sunxi-aarch64-glibc/"
	make
	make install
	cp /root/work/display/libcedarc-master/install/lib/* /root/work/display/libcedarc-master/install/tmp


	demo:
		cd ~/work/display/libcedarc-master/demo/vencoderDemo
		export LD_LIBRARY_PATH=/root/work/display/libcedarc-master/install/tmp:/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/install/lib/
		gcc EncoderTest.c -o vencoderDemo -I/root/work/display/libcedarc-master/ -I/root/work/display/libcedarc-master/base/include -I/root/work/display/libcedarc-master/include -L/root/work/display/libcedarc-master/library/toolchain-sunxi-aarch64-glibc/ -L/root/work/display/libcedarc-master/install/lib/ -lpthread -lm -lcdc_base -lVE -lvencoder -lMemAdapter

		./vencoderDemo -i out_yuv420p.yuv -o out_yuv420p_640x360.h264 -f 0 -s 1 -d 1 -b 4608000

		./vencoderDemo -i akiyo_cif.yuv -o akiyo_cif.h264 -f 0 -s 288 -d 288 -b 4608000


		gcc demojpeg.c -o demojpeg -I/root/work/display/libcedarc-master/ -I/root/work/display/libcedarc-master/base/include \
		-I/root/work/display/libcedarc-master/include -L/root/work/display/libcedarc-master/library/toolchain-sunxi-aarch64-glibc/ \
		-L/root/work/display/libcedarc-master/install/lib/ \
		-lpthread -lm -lcdc_base -lVE -lvdecoder -lMemAdapter -lvideoengine -ldl

/*******************************************************************************************************************************/

librtmp下面的测试代码编译:
	gcc librtmp_send264.c simplest_librtmp_send264.c -o rtmp -L/root/work/display/server/rtmpdump/librtmp/ -lrtmp -lm

	
	
	
	
/*******************************************************************************************************************************/
libcedarc_libcedarx(CedarC-v1.1.9):

编译: libcedarc_libcedarx\nonfree\allwinner\tina_multimedia\libcedarx\ReadMe
	



	gcc demojpeg.c -o demojpeg -I/root/work/display/libcedarc-master/ -I/root/work/display/libcedarc-master/base/include \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/libcore/base/include/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/libcore/stream/include/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/libcore/parser/include/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/libcore/common/iniparser/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/external/include/adecoder \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/external/include/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/ \
	-I/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarc/include \
	-L/root/work/display/libcedarc-master/library/toolchain-sunxi-aarch64-glibc/ \
	-L/root/work/display/libcedarc-master/install/lib/ \
	-L/root/tmp/handsomefeed-2d4cd0d1e1e7cf61b0a8dc2b9d1ce7d50b7f8780/nonfree/allwinner/tina_multimedia/libcedarx/install/lib/ \
	-lcdc_base -lpthread -lm -lVE -lvdecoder -lMemAdapter -lvideoengine -ldl -lcrypto -lz -lssl -lcdx_parser -lcdx_common -lcdx_base



	在GitHub上搜索CedarC-v1.1.9版本信息，可搜到较新版本的编解码库	

/*******************************************************************************************************************************/

libjpeg-turbo:
	https://blog.csdn.net/Dancer__Sky/article/details/78631577
	https://sourceforge.net/projects/libjpeg-turbo/files/

	apt-get install nasm  libtool cmake
	
	$ mkdir build
	$ cd build
	$ mkdir install_lib
	$ cmake .. -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=/root/tmp/libjpeg-turbo-2.1.4/build/install_lib/usr/local   -DCMAKE_INSTALL_MANDIR:PATH=/root/tmp/libjpeg-turbo-2.1.4/build/install_lib/usr/local/share/man -DCMAKE_INSTALL_DOCDIR:PATH=/root/tmp/libjpeg-turbo-2.1.4/build/install_lib/usr/local/share/doc/libjpeg-turbo-2.1.4  -DWITH_JPEG8=1
	$ make
	$ make install
	$ make test
	
	
	-G “Unix Makefiles” 生成unix类型系统的makefile文件。
	-DCMAKE_INSTALL_PREFIX:PATH=/usr/local 指定库安装路径，否则默认为/opt/libjpeg-turbo。
	-DCMAKE_INSTALL_MANDIR:PATH=/usr/local/share/man 指定man文件（相关命令参考信息）安装路径。
	-DWITH_JPEG8=1 这个开关启用与libjpeg 8版本API兼容。
	-DCMAKE_INSTALL_DOCDIR:PATH=/usr/local/share/doc/libjpeg-turbo-1.5.9 指定文档安装路径。

/*******************************************************************************************************************************/
	
ffmpeg:
	cp /root/work/display/libcedarc-master/install/lib/*  /root/work/display/codec/ffmpeg-4.2.2/tmp/lib
	cd /root/work/display/codec/ffmpeg-4.2.2/
	./configure --prefix=/root/work/display/codec/ffmpeg-4.2.2/tmp --enable-omx --enable-shared
	export LD_LIBRARY_PATH=/root/work/display/codec/ffmpeg-4.2.2/tmp/lib
	./ffmpeg -f v4l2 -r 30 -s 640x360 -i /dev/video0 -pix_fmt yuyv422 -c:v  h264_omx output0.avi-y 
	./ffmpeg -f v4l2 -r 30 -s 640x360 -i /dev/video0 -pix_fmt yuyv422 -f flv rtmp://192.168.137.11:1935/live/111
	./ffmpeg -i /root/test.264 -f flv rtmp://192.168.137.11:1935/live/111

	gcc encode_video.c -o encode_video  -L/root/work/display/codec/ffmpeg-4.2.2/tmp/lib -I/root/work/display/codec/ffmpeg-4.2.2/tmp/include/ -lavdevice -lavformat -lavfilter -lavcodec -lswresample -lswscale -lavutil

/*******************************************************************************************************************************/	
	

libyuv:
	apt-get install g++ libjpeg-dev

	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=/root/tmp/libyuv-main/build/build_libs -DCMAKE_BUILD_TYPE="Release" ..
	cmake --build . --target install --config Release

	cmake ..
    cmake --build .
	

 

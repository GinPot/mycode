TARGET 	:= test

#LDFLAGS := -lpthread -lm -lcdc_base -lVE -lvencoder -lvdecoder -lvideoengine -lMemAdapter -ldl \
#		   -lrtmp \
#		   -lturbojpeg -ljpeg
#
#INCLUDE := -I./inc/libcedarc/ \
#		   -I./inc/librtmp/ \
#		   -I./inc/libjpeg/
#		   
#LIBSHARED := -L./lib/libcedarc/toolchain-sunxi-aarch64-glibc/ -L./lib/libcedarc/cedarc/ \
#			 -L./lib/librtmp/ \
#			 -L./lib/libjpeg/

SRC_C	:= $(wildcard *.c)


all : 
	$(CC) $(SRC_C) -o $(TARGET)  $(INCLUDE) $(LIBSHARED) $(LDFLAGS)


clean:
	rm -rf *.o *.d $(TARGET)



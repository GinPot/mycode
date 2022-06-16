CFLAGS := -Wall -O2 -g

LDFLAGS := -fPIC -shared -lpthread

export CFLAGS


TARGET := libcamer.so


all : 

	$(CC) $(LDFLAGS) CameraSo.c -o $(TARGET) -ljpeg


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

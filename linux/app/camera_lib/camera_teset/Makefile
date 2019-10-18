CFLAGS := -Wall -O2 -g



export CFLAGS


TARGET := CameraTest


all : 

	$(CC) CameraTest.c -o $(TARGET) -ldl


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

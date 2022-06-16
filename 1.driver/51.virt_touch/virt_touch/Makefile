CFLAGS := -Wall -O2 -g



export CFLAGS


TARGET := virt_touch


all : 

	$(CC) virt_touch.c -o $(TARGET) -ldl


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

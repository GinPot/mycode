CFLAGS := -Wall -O2 -g

LDFLAGS :=  -shared 

export CFLAGS


TARGET := libcamer.so


all : 

	$(CC) $(LDFLAGS) CameraSo.c -o $(TARGET) 


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

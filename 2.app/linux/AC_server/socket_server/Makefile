CFLAGS := -Wall -O2 -g

LDFLAGS := -lpthread

export CFLAGS


TARGET := server


all : 

	$(CC) $(LDFLAGS) server.c -o $(TARGET)


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

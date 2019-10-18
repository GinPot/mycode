CFLAGS := -Wall -O2 -g

LDFLAGS := -lpthread

export CFLAGS


TARGET := aclinserver


all : 

	$(CC) $(LDFLAGS) lin_aurt.c aurt.c socket_server.c -o $(TARGET)


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

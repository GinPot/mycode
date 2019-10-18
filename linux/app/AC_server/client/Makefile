CFLAGS := -Wall -O2 -g

LDFLAGS := -lpthread

export CFLAGS


TARGET := app_main


all : 

	$(CC) $(LDFLAGS) client.c app_main.c -o $(TARGET)


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

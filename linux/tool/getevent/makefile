#CC = arm-none-linux-gnueabi-gcc
CPPFLAGS =
CFLAGS = -O2
LDFLAGS = -static
LIBS =
OBJECT = getevent.o
TARGET = getevent

all:$(TARGET)
  $(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS)
clean:
	rm -f $(TARGET) $(OBJECT)
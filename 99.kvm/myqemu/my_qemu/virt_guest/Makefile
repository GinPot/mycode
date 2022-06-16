CC						= aarch64-linux-gnu-gcc
LD						= aarch64-linux-gnu-ld
OBJCOPY					= aarch64-linux-gnu-objcopy
OBJDUMP					= aarch64-linux-gnu-objdump

all:
	$(CC) -c -o start.o start.S
	$(CC) -c -o main.o main.c
	$(LD) -T../guest.lds main.o start.o -o guest
	$(OBJCOPY) -O binary -S guest guest.bin
	$(OBJDUMP) -D guest > guest.dump
	cp guest.bin ../

clean:
	rm -rf *.o *.dump *.bin *.dump guest
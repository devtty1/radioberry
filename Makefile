CC=arm-linux-gnueabihf-gcc -g
CFLAGS=-c -Wall --sysroot=$(MY_SYSROOT)
LDFLAGS=--sysroot=$(MY_SYSROOT) -lrt -lasound -lmpd -lconfuse -L=usr/lib
SOURCES=mcp32xx.c spi_base.c set_volume.c set_volume_alsa.c set_tuner.c mpd_ctl.c radioberry.c set_volume_mpd.c gpio_base.c lcd_ctl.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=radioberry

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -rf *o $(EXECUTABLE)

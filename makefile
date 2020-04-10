# Comment/uncomment to enable/disable debugging code
DEBUG	= 0

ifeq ($(DEBUG),1)
	DEBFLAGS = -g -O -DDEBUG 
else
	DEBFLAGS = -O2
endif

CC=gcc
CFLAGS = -DLINUX $(DEBFLAGS)
# LINKFLAG = -lcurses -lpci -lz
LINKFLAG = -lcurses 
 

TARGET = dbgLinux
# OBJ = dbg.o edit.o memory.o misc.o smbus.o e820.o apic.o common.o ipmitool.o isa.o
# OBJ = dbg.o edit.o memory.o misc.o smbus.o e820.o apic.o common.o ipmitool.o
OBJ = dbg.o edit.o memory.o misc.o smbus.o e820.o apic.o common.o cpu.o 

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)  $(LINKFLAG) 
	rm -rf /home/quake/$(TARGET)
	cp -rf $(TARGET) /home/quake/.

%.o:%.c
	$(CC) $(CFLAGS) -c -o  $@ $<


all: $(TARGET)

clean:
	rm -f *~ core $(TARGET)  $(ARCH)
	rm -f *.o


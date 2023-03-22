TARGET:= Kernel.elf
#INCLUDIRS:= 
#INCLUDE:=$(patsubst %, -I %,$(INCLUDIRS))
SRC:=$(wildcard *.c \
			./boot/*.c \
			./drivers/video/*.c \
			./drivers/console/*.c \
			./kernel/*.c \
			./mm/*.c \
)
OBJS:=$(patsubst %.c,%.o,$(SRC))
CFLAGS:= -no-pie -nostdlib -Wall -ffreestanding \
       -I ../LiuOSBoot/Include/SysGraphics/ \
	   -I ../LiuOSBoot/Include/SysConfig/ \
	   -I ../LiuOSBoot/Include/FontLoader/ \
	   -I ../LiuOSBoot/Include/GeneralFileLoader/ \
	   -I ../LiuOSBoot/Include/Logo/ \
	   -I ../LiuOSBoot/Include/Memory/ \
	   -I ../../MdePkg/Include/ \
	   -I ./Include/  \
	   -I ./Include/linux  \
	   -I ./Include/boot \
	   -I ./Include/hardware \
	   -I ../../BaseTools/Source/C/Include/AArch64/ \
	   -g  #debug mode 

#$(TARGET):$(OBJS)
#	$(CC) $(CFLAGS) $^ -o $@


$(TARGET):$(OBJS)
	$(LD) -Tldscript.ld  $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all:
	echo $(SRC)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
ARMGNU = arm-none-eabi

TARGET = kernel7.bin
SDTARGET = things_to_copy_to_your_sd_card/kernel7.img
DEPS = *.h

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 

all : $(TARGET) 

%.o : %.s
	$(ARMGNU)-as $< -o $@

%.o : %.c $(DEPS)
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

OBJECTS := $(patsubst %.s,%.o,$(wildcard *.s)) $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET) : $(OBJECTS) memmap
	$(ARMGNU)-ld $(OBJECTS) -T memmap -o kernel7.elf
	$(ARMGNU)-objdump -D kernel7.elf > kernel7.list
	$(ARMGNU)-objcopy kernel7.elf -O binary $(TARGET)
	cp $(TARGET) $(SDTARGET)

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.auto
	rm -f **/*.img



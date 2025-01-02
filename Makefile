# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2

# Paths
KERNEL_BUILD_PATH = /lib/modules/$(shell uname -r)/build
BUILD_PATH = $(PWD)/build
MODULE_PATH = $(PWD)

# Executable targets and source files
EX_TARGET = libcamera_mon
EX_SRC = libcamera_mon.c ring_buffer.c data_parser.c filter.c

# Kernel module target and source files
KO_TARGET = libcamera_proc
KO_SRC = libcamera_proc.c

# Kernel module object file
obj-m += libcamera_proc.o

# Default target
all: $(EX_TARGET)  $(KO_TARGET)

# Build ko target
$(KO_TARGET): $(KO_SRC) $(BUILD_PATH)
	make -C $(KERNEL_BUILD_PATH) M=$(MODULE_PATH) modules

	mv $(KO_TARGET).ko $(BUILD_PATH)
	mv $(KO_TARGET).mod $(BUILD_PATH)
	mv $(KO_TARGET).mod.c $(BUILD_PATH)
	mv $(KO_TARGET).mod.o $(BUILD_PATH)
	mv $(KO_TARGET).o $(BUILD_PATH)
	mv modules.order $(BUILD_PATH)
	mv Module.symvers $(BUILD_PATH)

	mv .$(KO_TARGET).ko.cmd $(BUILD_PATH)
	mv .$(KO_TARGET).mod.cmd $(BUILD_PATH)
	mv .$(KO_TARGET).mod.o.cmd $(BUILD_PATH)
	mv .$(KO_TARGET).o.cmd $(BUILD_PATH)
	mv .modules.order.cmd $(BUILD_PATH)
	mv .Module.symvers.cmd $(BUILD_PATH)

# Build binary target
$(EX_TARGET): $(EX_SRC) $(BUILD_PATH)
	$(CC) $(CFLAGS) -o $(EX_TARGET) $(EX_SRC)

	mv $(EX_TARGET) $(BUILD_PATH)

# Build path
$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

# Clean target
clean:
	rm -rf $(BUILD_PATH)
	rm -f $(EX_TARGET)
	make -C $(KERNEL_BUILD_PATH) M=$(MODULE_PATH) clean

# Phony targets
.PHONY: all clean



	


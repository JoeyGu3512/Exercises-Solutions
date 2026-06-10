
ifndef CC
	CC = gcc
endif

# device
ifndef DEVICE
	DEVICE = CL_DEVICE_TYPE_DEFAULT
endif
# CL version
ifndef CL_VER
	CL_VER = 300
endif

CCFLAGS = -lm -O3
CCFLAGS += -D DEVICE=$(DEVICE)
CCFLAGS += -D CL_TARGET_OPENCL_VERSION=$(CL_VER)

LIBS = -fopenmp

COMMON_DIR = ../../C_common

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
       LIBS =  
endif

pi: pi.c $(COMMON_DIR)/wtime.c
	$(CC) $^ $(CCFLAGS) -o $@

clean:
	rm -f pi

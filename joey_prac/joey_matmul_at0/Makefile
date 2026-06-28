
################################################################

# 
# Makefile for an OpenCL matmul practice
# JoeyGu3412QwQ
# 2026June27 15pm
# Msys2 MinGW64 - Win11
#

################################################################

# Set C compiler
ifndef CC
	CC=gcc
endif

# Set CL version
ifndef CL_VERSION
	CL_VERSION=300
endif

# Set Default Device Const
ifndef CL_DEFAULT_DEVICE
	CL_DEFAULT_DEVICE=CL_DEVICE_TYPE_DEFAULT
endif

################################################################

# C compiler flags
CC_FLAGS= -g -O0 -std=c99 
CC_FLAGS+= -D CL_TARGET_OPENCL_VERSION=$(CL_VER) 
CC_FLAGS+= -D DEVICE=$(DEVICE)

# Libs to link
LIBS= -lm -fopenmp -lOpenCL 
LIBS+= -L"./libs/argp/lib" -largp

# Extra include for main
INCLUDE= -I"./libs/argp/include" -I"./include"

################################################################

# ohjects going into main
OBJECTS_MAIN= main_matmul.o
OBJECTS_MAIN+= matmul_pre.o
OBJECTS_MAIN+= matmul_omp.o
OBJECTS_MAIN+= matmul_ocl.o
OBJECTS_MAIN+= matmul_utils.o

# !!ALL!!
all: main_matmul

# !!MAIN!!
main_matmul: $(OBJECTS_MAIN)
	$(CC) $^ $(CC_FLAGS) $(LIBS) -o $@

# Objects
main_matmul.o: ./src/main_matmul.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_ocl.o: ./src/matmul_ocl.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_omp.o: ./src/matmul_omp.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_pre.o: ./src/matmul_pre.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_utils.o: ./src/matmul_utils.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@

# ##CLEAN##
clean:
	rm -f ./main_matmul
	rm -f ./main_matmul.o
	rm -f ./matmul_ocl.o
	rm -f ./matmul_omp.o
	rm -f ./matmul_pre.o
	rm -f ./matmul_utils.o

################################################################

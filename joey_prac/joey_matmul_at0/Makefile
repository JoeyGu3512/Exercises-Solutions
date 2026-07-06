
################################################################
################################################################
################################################################

# 
# Makefile for an OpenCL matmul practice
# JoeyGu3412QwQ
# 2026June27 15pm
# Msys2 MinGW64 - Win11
#

################################################################
################################################################
################################################################

# SET CONSTANTS

# Set C compiler
ifndef CC
	CC=gcc
endif

# Set archive
AR=ar

# Set CL version
ifndef CL_VERSION
	CL_VERSION=300
endif

# Set Default Device Const
ifndef OCL_DEFAULT_DEVICE
	OCL_DEFAULT_DEVICE=CL_DEVICE_TYPE_DEFAULT
endif

################################################################
################################################################
################################################################

# COMPILER FLAGS

# C compiler flags - O0
CC_FLAGS= -g -O1 -std=c99 -Wall
CC_FLAGS+= -D OCL_TARGET_OPENCL_VERSION=$(CL_VERSION) 
CC_FLAGS+= -D OCL_DEFAULT_DEVICE=$(OCL_DEFAULT_DEVICE)

# C compiler flags - O3
CC_FLAGS_O3= -g -O3 -std=c99 -Wall
CC_FLAGS_O3+= -D OCL_TARGET_OPENCL_VERSION=$(CL_VERSION) 
CC_FLAGS_O3+= -D OCL_DEFAULT_DEVICE=$(OCL_DEFAULT_DEVICE)

################################################################
################################################################
################################################################

# ALL

# !!ALL!!
all: main_matmul

################################################################
################################################################
################################################################
####### [[ MAIN ]] The executable for testing matmuls ##########
################################################################

# Libs to link for main
LIBS_MAIN= -lm -fopenmp -lOpenCL -lopenblas -lgsl
LIBS_MAIN+= -L"./libs/argp/lib" -largp

# Extra include for main
INCLUDE= -I"./libs/argp/include" -I"./include"

################################################################

# ohjects going into main
OBJECTS_MAIN= main_matmul.o
OBJECTS_MAIN+= matmul_test_omp.o
OBJECTS_MAIN+= matmul_test_ocl.o
OBJECTS_MAIN+= matmul_utils.o

# link project libs
LIBS_MAIN += -L"./"
LIBS_MAIN += -l"matmul_omp"
LIBS_MAIN += -l"matmul_ocl"
LIBS_MAIN += -l"matmul_pre"
# project lib files
PROJ_LIBS = libmatmul_omp.dll 
PROJ_LIBS += libmatmul_ocl.dll 
PROJ_LIBS += libmatmul_pre.dll

################################################################

# !!MAIN!!
main_matmul: $(OBJECTS_MAIN) $(PROJ_LIBS)
	$(CC) $^ $(CC_FLAGS) $(LIBS_MAIN) -o $@
# Objects for main
main_matmul.o: ./src/main_matmul.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_test_ocl.o: ./src/matmul_test_ocl.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_test_omp.o: ./src/matmul_test_omp.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_utils.o: ./src/matmul_utils.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@

################################################################
################################################################
################################################################

# Objects going into pre lib
OBJECTS_PRE= matmul_pre_lib.o
OBJECTS_PRE+= matmul_pre_kern0.o
OBJECTS_PRE+= matmul_pre_kern1.o

# Libs OMP
LIBS_PRE= -lm -fopenmp -lopenblas -lgsl

################################################################

# PRE MATMUL LIB
libmatmul_pre.dll: $(OBJECTS_PRE)
	$(CC) -shared $^ $(CC_FLAGS) $(LIBS_PRE) -o $@
# Objects
matmul_pre_lib.o: ./src/matmul_pre/matmul_pre_lib.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_pre_kern0.o: ./src/matmul_pre/matmul_pre_kern0.c
	$(CC) -c $^ $(CC_FLAGS_O3) $(INCLUDE) -o $@
matmul_pre_kern1.o: ./src/matmul_pre/matmul_pre_kern1.c
	$(CC) -c $^ $(CC_FLAGS_O3) $(INCLUDE) -o $@

################################################################
################################################################
################################################################

# Objects going into omp lib
OBJECTS_OMP= matmul_omp_lib.o
OBJECTS_OMP+= matmul_omp_kern0.o

# Libs OMP
LIBS_OMP= -lm -fopenmp

################################################################

# OMP MATMUL LIB
libmatmul_omp.dll: $(OBJECTS_OMP)
	$(CC) -shared $^ $(CC_FLAGS) $(LIBS_OMP) -o $@
# Objects
matmul_omp_lib.o: ./src/matmul_omp/matmul_omp_lib.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_omp_kern0.o: ./src/matmul_omp/matmul_omp_kern0.c
	$(CC) -c $^ $(CC_FLAGS_O3) $(INCLUDE) -o $@

################################################################
################################################################
################################################################

# Objects going into omp lib
OBJECTS_OCL= matmul_ocl_lib.o
OBJECTS_OCL+= matmul_ocl_kern0.o

# Libs OCL
LIBS_OCL= -lOpenCL

################################################################

# OMP MATMUL LIB
libmatmul_ocl.dll: $(OBJECTS_OCL)
	$(CC) -shared $^ $(CC_FLAGS) $(LIBS_OCL) -o $@
# Objects
matmul_ocl_lib.o: ./src/matmul_ocl/matmul_ocl_lib.c
	$(CC) -c $^ $(CC_FLAGS) $(INCLUDE) -o $@
matmul_ocl_kern0.o: ./src/matmul_ocl/matmul_ocl_kern0.c
	$(CC) -c $^ $(CC_FLAGS_O3) $(INCLUDE) -o $@

################################################################
################################################################
################################################################

# ##CLEAN##
clean:
	# Clean thingies from main...
	rm -f ./main_matmul
	rm -f ./main_matmul.o
	rm -f ./matmul_test_ocl.o
	rm -f ./matmul_test_omp.o
	rm -f ./matmul_utils.o
	rm -f ./matmul_pre.o
	rm -f ./matmul_pre_kern0.o
	rm -f ./matmul_pre_kern1.o
	# Clean thingies from pre...
	rm -f ./libmatmul_pre.dll
	rm -f ./matmul_pre_lib.o
	rm -f ./matmul_pre_kern0.o
	rm -f ./matmul_pre_kern1.o
	# Clean thingies from main...
	rm -f ./libmatmul_omp.dll
	rm -f ./matmul_omp_lib.o
	rm -f ./matmul_omp_kern0.o
	# Clean thingies from main...
	rm -f ./libmatmul_ocl.dll
	rm -f ./matmul_ocl_lib.o
	rm -f ./matmul_ocl_kern0.o

################################################################
################################################################
################################################################

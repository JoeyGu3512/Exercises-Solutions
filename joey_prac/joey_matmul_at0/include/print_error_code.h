#ifndef PRINT_ERROR_CODE
#define TIMING_H
/*
DeepSeek生的简单返回码打印宏
谢谢DeepSeek！XwX
*/

/**************************************************************/

#include <stdio.h>
#include <time.h>


#define PRINT_ERROR_CODE(func_name, ret) \
    fprintf(stderr, "[ERROR] Function %s failed returning code %d. \n", (func_name), (ret))

/**************************************************************/

#endif
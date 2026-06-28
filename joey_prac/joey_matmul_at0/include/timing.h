/*
DeepSeek做的计时器宏（给都给了就用吧QwQ）
谢谢DeepSeek！XwX
*/

/**************************************************************/

#include <stdio.h>
#include <time.h>

#define TIMEIT(code) do {                                      \
    clock_t _start = clock();                                  \
    code;                                                      \
    clock_t _end = clock();                                    \
    printf(                                                    \
        "[Time] %.6f s\n",                                     \
        (double)(_end - _start) / CLOCKS_PER_SEC               \
    );                                                         \
} while(0)

/**************************************************************/

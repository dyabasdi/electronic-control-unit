/* Main header file for all of the processes*/
#include <stdio.h>
#include <stdint.h>
#include <string.h> // For memcpy
#include <stdbool.h>

typedef struct {
    uint32_t timer;
} Global;

// read-only global variables
extern Global global;
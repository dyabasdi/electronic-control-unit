#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <process.h>
#include <main.h>



// Create local struct just for this program
Signals s_signals;

void processData100hz(){
    // init
    const bool startupTimer = (global.timer > 5) ? 1 : 0;
}
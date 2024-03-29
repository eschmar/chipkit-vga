#include <pic32mx.h>
#include "helpers.h"

#define SCREEN_H          480
#define SCREEN_SCALED_H   60
#define SCREEN_W          640
#define SCREEN_SCALED_W   80

#define FILE_IMAGE  "image.txt"

#define IN_DRAW            0
#define IN_VSYNC_PULSE     1
#define IN_VSYNC_FP        2
#define IN_VSYNC_BP        3
#define IN_HSYNC_PULSE     5
#define IN_HSYNC_FP        6
#define IN_HSYNC_BP        7

#define nop __asm__("nop \n")

/* Define VGA Constants */
#define VSYNC_FP_LINE     10
#define VSYNC_SP_LINE      2
#define VSYNC_BP_LINE     33
#define VSYNC_VA_LINE    480
#define VSYNC_TL_LINE    525

#define HSYNC_FP_PXL      16
#define HSYNC_SP_PXL      96
#define HSYNC_BP_PXL      48
#define HSYNC_VA_PXL     640
#define HSYNC_TL_PXL     800

#define SCALING 8

#define PIN_VSYNC        0x8
#define PIN_HSYNC       0x10
#define PIN_VGA         0x40


/* Define VGA vars */
int  lineCounter;
char showRow;
int displayState;
int previousState;
char screen[SCREEN_SCALED_H][SCREEN_SCALED_W];

// VSync counters
short vsyncFP = VSYNC_FP_LINE;
short vsyncBP = VSYNC_BP_LINE;
short vsyncSP = VSYNC_SP_LINE;
short vsyncVA = VSYNC_VA_LINE;

// HSync counters
short hsyncFP = HSYNC_FP_PXL / SCALING;
short hsyncBP = HSYNC_BP_PXL / SCALING;
short hsyncSP = HSYNC_SP_PXL / SCALING;

// Current position in active area
short x = HSYNC_TL_PXL / SCALING;
short y = VSYNC_TL_LINE;

// Length 88 scaled pixels
short vsyncPorch = (HSYNC_BP_PXL + HSYNC_FP_PXL + HSYNC_VA_PXL) / SCALING;


void generateArt() {
    int i = 15;
    int j = 20;
    int z = 1;
    while (i < SCREEN_SCALED_H && j < SCREEN_SCALED_W) {
        screen[i][j] = 1;
        i += z;
        screen[i][j] = 1;
        j += z;
        z++;
    }
}

int main() {
    // initialise LEDs as outputs
    TRISECLR = 0xff;
    PORTECLR = 0xff;
    
    // setup timers
    enableTimer2(1280, 0x1B, 0x001, 1);
    enableTimer3(160, 0x1B, 0x001, 1);

    // enable interrupts
    enableMultiVectorMode();
    enable_interrupt();
   
	return 0;
}

int ysyncCounter = 0;
/**
 * ISR Interrupt handler for timer 2
 */
void timer2_interrupt_handler(void) {
   IFSCLR(0) = 0x100;
   ysyncCounter = (ysyncCounter + 1) % 525;

    if (ysyncCounter == 0) {
        PORTESET = PIN_VSYNC;
        //ysyncCounter--;
    }
    if (ysyncCounter == 2) {
        PORTECLR = PIN_VSYNC;
    }
}


int hsyncCounter = 0;
/**
 * ISR Interrupt handler for timer 3
 */
void timer3_interrupt_handler(void) {
   IFSCLR(0) = 0x1000;
   hsyncCounter = (hsyncCounter + 1) % 8;

    if (hsyncCounter == 0) {
        PORTESET = PIN_HSYNC;
        //hsyncCounter--;
    }
    if (hsyncCounter == 1) {
        PORTECLR = PIN_HSYNC;
    }
}

/**
 * Resets all the counters 
 */
void resetCounters(void) {
    vsyncFP = VSYNC_FP_LINE;
    vsyncBP = VSYNC_BP_LINE;
    vsyncSP = VSYNC_SP_LINE;

    hsyncFP = HSYNC_FP_PXL / SCALING;
    hsyncBP = HSYNC_BP_PXL / SCALING;
    hsyncSP = HSYNC_SP_PXL / SCALING;

    x = HSYNC_VA_PXL / SCALING;
    y = VSYNC_VA_LINE;   
}

/**
 * Triggers vertical and horizontal sync pulses
 */
void handleSyncPulses() {
    // HSYNC
    if (x == hsyncSP) {
        PORTESET = PIN_HSYNC;
    }else if (x == (HSYNC_TL_PXL / SCALING)) {
        PORTECLR = PIN_HSYNC;
    }
    
    // VSYNC
    if (y == vsyncSP && x == (HSYNC_TL_PXL / SCALING)) {
        PORTESET = PIN_VSYNC;
    }else if (y == 1 && x == 1) {
        PORTECLR = PIN_VSYNC;
    }
}

/**
 * Move to the next position
 */
void advance() {
    x--;
    
    if (x == 0 && y > 1) {
        x = HSYNC_TL_PXL / SCALING;
        y--;
    }else if (x == 0 && y == 1) {
        x = HSYNC_TL_PXL / SCALING;
        y = VSYNC_TL_LINE;
    }
}

/**
 * Keeps track of current and previous states
 */
void updateState(int nextState) {
    previousState = displayState;
    displayState = nextState;
}

/**
 * ISR general interrupt handler
 */
void core_interrupt_handler(void) {
    // clear interrupt flag
    // IFSCLR(0) = 0x100;

    // pulse!
    //handleSyncPulses();

    // do something
    advance();
}


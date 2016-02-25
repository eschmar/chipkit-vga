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

#define PIN_VSYNC        0x4
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
short x = HSYNC_VA_PXL / SCALING;
short y = VSYNC_VA_LINE;

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
    enableTimer2(25, 0x1B, 0x0, 1);
    
    // enable interrupts
    enable_interrupt();
   
	return 0;
}

/**
 * ISR Interrupt handler for timer 2
 */
void timer2_interrupt_handler(void) {
   
}

/**
 * ISR Interrupt handler for timer 3
 */
void timer3_interrupt_handler(void) {
   
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
    switch(displayState) {
        
        // VSync front porch: 88 iterations for 10 lines
        case IN_VSYNC_FP:
            vsyncPorch--;
            if (vsyncPorch == 0) {
                updateState(IN_HSYNC_PULSE);
                vsyncPorch = (HSYNC_BP_PXL + HSYNC_FP_PXL + HSYNC_VA_PXL) / SCALING;
            }
        break;
        
        // HSync front porch: 2 iterations for 480 lines
        case IN_HSYNC_FP:
            hsyncFP--;
            if (hsyncFP == 0) {
                updateState(IN_DRAW);
                hsyncFP = HSYNC_FP_PXL / SCALING;  
            }
        break;
        
        // Skip the drawing activity filling with empty iterations
        case IN_DRAW:
            x--;
            if (x == 0) {
                updateState(IN_HSYNC_BP);
                x = HSYNC_VA_PXL / SCALING;
            }
        break;
        
         // HSync back porch: 6 iterations for 480 lines
        case IN_HSYNC_BP:
            hsyncBP--;
            if (hsyncBP == 0) {
                updateState(IN_HSYNC_PULSE);
                hsyncBP = HSYNC_BP_PXL / SCALING;  
            }
        break;
        
        // VSync back porch: 88 iterations for 33 lines 
        case IN_VSYNC_BP:
            vsyncPorch--;
            if (vsyncPorch == 0) {
                updateState(IN_HSYNC_PULSE);
                vsyncPorch = (HSYNC_BP_PXL + HSYNC_FP_PXL + HSYNC_VA_PXL) / SCALING;
            }
        break;
        
        // VSync pulse: 88 iterations for 2 lines. VSync pin HIGH
        case IN_VSYNC_PULSE:
            PORTESET = PIN_VSYNC;
            vsyncPorch--;
            if (vsyncPorch == 0) {
                updateState(IN_HSYNC_PULSE);
                vsyncPorch = (HSYNC_BP_PXL + HSYNC_FP_PXL + HSYNC_VA_PXL) / SCALING;
            }   
        break;
        
        // HSync pulse: 12 iterations for a variable number of lines depending
        // on the previous state. HSync pin HIGH
        case IN_HSYNC_PULSE:
            PORTESET = PIN_HSYNC;
        
            hsyncSP--;
            if (hsyncSP == 0) {
                if (previousState == IN_VSYNC_FP) {
                    vsyncFP--;
                    if (vsyncFP == 0) {
                        updateState(IN_VSYNC_FP);
                    } else {
                        updateState(IN_VSYNC_FP);
                        vsyncFP = VSYNC_FP_LINE;
                    }
                } else if (previousState == IN_HSYNC_BP) {
                    vsyncVA--;
                    if (vsyncVA == 0) {
                        updateState(IN_VSYNC_BP);
                    } else {
                        updateState(IN_HSYNC_FP);
                        vsyncVA = VSYNC_VA_LINE;
                    }
                } else if (previousState == IN_VSYNC_BP) {
                    vsyncBP--;
                    if (vsyncBP == 0) {
                        updateState(IN_VSYNC_PULSE);
                    } else {
                        updateState(IN_VSYNC_BP);
                        vsyncBP = VSYNC_BP_LINE;
                    }
                } else if (previousState == IN_VSYNC_PULSE) {
                    vsyncSP--;
                    if (vsyncSP == 0) {
                        resetCounters();
                        updateState(IN_VSYNC_FP);
                    } else {
                        updateState(IN_VSYNC_PULSE);
                        vsyncSP = VSYNC_SP_LINE;
                    }
                }
                hsyncSP = HSYNC_SP_PXL / SCALING;
                PORTECLR = PIN_HSYNC;
            }
        break;
    }
}
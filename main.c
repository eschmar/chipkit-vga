#include <pic32mx.h>
#include "helpers.h"

#define SCREEN_H          480
#define SCREEN_SCALED_H   30
#define SCREEN_W          640
#define SCREEN_SCALED_W   40

#define FILE_IMAGE  "image.txt"

#define IN_DRAW            0
#define IN_VSYNC_PULSE     1
#define IN_VSYNC_FP        2

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

/* Define VGA vars */
int  lineCounter;
char showRow;
char displayState;
char screen[SCREEN_SCALED_H][SCREEN_SCALED_W];

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
    enableTimer2(31250, 0x1B, 0x7, 1);
    enableTimer3(51250, 0x1B, 0x7, 1);
    
    // enable interrupts
    enableMultiVectorMode();
    enable_interrupt();
   
	return 0;
}

/**
 * ISR Interrupt handler for timer 2
 */
void timer2_interrupt_handler(void) {
    int next = ((PORTE & 0xf) + 1) % 0xf;
    PORTECLR = ~next & 0xf;
    PORTESET = next & 0xf;
    IFSCLR(0) = 0x100;
}

/**
 * ISR Interrupt handler for timer 3
 */
void timer3_interrupt_handler(void) {
    int next = ((((PORTE & 0xf0) >> 4) + 1) % 0xf) << 4;
    PORTECLR = ~next & 0xf0;
    PORTESET = next & 0xf0;
    IFSCLR(0) = 0x1000;
}

/**
 * ISR general interrupt handler
 */
void core_interrupt_handler(void) {
    // code
}
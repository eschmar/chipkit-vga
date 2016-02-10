#include <pic32mx.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_H          480
#define SCREEN_SCALED_H   30
#define SCREEN_W          640
#define SCREEN_SCALED_W   40

#define IMAGE_FILE = "image.txt";

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

void parseImageFile() {
    // todo.
    
    // placeholder:
    int i,j;
    for (i = 0; i < SCREEN_SCALED_H; i++) {
        for (j = 0; j < SCREEN_SCALED_W; j++) {
            screen[i][j] = (i+j) % 2;
        }
    }
    
    return;
}

int main() {
    // main logic
    parseImageFile();
	return 0;
}

void timer2_interrupt_handler(void) {
    
}
#include <pic32mx.h>

int main() {
    // initialise LEDs as outputs
    TRISECLR = 0xff;
    PORTECLR = 0xff;
    
    // turn on LED1
    PORTESET = 0x1;
    
    // init timer
    T2CON = 0x0;        // stop timer
    TMR2 = 0;           // clear timer
    PR2 = 31250;        // set period to (80kk/256/10)
    IPCSET(2) = 0x1F;   // set priority 7/3
    
    IFSCLR(0) = 0x100;  // reset timer interrupt status flag
    IECSET(0) = 0x100;  // enable timer interrupts

    // set prescaling to 1:256 and enable timer (bits 15,6-4)
    T2CONSET = 0x0070;
    
    // start timer
    T2CONSET = 0x8000;
    enable_interrupt();
   
	return 0;
}


void timer2_interrupt_handler(void) {
    int next = (PORTE + 1) % 0xff;
    PORTECLR = ~next;
    PORTESET = next;
    IFSCLR(0) = 0x100;
}
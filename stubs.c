#include <pic32mx.h>

/* Non-Maskable Interrupt; something bad likely happened, so hang */
void _nmi_handler() {
	for(;;);
}

/* This function is called upon reset, before .data and .bss is set up */
void _on_reset() {
	
}

/* This function is called before main() is called, you can do setup here */
void _on_bootstrap() {
    // initialise ports
    AD1PCFG = 0xffff;   // set all pins to digital
    ODCE = 0x0;         // normal digital output (not open-drain config)
    PORTECLR = 0x1f;    // clear bits 4:0
    TRISECLR = 0x1f;    // set bits 4:0 as outputs
    
    // initialise timer
    T2CON = 0x0;        // stop timer
    TMR2 = 0;           // clear timer
    PR2 = 160;          // set period to (80kk/16)
    IPCSET(2) = 0x1F;   // set priority 7/3
    
    IFSCLR(0) = 0x100;  // reset timer interrupt status flag
    IECSET(0) = 0x100;  // enable timer interrupts
    T2CONSET = 0x0040;  // prescaling 1:16 (bits 6:4)
    
    // start timer
    T2CONSET = 0x8000;
    enable_interrupt();
}

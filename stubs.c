/* Non-Maskable Interrupt; something bad likely happened, so hang */
void _nmi_handler() {
	for(;;);
}

/* This function is called upon reset, before .data and .bss is set up */
void _on_reset() {
	
}

/* This function is called before main() is called, you can do setup here */
void _on_bootstrap() {
    /* Ensure no interrupts during the setup (to be safe) */
    /**--> Turn off interrupts TO FIX!

  // Use PORTE for all signals:
   /*                      *
   * Pin | signal | output *
   * --------------------- *
   *  26 |  RE0   | Vsync  *
   *  27 |  RE1   | Hsync  *
   *  28 |  RE2   | R      *
   *  29 |  RE3   | G      *
   *  30 |  RE4   | B      *
   *                      */
  AD1PCFG = 0xFFFF; // Set all pins as digital
  ODCE = 0x0; // Normal digital output (not Open-Drain config)
  PORTECLR = 0x1F; // Clear bits 4-0
  TRISECLR = 0x1F; // Set bits 4-0 as outputs
  
  // Use Timer2 for Hsync pulses and VGA timing
  T2CON = 0x0;          // Stop clock until initialisation is done (first bit to 0)
  T2CONSET = ????;      // Apply prescale
  TMR2 = 0x0;           // Timer set to 0
  PR2 = ????;           // Load period register
  // Interrupt flag TO FIX!
  
  // Setup VGA variables
  lineCounter = 0;
  showRow = 0;
  displayState = IN_VSYNC_PULSE;

  memset(screen, 0, sizeof(screen)); // Clear array of screen pixels

  /**--> Turn on interrupts TO FIX!

  return;
}

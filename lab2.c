#include <msp430fr5739.h>

/*
 * main.c
 */

// Type definitions
typedef unsigned char		uc;

// User definitions
#define SW_DEB_CONST		1600			// Load into TA1CCR0 the value SW_DEB_CONST i.e. debounce time = 1600*1/8000 = 200ms
#define SW_HALF_DEB_CONST	SW_DEB_CONST/2	// Shorter debounce time model assuming half


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    // Enable on-board switches
    	// P4.0 and P4.1 are interfaced to switches
    	// Note: S1 when pressed makes P4.0 logic LOW (See schematic of MSP-EXP430FR5739)
    	// Note: S2 when pressed makes P4.1 logic LOW (See schematic of MSP-EXP430FR5739)
    	// Port 4 has only two pins
    	P4OUT |= (BIT0 | BIT1);					// Configure pullup resistor
    	P4DIR &= ~(BIT0 | BIT1);				// Direction = input
    	P4REN |= BIT0 | BIT1;					// Enable pullup resistor
    	P4IES &= ~(BIT0 | BIT1);				// P4.0 and P4.1 Lo/Hi edge interrupts
    	P4IE = (BIT0 | BIT1);						// P4.0 and P4.1 interrupts enabled
    	P4IFG = 0;								// P4 IFG cleared

    	PJOUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);			// Initialise PJ.1, PJ.2 and PJ.3 to OFF
    	PJDIR |= (BIT0 | BIT1 | BIT2 | BIT3);		// Set PJ.0, PJ.1, PJ.2 and PJ.3 as outputs


    	P3DIR &= ~(BIT0 | BIT1| BIT2);
    	P1DIR &= ~BIT4;
    	P2OUT |= BIT7;
    	_BIS_SR(CPUOFF + GIE);
	return 0;
}

// Disable switches interfaced to P4.0 and P4.1
inline void DisableSwitches(void) {
	P4IFG = 0;								// P4 IFG cleared
	P4IE &= ~(BIT0 + BIT1);					// P4.0 and P4.1 interrupts disabled
	P4IFG = 0;								// P4 IFG cleared
}

// Enable switches interfaced to P4.0 and P4.1
inline void EnableSwitches(void) {
	P4IFG = 0;								// P4 IFG cleared
	P4IE = BIT0 + BIT1;						// P4.0 and P4.1 interrupts enabled
}

// Simple software switch debounce model as a constant delay
inline void StartDebounceTimer(uc ucDelay) {
	// Default delay = 0
	TA1CCTL0 = CCIE;						// TACCR0 interrupt enabled
	if(ucDelay) {
		TA1CCR0 = SW_HALF_DEB_CONST;
	} else {
		TA1CCR0 = SW_DEB_CONST;
	}
	TA1CTL = TASSEL_1 + MC_1;				// ACLK, up mode
}

// Switch presses causes this debounce ISR to fire
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void) {
	TA1CCTL0 = 0;
	TA1CTL = 0;
	EnableSwitches();
}

// Port 4 ISR to detect switch presses
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void) {
	// See link to get information on __even_in_range intrinsic
	// https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/238317
	switch(__even_in_range(P4IV, P4IV_P4IFG1)) {
		case P4IV_P4IFG0:
			DisableSwitches();
			P4IFG &= ~BIT0;					// Clear P4.0 IFG
			PJOUT ^= BIT2;					// Toggle PJ.2
			StartDebounceTimer(0);			// Reenable switches after debounce i.e. time elapsed
			break;
		case P4IV_P4IFG1:
			DisableSwitches();
			P4IFG &= ~BIT1;					// Clear P4.1 IFG
			PJOUT ^= BIT3;					// Toggle PJ.3
			StartDebounceTimer(0);			// Reenable switches after debounce i.e. time elapsed
			break;
	    default:
	    	break;
	}
}

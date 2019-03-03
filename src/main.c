/*
 * Ikea Ansluta Remote Controller
 *
 * Copyright (C) 2019 Michal Krombholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ansluta.h"
#include <stdint.h>

/**
 * Main routine
 */
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT 0x5a80
	BCSCTL1 = CALBC1_1MHZ;	  // original MOV.B   #0xcc,  &0x0057  ; BCSCTL1
	DCOCTL = CALDCO_1MHZ;	  // original MOV.B   #0x68,  &0x0056  ; DCOCTL
	BCSCTL3 |= 0x20;		  // XT2S=0 LFXT1Sx=VLOCLK

	// Port 1 setup
	P1DIR = P_LED | P_CS | P_SCLK | P_MOSI; // P1DIR 0x65=01100101 --> 6,5,2,0 out (P1.0 led; P1.2=CS; P1.5=SCLK, P1.6=SI)
	P1SEL = 0;								// select special io functions --> normal io

	// Port 2 setup
	P2DIR = 0;	  // P2 is all in
	P2REN = 0xff; // resistors enable - all
	P2OUT = 0xff; // pull up - all
	P2SEL = 0;	  // select special io functions --> normal io

	// enable interrupt for key switch
	P2IFG = 0;	   // clear flags register
	P2IE = P_KEY;  // enable
	P2IES = P_KEY; // edge

	// setup timer A0
	CCTL0 = CCIE;					// CCR0 interrupt enabled
	TACTL = TASSEL_2 + ID_3 + MC_1; // TASSEL_2 --> SMCLK (~1MHz), MC_1--> up, ID_3 --> SMCLK/8
	CCR0 = 50000;					// (SMCLK/8)/50000 = 2.5 Hz

	// Enter Low Power Mode = LPM0 w/ interrupt
	_BIS_SR(CPUOFF + GIE);

	// never here - just hang, no exit
	while (1)
		;
}

// Timer A0 interrupt service routine
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
	P1OUT ^= P_LED; // toggle LED for now
}

// Port 1 interrupt service routine
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
	// TODO - there is no debouncing of the switch
	if( P2IFG & P_KEY )
	{
		CCR0 = CCR0 > 10000 ? 10000 : 50000; // toggle between two blink speeds
	}
	P2IFG = 0; // clear all flags
}
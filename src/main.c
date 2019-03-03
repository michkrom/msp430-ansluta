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
    WDTCTL  = WDTPW + WDTHOLD; 	// Stop WDT 0x5a80
	BCSCTL1 = CALBC1_1MHZ;      // original MOV.B   #0xcc,  &0x0057  ; BCSCTL1
  	DCOCTL  = CALDCO_1MHZ;      // original MOV.B   #0x68,  &0x0056  ; DCOCTL
  	BCSCTL3 |= 0x20;            // XT2S=0 LFXT1Sx=VLOCLK
  	

	P1DIR = P_LED|P_CS|P_SCLK|P_MOSI; // P1DIR 0x65=01100101 --> 6,5,2,0 out (P1.0 led; P1.2=CS; P1.5=SCLK, P1.6=SI)
	P1SEL = 0; // select special io functions - normal io

	P2DIR = 0;    // P2 is all in
	P2REN = 0xff; // resistors enable - all
	P2OUT = 0xff; // pull up - all
	P2SEL = 0;    // select special io functions - normal io
	
	// enable interrupt for key switch
	P2IFG = 0; // clear flags
	P2IE = P_KEY; // enable
	P2IES = P_KEY; // edge

	while(1)
	{
		LED(1);
		DELAYLOOP(0xFFFF)
		LED(0);
		DELAYLOOP(0xFFFF)
	}
 
	// hang, not exit
    while(1);
}

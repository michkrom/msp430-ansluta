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

#include <msp430.h>
#include <stdint.h>

/*

The Ansluta remote consists of: MPS430G2231 MPU, CC2500 Transiver, LED, Switch

MSP430       To
----------------------------------------
     P1.0    LED
RXD  P1.1    unused?
TXD  P1.2    CC2500 CS
     P1.3    CC2500 GDO0
     P1.4    CC2500 GDO2
SCLK P1.5    CC2500 SCLK
SOMI P1.6    CC2500 SI
SIMO P1.7    CC2500 SO
     P2.6    unused? (N-FET Gate in transformer)
     P2.7    Key (switch)

note - since the P1.6 and P1.7 are swapped (hw desin error likely) we cannot use build-in SPI engine (sic!)
* 
**/

// P1 --> 0x20..0x27
// P2 --> 0x28..0x2f

// P1
#define P_LED  BIT0
#define P_CS   BIT2
#define P_GDO0 BIT3
#define P_GDO2 BIT4
#define P_SCLK BIT5
#define P_MOSI BIT6
#define P_MISO BIT7
// P2
#define P_KEY  BIT7

#define LED(v) P1OUT = v ? (P1OUT | P_LED) : (P1OUT & ~P_LED);
#define CS(v) P1OUT = v ? (P1OUT | P_CS) : (P1OUT & ~P_CS);
#define SCLK(v) P1OUT = v ? (P1OUT | P_SCLK) : (P1OUT & ~P_SCLK);
#define MOSI(v) P1OUT = v ? (P1OUT | P_MOSI) : (P1OUT & ~P_MOSI);
#define KEY() (P2IN & P_KEY)
#define MISO() (P1IN & P_MISO)

#define DELAYLOOP(count) for(uint16_t cnt = 0; cnt < count; ++cnt) __asm__("");

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

#include <msp430.h>
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	P3DIR = 0x30;
	P3OUT = 0x20;

	P8SEL |= BIT2+BIT3;// UCA1TXD(P8.2) UCA1RXD(P8.1) 1100

	UCA1CTL1 |= UCSWRST;// **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_1;// CLK = ACLK
	UCA1BR0 = 0x03;// 2400 (see User's Guide)
	UCA1BR1 = 0x00;
	UCA1MCTL |= UCBRS_3;// Modulation UCBRSx=5, UCBRFx=0
	UCA1CTL1 &= ~UCSWRST;// Initialize USCI state machine
	while(1)
	{
		while (!(UCA1IFG&UCRXIFG));
		{
		UCA1TXBUF = UCA1RXBUF;
		while(!(UCA1IFG&UCTXIFG));
		}
	}
}

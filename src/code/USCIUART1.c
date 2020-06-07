#include <msp430.h>
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;// Stop watchdog timer
	P3DIR = 0x30;
	P3OUT = 0x20;
	P4DIR = 0x30;
	P4OUT = 0x10;
	P8SEL |= BIT2+BIT3;// UCA1TXD(P8.2) UCA1RXD(P8.3) 1100

	UCA1CTL1 |= UCSWRST;// **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_1;// BRCLK = SMCLK
	UCA1BR0 = 3;// 1048576/9600=109.23;109.23/16=6.83
	UCA1BR1 = 0;//
	UCA1MCTL = UCBRS_3+UCBRF_0;//UCBRFx=0.83*16=13.28
	UCA1CTL1 &= ~UCSWRST;// **Initialize USCI state machine**
	UCA1IE |= UCRXIE;// Enable USCI_A0 RX interrupt
	LPM3;
	_EINT();
	_NOP();
}
// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch(__even_in_range(UCA1IV,4))
	{
		case 0: break;// Vector 0 - no interrupt
		case 2:
			if(UCA1RXBUF==0xAA)// Vector 2 - RXIFG
			{
			while (!(UCA1IFG&UCTXIFG));
			UCA1TXBUF = 0x55;// TX -> RXed character
		}
			break;
		case 4: break;// Vector 4 - TXIFG	default: break;
	}
}

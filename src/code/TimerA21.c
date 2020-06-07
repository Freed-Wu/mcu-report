﻿#include <msp430.h>
void main(void)
{
	WDTCTL = WDTPW+WDTHOLD;
	P1DIR |= BIT6 + BIT7;
	P1SEL |= BIT6 + BIT7;
	TA0CCR0 = 512-1;
	TA0CCTL1 = OUTMOD_7;
	TA0CCR1 = 128;
	TA0CCTL2 = OUTMOD_7;
	TA0CCR2 = 384;
	TA0CTL = TASSEL_2 + MC_3 + TACLR;
	LPM0;
	_NOP();
}

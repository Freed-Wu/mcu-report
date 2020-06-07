#include <msp430.h>
void main(void)
{
	WDTCTL = WDTPW+WDTHOLD;
	P4DIR |= BIT1;
	TA0CCR0 = 50000;
	TA0CTL = TASSEL__SMCLK + MC__UPDOWN +	TAIE + TACLR;
	LPM3;
	_EINT();
	_NOP();
}
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
	switch(__even_in_range(TA0IV,14))
	{
		case TA0IV_NONE:
			break;
		case TA0IV_TACCR1:
			break;
		case TA0IV_TACCR2:
			break;
		case TA0IV_TACCR3:
			break;
		case TA0IV_TACCR4:
			break;
		case TA0IV_5:
			break;
		case TA0IV_6:
			break;
		case TA0IV_TAIFG:
			P4OUT ^= BIT1;
			break;
		default:
			break;
	}
}

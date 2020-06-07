#include <msp430.h>
void main(void)
{
	WDTCTL = WDT_MDLY_0_064;
	SFRIE1|=WDTIE;
	P4DIR|=BIT1;
	P3DIR|=BIT4;
	P3SEL|=BIT4;
	while(BAKCTL&LOCKIO)
		BAKCTL&=~(LOCKIO);
	UCSCTL6&=~(XT1OFF);
	LPM0;
	_EINT();
	_NOP();
}
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
	P4OUT^=BIT1;
}

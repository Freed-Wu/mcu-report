#include <msp430.h>
void main(void)
{
	WDTCTL = WDTPW+WDTHOLD;
	
	while(BAKCTL & LOCKBAK)
		BAKCTL &= ~(LOCKBAK);
	UCSCTL6 &= ~(XT1OFF); // XT1 On
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	}while (SFRIFG1&OFIFG);

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

#include <msp430.h>

#define CPU_F ((double)32768*32)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	P1DIR |= BIT0; // ACLK set out to pin
	P1SEL |= BIT0;
	while(BAKCTL & LOCKBAK)
		BAKCTL &= ~(LOCKBAK);
	UCSCTL6 &= ~(XT1OFF); // XT1 On
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	}while (SFRIFG1&OFIFG);

	P4DIR |= BIT1+BIT2;
	P4OUT &= ~(BIT1+BIT2);
	P2DIR &= ~(BIT6+BIT7);
	P2IES|=BIT6+BIT7;
	P2IE|=BIT6+BIT7;

	_EINT();
	_NOP();
}
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
	switch(__even_in_range(P2IV,10))
	{
		case P2IV_P2IFG6:
			delay_ms(10);
			if ((P2IN & BIT6) == 0)
				P4OUT |= BIT1;
			break;
			delay_ms(10);
			if ((P2IN & BIT7) == 0)
				P4OUT &= ~BIT1;
			break;
	}
}

#include<msp430.h>
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	ADC12CTL0 = ADC12SHT02 + ADC12ON;
	ADC12CTL1 = ADC12CSTARTADD_8 + ADC12SHP + ADC12SSEL_0 + ADC12CONSEQ_0;
	ADC12MCTL0 = ADC12SREF_0;
	ADC12IE = 0x0100;// Enable interrupt
	ADC12CTL0 |= ADC12ENC;
	P6SEL |= 0x01;// P6.0 ADC option select
	P4DIR |=BIT1+BIT2;// P4.1~P4.3 output
	while (1)
	{
		ADC12CTL0 |= ADC12SC;// Start sampling/conversion
		LPM0;
		_EINT();
		_NOP();
	}
}
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
	switch(__even_in_range(ADC12IV,34))
	{
		case ADC12IV_ADC12IFG14:// Vector 6:ADC12IFG0
			if(ADC12MEM8 < 0x7ff)
			{
			P4OUT|=BIT1;
			P4OUT&=~BIT2;
		}

			else
			{
			P4OUT&=~BIT1;
			P4OUT|=BIT2;
		}
		LPM0_EXIT;
	}
}

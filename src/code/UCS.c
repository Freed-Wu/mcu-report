#include<msp430.h>
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	P1DIR |= BIT0; // ACLK set out to pins
	P1SEL |= BIT0;
	P3DIR |= BIT4; // SMCLK set out to pins
	P3SEL |= BIT4;
	while(BAKCTL & LOCKIO) // Unlock XT1 pins for operation, for 6638
		BAKCTL &= ~(LOCKIO);
	UCSCTL6 &= ~(XT1OFF); // XT1 On
	// Loop until XT1 fault flag is cleared
	do
	{
		UCSCTL7 &= ~XT1LFOFFG; // Clear XT1 fault flags
	}while (UCSCTL7&XT1LFOFFG); // Test XT1 fault flag
	_BIS_SR(SCG0); // Disable the FLL control loop
	UCSCTL0 = 0x0000; // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_4; // Set RSELx for DCO = 4.9 MHz
	UCSCTL2 = FLLD_1 + 243; // Set DCO Multiplier for 2.45MHz
	_BIC_SR(SCG0); // Enable the FLL control loop
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	}while (SFRIFG1&OFIFG); // Test oscillator fault flag
	_NOP();
}

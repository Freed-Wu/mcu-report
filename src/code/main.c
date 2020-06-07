#include <msp430.h>
#include <math.h>
#include "LCD.h"
#include "TM1638.h"

#define PI 3.1415926
#define CPU_F ((double)32768*32)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define keyC 12

void SPI_GPIO_Init(void);
void TM1638_Init(void);

uint8_t num[8];
uint8_t led_flag[8];
const uint8_t tab[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int sin_table[10];
int *sin_data_pr;
double i=0;
int j, k;
char flag=1;
int key_table[22]={261, 293, 329, 349, 391, 440, 493, 533, 587, 659, 698, 783, 880, 987, 1046, 1174, 1318, 1396, 1567, 1760, 1975};
int period_table[21];
int song_tone_table[2][32]={
	{7, 8, 9, 7, 7, 8, 9, 7, 9, 10, 11, 9, 10, 11, 11, 12, 11, 10, 9, 7, 11, 12, 11, 10, 9, 7, 7, 11, 7, 7, 11, 7}, 
	{4, 4, 5, 4, 7, 6, 4, 4, 5, 4, 8, 7, 4, 4, 11, 9, 7, 6, 10, 10, 9, 7, 8, 7}
};
int song_long_table[2][32]={
	{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 8, 2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 4, 4, 4, 4, 8, 4, 4, 8}, 
	{2, 2, 4, 4, 4, 8, 2, 2, 4, 4, 4, 8, 2, 2, 5, 4, 5, 8, 2, 2, 4, 4, 4, 8}
};
int index=0;
int m, n;
int kind=0;
int page=0;
int cursor=1;

void main(void)
{
	unsigned int key = 0;
	unsigned char count;

	WDTCTL = WDTPW + WDTHOLD;
	TM1638_Init();

	P1DIR |= BIT0;
	P1SEL |= BIT0;
	while(BAKCTL & LOCKBAK)
		BAKCTL &= ~(LOCKBAK);
	UCSCTL6 &= ~(XT1OFF);
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);// Clear XT2, XT1, DCO fault flags
		SFRIFG1 &= ~OFIFG;// Clear fault flags
	}while (SFRIFG1&OFIFG);

	P2DIR &= ~(BIT6+BIT7);
	P2IES|=BIT6+BIT7;
	P2IE|=BIT6+BIT7;

	SPI_GPIO_Init();
	P4DIR |= BIT1+BIT2+BIT3;// Four leds
	UCB1CTL1 |= UCSWRST;// **Put state machine in reset**
	UCB1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;// 3-pin, 8-bit SPI master
	// Clock polarity high, MSB
	UCB1CTL1 |= UCSSEL_2;// SMCLK
	UCB1BR0 = 0x02;// /2
	UCB1BR1 = 0;//
	//UCB1MCTL = 0;// No modulation
	UCB1CTL1 &= ~UCSWRST;// **Initialize USCI state machine**
	UCB1IE = UCRXIE ;// Enable USCI_A0 RX, TX interrupt

	P3DIR |= BIT6;//LCD Trigger
	for(j=0;j<10;j++)
	{
		i+=PI/5;
		sin_table[j]=(int)((sin(i)+1)*2048);
	}
	sin_data_pr=&sin_table[0];

	for(j=0;j<20;j++)
		period_table[j]=(int)(100000/key_table[j]);

	DAC12_0CTL0 = DAC12IR + DAC12SREF_1 + DAC12AMP_5 + DAC12ENC + DAC12CALON+DAC12OPS;

	P5DIR|=BIT1;
	P5OUT&=~BIT1;

	// CCR0 interrupt enabled
	TA1CCR0 = period_table[0];
	TA1CTL = TASSEL__SMCLK + MC__UP + TACLR ;
	_EINT();

	while(1)
	{
		LCD_Clear(CYAN);
		SPI_Delay(10);
		LCD_DrawRectangle(0, 0, 220, 20);
		LCD_Fill(10, 90, 230, 280, YELLOW);
		LCD_Fill(20, 100, 220, 270, CYAN);
		LCD_DrawLine(0, 220, 220, 220);
		switch(page)
		{
		case 0:
			LCD_ShowString(50, 19, "MSP430 Music Player");
			LCD_ShowString(20, 50, "1:Choose a song");
			LCD_ShowString(20, 70, "2:Play a song");
			LCD_ShowString(20, 90, "3:About me");
			LCD_ShowString(10, 240, "0~9:enter option");
			LCD_ShowString(10, 260, "A:up cursor");
			LCD_ShowString(10, 280, "B:down cursor");
			LCD_ShowString(10, 300, "C:return main menu");
			P4OUT |= BIT1|BIT2|BIT3;
			break;

		case 1:
			LCD_ShowString(70, 19, "Song Sheet");
			LCD_ShowString(20, 50, "1:Two tigers");
			LCD_ShowString(20, 70, "2:Happy birthday to you");
			LCD_ShowString(10, 240, "0~9:enter option");
			LCD_ShowString(10, 260, "A:up cursor");
			LCD_ShowString(10, 280, "B:down cursor");
			LCD_ShowString(10, 300, "C:return main menu");
			P4OUT =0;
			P4OUT |= BIT1;
			break;

		case 2:
			LCD_ShowString(70, 19, "Enjoy it");
			LCD_ShowString(10, 240, "1~7:make do, re...si");
			LCD_ShowString(10, 260, "8, 9, 0:make high do, re, me");
			LCD_ShowString(10, 300, "C:return main menu");
			P4OUT =0;
			P4OUT |= BIT2;
			break;

		case 3:
			LCD_ShowString(70, 19, "About me");
			LCD_ShowString(20, 50, "Author:Wu Zhenyu");
			LCD_ShowString(20, 70, "Apartment:NJUST");
			LCD_ShowString(20, 90, "Email:Wuzy01@qq.com");
			LCD_ShowString(20, 110, "Copyright for free");
			LCD_ShowString(10, 240, "0~9:enter option");
			LCD_ShowString(10, 300, "C:return main menu");
			P4OUT =0;
			P4OUT |= BIT3;
	}

		do
		{
			key=Read_key();
		}
		while(key==16);

		while(Read_key()==key);
		num[0] = tab[key];
		Write_DATA(0*2, num[3]);
		Write_DATA(1*2, num[2]);
		Write_DATA(2*2, num[1]);
		Write_DATA(3*2, num[0]);
		Write_DATA(4*2, num[7]);
		Write_DATA(5*2, num[6]);
		Write_DATA(6*2, num[5]);
		Write_DATA(7*2, num[4]);
		for(count=7;count>0;count--)
			num[count] = num[count-1];

		switch(page)
		{
		case 0:
			if(key<4&&key>0)
				page=key;
			break;
		case 1:
			if(key==keyC)
			{
			page=0;
			TA1CCTL0 &= ~CCIE;// CCR0 interrupt enabled
			TA1CTL&=~TAIE;
		}
			else if(key==1)
			{
			kind=0;
			TA1CCTL0 = CCIE;// CCR0 interrupt enabled
			TA1CTL|=TAIE;
		}
			else if(key==2)
			{
			kind=1;
			TA1CCTL0 = CCIE;// CCR0 interrupt enabled
			TA1CTL|=TAIE;
		}
			break;

		case 2:
			if(key==keyC)
			{
			page=0;
			TA1CCTL0 &= ~CCIE;// CCR0 interrupt enabled
			TA1CTL&=~TAIE;

		}
			else
			{
			TA1CCTL0 = CCIE;// CCR0 interrupt enabled
			TA1CTL|=TAIE;
		}
			break;
		case 3:
			if(key==keyC)
			{
			page=0;
			TA1CCTL0 &= ~CCIE;// CCR0 interrupt enabled
			TA1CTL&=~TAIE;

		}
	}
	}
}
/**************************
	lcd_rst	 ---PJ.0
	lcd_cs	---p3.6
	lcd_bl_en ---p3.7
	clk		 ---p8.4
	spi_mosi	---p8.5
	spi_miso	---p8.6
 **************************/
void SPI_GPIO_Init(void)
{
	P8SEL |= BIT4+BIT5+BIT6;//Bit=1:Peripheral module function is selectedfor the pin
	P8DIR |=	BIT4+BIT5;//Port configured as output
	P8DIR &=	BIT6;//MISO

	// Configure TS3A5017DR	IN1 and IN2
	P3DIR |= BIT4 + BIT5;//P3.4 :IN1 ;P3.5 :IN2	 set as output
	P3OUT |= BIT4;//IN1 = 1
	P3OUT &= ~BIT5;//IN2 = 0
	P3DIR |= BIT7;
	P3OUT |= BIT7;

}

void TM1638_Init(void)
{
	P8DIR |= BIT0;
	P4DIR |= BIT6 + BIT7;

	P1DIR |= BIT0;// ACLK set out to pins
	P1SEL |= BIT0;
	P3DIR |= BIT4;// SMCLK set out to pins
	P3SEL |= BIT4;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	P4OUT ^= BIT1;// Toggle P1.0
	DAC12_0DAT=*sin_data_pr++;

	if (sin_data_pr >= &sin_table[10])
		sin_data_pr = &sin_table[0];

	DAC12_0DAT &= 0xFFF;// Modulo 4096

	TA1CCR0 += period_table[song_tone_table[kind][index]];
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
	switch(__even_in_range(TA1IV, 14))
	{
		case TA1IV_TAIFG:
			 if(k++==song_long_table[kind][m])
			 {
			 k=0;
			 if(++m==24)
				 m=0;

			 DAC12_0DAT=0x000;
			 TA1CCTL0 &=~ CCIE;
			 if(++n==1)
			 {
			 n=0;
			 TA1CCTL0 = CCIE;
			 TA1CCR0 = period_table[song_tone_table[kind][index++]];
			 if(index==24)
				 index=0;
		 }

		 }
	}
}
int turn=1;
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
	switch(__even_in_range(P2IV, 10))
	{
		case P2IV_P2IFG6:
			delay_ms(10);
			if ((P2IN & BIT6) == 0)
			{
			if(turn==1)
			{
			TA1CCTL0 &= ~CCIE;// CCR0 interrupt enabled
			TA1CTL&=~TAIE;
		}
			else
			{
			TA1CCTL0 |= CCIE;// CCR0 interrupt enabled
			TA1CTL|=TAIE;
		}
			turn=~turn;
		}
			break;// No interrupt
		case P2IV_P2IFG7:
			delay_ms(10);
			if ((P2IN & BIT7) == 0)
			{
			kind=~kind;
		}
	}
}


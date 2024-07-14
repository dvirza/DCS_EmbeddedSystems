#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx


#define   debounceVal      250


// PushButtons abstraction
#define PBsArrPort     P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
//#define PB3                0x08



// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

// RGB abstraction
#define RGBArrPortOut      P2OUT
#define RGBArrPortDir      P2DIR
#define RGBArrPortSEL      P2SEL

// ADC10 abstraction
//#define ADC10BArrPort         P1OUT
//#define ADC10ArrPortDir      P1DIR
//#define ADC10ArrPortSel      P1SEL

#define UARTPortMode       P1SEL
#define UARTPinsSel        P1SEL2
#define UARTPinsDir        P1DIR
#define UARTPinsOut        P1OUT

// Timers abstraction
#define Timer0_CCR0        TACCR0
#define Timer0_CTL         TACTL
#define Timer1_CCR0        TA1CCR0
#define Timer1_CCR1        TA1CCR1
#define Timer1_CCR2        TA1CCR2
#define Timer1_CTL         TA1CTL
#define Timer1_CCTL1       TA1CCTL1
#define Timer1_CCTL2       TA1CCTL2

// ADC abstraction
#define ADC_CTL0           ADC10CTL0
#define ADC_CTL1           ADC10CTL1
#define ADC_AE0            ADC10AE0
#define ADC_MEM            ADC10MEM
#define A3                 BIT3

// Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT

// UART abstraction
#define UART0RecBuf        UCA0RXBUF
#define UART0TransBuf      UCA0TXBUF
#define TXLED              BIT0
#define RXLED              BIT6
#define TXD                BIT2
#define RXD                BIT1
#define UARTTransIE        UCA0TXIE
#define UARTRecIE          UCA0RXIE
#define UARTIntReg         IE2


extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);
extern void TIMER1_A0_config(void);
extern void ADCconfig(void);
extern void UARTconfig(void);
extern void UART_Receive();
extern unsigned X;

#endif

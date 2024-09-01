#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx

#define   debounceVal      10000

// LCDs abstraction
#define LCD_DATA           0xF0;
#define LCD_CTL            0x0E;




//LCD abstraction
#define LCDDataArrPort            P2OUT
#define LCDDataArrPortDir         P2DIR
#define LCDDataArrPortSel         P2SEL
#define LCD_DATA_WRITE            P2OUT
#define LCD_DATA_DIR              P2DIR
#define LCD_DATA_READ             P2IN

#define LCDDataEntries            BIT4 + BIT5 + BIT6 + BIT7

#define LCDCTLArrPort             P2OUT
#define LCDCTLArrPortDir          P2DIR
#define LCDCTLArrPortSel          P2SEL

#define LCDControlEntries         BIT1 + BIT2 + BIT3

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

// JS abstraction
#define JSPortOut     P2OUT
#define JSPortSel     P2SEL
#define JSPortDir     P2DIR
#define JSPortIn      P2IN
#define JSIntEdgeSel  P2IES
#define JSIntEn       P2IE
#define JSIntPend     P2IFG

// Joystick ADC channels
#define JOYSTICK_X_CHANNEL  INCH_0 // X-axis on P1.0 (A0)
#define JOYSTICK_Y_CHANNEL  INCH_3 // Y-axis on P1.3 (A3)
#define JSPB                0x01
#define ToolToggleCommand   0xAA
#define ShowDynamicAngle    0xBB

// Motor abstraction
#define MotorPortOut     P1OUT
#define MotorPortSel     P1SEL
#define MotorPortDir     P1DIR


extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);
extern void TIMER1_A0_config(void);
extern void ADCconfig(void);
extern void UARTconfig(void);
extern void UART_Receive();
extern void Motorconfig(void);
extern void JSconfig(void);
extern void LCDconfig(void);






#endif

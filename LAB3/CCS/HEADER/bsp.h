#ifndef _bsp_H_
#define _bsp_H_

//#include  <msp430g2553.h>          // MSP430x2xx
#include  <msp430xG46x.h>  // MSP430x4xx


#define SMCLK_FREQ         1087438

#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xCCCC  // 500 ms


// Keypad abstraction
#define KPOut              P10OUT
#define KPDir              P10DIR
#define KPIn               P10IN

#define KPpress            0x02

#define KPArrPort          P2IN
#define KPArrIntPend       P2IFG
#define KPArrIntEn         P2IE
#define KPArrIntEdgeSel    P2IES
#define KPArrPortSel       P2SEL
#define KPArrPortDir       P2DIR

#define KPIRQ              0x02


// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0xFE

//    PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG 
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL 
#define PBsArrPortDir      P1DIR 
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x02


//    LEDs abstraction
#define LEDsArrPort        P9OUT
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL


//    LCD abstraction
#define LCD_DATA_WRITE    P1OUT
#define LCD_DATA_DIR      P1DIR
#define LCD_DATA_SEL      P1SEL
#define LCD_CTL_SEL       P2SEL
#define LCD_DATA_READ     P1IN


//   BUZER abstraction
#define BUZER_OUT         P2OUT
#define BUZER_DIR         P2DIR
#define BUZER_SEL         P2SEL


//    genrator abstraction
#define GENsArrPortIn	    P2IN 
#define GENsArrPortDir      P2DIR 
#define GENsArrPortSel      P2SEL
#define GENsArrPortOut	    P2OUT 

extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void TIMERcfgST4(void);
extern void DMAconfig(void);


#endif




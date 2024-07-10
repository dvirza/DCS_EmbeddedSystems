 // private library - BSP layer

#ifdef __MSP430FG4619__
#include  "../header/bsp_msp430x4xx.h"
#else
#include  "../header/bsp_msp430x2xx.h"
#endif

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P1.4-P1.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7

  // RGB configuration
  RGBArrPortOut = 0x00;
  RGBArrPortDir |= 0x07;
  RGBArrPortSEL &= ~0x07;

  // Buzzer Setup
  BuzzPortDir |= 0x10;             // P2.4 Output compare - '1'
  BuzzPortSel |= 0x10;             // P2.4 Select = '1'
  BuzzPortOut &= ~0x10;             // P2.4 out = '0'

  _BIS_SR(GIE);                     // enable interrupts globally
}                             

//------------------------------------------------------------------------------------- 
//                           Timers configuration
//-------------------------------------------------------------------------------------
void TIMER0_A0_config(void){
    Timer0_CCR0 = X;
    Timer0_CTL |= TASSEL_2 + MC_0 + ID_3 + TACLR;  //  select: 2 - SMCLK ; control: 0 - hold  ; divider: 3 - /8
    Timer0_CTL &= ~TAIE;
}  

void TIMER1_A0_config(void){
    Timer1_CCR0 = 0xFFFF;
    Timer1_CTL |= TASSEL_2 + MC_0 + ID_0 + TACLR;  // select: 2 - SMCLK ; control: 0 - hold  ; divider: 3 - /8
    Timer1_CTL &= ~TAIE;
}

//------------------------------------------------------------------------------------- 
//                            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
      ADC_CTL0 = ADC10SHT_2 + ADC10ON + SREF_0 + ADC10IE;  // 16*ADCLK + Turn on + ref to Vcc and Gnd voltage + Interrupt Enable
      ADC_CTL1 = INCH_3 + ADC10SSEL_3;     // Input A3(P1.3) + use SMCLK freq for sampling
      ADC_AE0 |= A3;                      // P1.3 ADC
}

//-------------------------------------------------------------------------------------
//                               UART configuration
//-------------------------------------------------------------------------------------

void UARTconfig(void){
    if (CALBC1_1MHZ==0xFF)                   // If calibration constant erased
    {
      while(1);                              // do not load, trap CPU!!
    }
    DCOCTL = 0;                              // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                   // Set DCO
    DCOCTL = CALDCO_1MHZ;

//    P2DIR = 0xFF;                          // All P2.x outputs
//    P2OUT = 0;                             // All P2.x reset
    UARTPortMode = RXD + TXD ;               // P1.1 = RXD, P1.2=TXD
    UARTPinsSel = RXD + TXD ;                // P1.1 = RXD, P1.2=TXD
    //UARTPinsDir |= RXLED + TXLED;
    UARTPinsOut &= 0x00;

    UCA0CTL1 |= UCSSEL_2;                    // Selects SMCLK for UART clock
    UCA0BR0 = 104;                           // Baud rate setting for 1MHz 9600
    UCA0BR1 = 0x00;                          // Baud rate setting for 1MHz 9600
    UCA0MCTL = UCBRS0;                       // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                    // **Initialize USCI state machine**
}




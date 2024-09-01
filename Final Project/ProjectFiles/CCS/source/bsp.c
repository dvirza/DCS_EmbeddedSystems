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
  
  _BIS_SR(GIE);                     // enable interrupts globally
}                             

//------------------------------------------------------------------------------------- 
//                           Timers configuration
//-------------------------------------------------------------------------------------

void TIMER0_A0_config(void){
    Timer0_CCR0 = 0xFFFF;
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
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_3 + CONSEQ_1 + ADC10SSEL_3;            // A1/A3 highest highest channel for a sequence of conversions
    //ADC10DTC1 = 0x02;                         // 2 conversions
    ADC10AE0 |= 0x09;                         // P1.0-1.3 ADC10 option select
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

    UARTPortMode = RXD + TXD ;               // P1.1 = RXD, P1.2=TXD
    UARTPinsSel = RXD + TXD ;                // P1.1 = RXD, P1.2=TXD
    UARTPinsOut &= 0x00;


    UCA0CTL1 |= UCSSEL_2;                    // Selects SMCLK for UART clock
    UCA0BR0 = 104;                           // Baud rate setting for 1MHz 9600
    UCA0BR1 = 0x00;                          // Baud rate setting for 1MHz 9600
    UCA0MCTL = UCBRS0;                       // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                    // **Initialize USCI state machine**
}


//-------------------------------------------------------------------------------------
//                               JS configuration
//-------------------------------------------------------------------------------------
//  P1.3 - Vrx; P1.4 - Vry; P1.5 - PB
// P1.3-P1.4 - X(Don't care) for Sel, Dir According the dataSheet For A3,A4 input Select For ADC

void JSconfig(void){
    JSPortSel &= ~0x01;  // P2.0 Sel = '0'
    JSPortDir &= ~0x01;  // P2.0 input = '0'
    JSPortOut &= ~0x01;  //
    JSIntEn |= 0x01; // P2.0 PB_interrupt = '1'
    JSIntEdgeSel |= 0x01; // P2.0 PB pull-up - '1'
    JSIntPend &= ~0x01; // Reset Int IFG - '0'
}

//-------------------------------------------------------------------------------------
//                               Step Motor configuration
//-------------------------------------------------------------------------------------
void Motorconfig(void){
    MotorPortSel &= ~0xF0;  // P1.4-P1.7 Sel = '0' (GPIO)
    MotorPortDir |= 0xF0;  // P1.4-P1.7 Dir = '1' (Output)
}

void LCDconfig(void){

    LCDDataArrPort      &= ~LCDDataEntries;
    LCDDataArrPortDir   |= LCDDataEntries;
    LCDDataArrPortSel   &= ~LCDDataEntries;
    LCDCTLArrPortSel    &= ~LCDControlEntries;
    LCDCTLArrPortDir    |= LCDControlEntries;
    LCDCTLArrPort       &= ~LCDControlEntries;

}




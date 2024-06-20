#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

const char* LCD_state1_txt = "fin =       Hz";

int state2Timer; // counts time staying in state 2
unsigned int TA1start;
unsigned int TA1end;
unsigned int CycleDiff = 0; // diff hold how many mclk clocks passed between for 1 sampled signal clock



unsigned int SampledFreq;

void main(void){

  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init(); //initiate anc clear lcd
  lcd_clear();
  
  state2Timer = 0;
  
  
  while(1){
	switch(state){
	  case state0:
		enterLPM(lpm_mode);
		break;
		 
          case state1: // Measure Frequency and Print On LCD (TimerA1)
          LCD_State1();  // initiante "fin" string on LCD
          enable_TimerA1();
          
            while(state == state1){ 
              disable_interrupts();
              FreqScan();   // Measure frequency
              refreshFreq(SampledFreq,ST1str); // Print measure on LCD
              enable_interrupts();
              }
            
          disable_TimerA1();
          break;
                
                
                
	  case state2: // Count Seconds and Print On LCD (TimerA0)
            TIMERconfig();
            enable_TimerA0();
            
            while(state == state2){
              countBySwitch();
            }
            
            disable_TimerA0();
            break;

          case state3: // Generates a signal with buzzer calibration
          enable_TimerA1S3();
          enable_interrupts();
          
          while(state == state3){
            Buzz();
          }
          
          disable_TimerA1();
          break;
          
        case state4:
          TIMERcfgST4();
          enable_TimerA0();
          while (state == state4){
            lcd_clear();
            lcd_home();
            SkippingB();
          }
          disable_TimerA0();
          break;
	}
  }
}
#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum    FSMstate state;
enum    SYSmode lpm_mode;

char    data_marix[M][N];
char    idiom_recorder[32];
int     KPToggle = 0;

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
          enable_keypad();
          enable_TimerBCCIE();
            while(state == state1){ 
                KPToggle = 0;
                idiomrecorder();
              }
            
          disable_TimerA1();
          break;
                
                
                
	  case state2: // Count Seconds and Print On LCD (TimerA0)
            
            while(state == state2){

            }
            

            break;

          case state3: // Generates a signal with buzzer calibration
          
          while(state == state3){


          }
          

          break;
          
        case state4:

          while (state == state4){

          }

          break;
	}
  }
}

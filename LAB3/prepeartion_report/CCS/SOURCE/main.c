#include  "../header/api.h"    		// private library - API layer

enum    FSMstate state;
enum    SYSmode lpm_mode;

char    idiom_recorder[32];
int     KPToggle = 0;

void main(void){

  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init(); //initiate and clear LCD
  lcd_clear();
  
  while(1){
	switch(state){
	      case state0:
	          enterLPM(lpm_mode);
	          break;

          case state1: // Measure Frequency and Print On LCD (TimerA1)
              idiomrecord();
//              state = state0;
              break;

          case state2: // Count Seconds and Print On LCD (TimerA0)
              Merge();
              state = state0;
              break;

          case state3: // Generates a signal with buzzer calibration
              LedSwitch();
              break;

          case state4:
              RealTimeTask();
              break;
          }
          
	}
  }


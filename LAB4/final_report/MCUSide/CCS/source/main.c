#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

unsigned int num = 0;
unsigned int X = 0xFFFF;
unsigned int X_flag = 0;
char timerInput[6];

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  while(1){
    lcd_clear();
	switch(state){
	  case state0:
	      UART_Receive();
	      enterLPM(lpm_mode);
		  break;
		 
	  case state1:
	      UART_Receive();
	      blinkRGB();
		  break;
		 
	  case state2:
	      UART_Receive();
	      count();
		  break;
                
	  case state3:
	      UART_Receive();
	      buzzer();
	      break;
            
	  case state4:
        if(state == state4){
            UART_Receive();
            state = state0;
        }
        set_X();
		break;   

      case state5:
          UART_Receive();
          measADC10();
          break;

      case state7:
          ShowLove();
          state = state0;
          break;

	}
  }
}

#ifndef _halGPIO_H_
#define _halGPIO_H_

#ifdef __MSP430FG4619__
#include  "../header/bsp_msp430x4xx.h"
#else
#include  "../header/bsp_msp430x2xx.h"
#endif

#include  "../header/app.h"    		// private library - APP layer
#include "stdint.h"


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
enum RXStatesCommands {eEnterSleepMode = '0',eEnterManualMode = '1',eEnterPaintMode = '5',eEnterCalibrationMode = '6',eEnterScriptMode = '7', eStartReceivingFileData = '8'}; // global variable
enum RXScriptMode {eScriptLoad = 'L',eScriptRemove = 'R',eScriptExecute = 'E',eQuitScriptMode = '0',eEndOfLine = '.', eEndOfData = '\n'}; // global variable


extern unsigned int TXFileNumber;
extern unsigned int TXAck;
extern int Trans7Pointer;
extern unsigned int count;
extern unsigned int FullRoundSteps;
extern float bytes_to_float(char *bytes);
extern unsigned int currStep;
extern int Xcenter;
extern int Ycenter;
extern unsigned int TXsync;
extern unsigned int TXFile;
extern unsigned int TXcmd;
extern unsigned int RXExecute;
extern unsigned int FileNumber;



extern void MotorCWStep(unsigned int *Pos);
extern void MotorCCWStep(unsigned int *Pos);

extern void sysConfig(void);
extern void delay(unsigned int);
extern void enterLPM(void);
extern void enable_interrupts();
extern void disable_interrupts();
extern void startTimerA0();
extern void stopTimerA0();

extern void startTimerA1();
extern void stopTimerA1();
extern void UART_Transmit();
extern void UART_TransmitByte(char chr);
extern void UART_TransmitCmd(uint8_t val);

void readJS_x_y( int* Vx, int* Vy);
extern void GetJSvals(int *VRX_avg,  int *VRY_avg);
void getReqPos(const int * x,const int * y);
extern __interrupt void PBs_handler(void);

#endif

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (P2OUT&=~0X02) : (P2OUT|=0X02)) // P2.1 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~0X02) : (P2DIR|=0X02)) // P2.1 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~0X04) : (P2OUT|=0X04)) // P2.2 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~0X04) : (P2DIR|=0X04)) // P2.2 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~0X08) : (P2OUT|=0X08)) // P2.3 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~0X08) : (P2DIR|=0X08)) // P2.3 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset


/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

#define MAX_FILES 3
#define MAX_FILENAME_LENGTH 16
#define FLASH_SEGMENT_SIZE 64
#define MAX_CMDS 10

#define FLASH_FILE0_DATA 0x01000
#define FLASH_FILE1_DATA 0x01040
#define FLASH_FILE2_DATA 0x01080

#define FLASH_FILE0_NAME 0x010B0
#define FLASH_FILE1_NAME 0x010C0
#define FLASH_FILE2_NAME 0x010D0

#define FLASH_FILE0_SIZE 0x010E0
#define FLASH_FILE1_SIZE 0x010E2
#define FLASH_FILE2_SIZE 0x010E4







extern void FLASHconfig(void);

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
/*
 *  Delay functions for HI-TECH C on the PIC18
 *
 *  Functions available:
 *      DelayUs(x)  Delay specified number of microseconds
 *      DelayMs(x)  Delay specified number of milliseconds
*/


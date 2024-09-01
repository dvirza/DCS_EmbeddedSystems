#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include "string.h"

unsigned int CalibToggle = 0;
unsigned int RXType = 0;
unsigned int RXExecute = 0;
char ReceivedData;
unsigned int EndTransmit = 0;
unsigned int EndReceive = 0;
unsigned int currStep = 0;
int Xcenter = 462; // 462
int Ycenter = 490; // 490

unsigned int tx_buffer;
unsigned int TXcmd = 0; // command transmit indicator
unsigned int TXuint = 0;
unsigned int TXAck = 0;
unsigned int TXFile = 0;
unsigned int TXFilePtr = 0;


unsigned int FileReceiveStep;
char TXDataBuffer[FLASH_SEGMENT_SIZE];
char RXDataBuffer[FLASH_SEGMENT_SIZE];
unsigned int RXDataBuffPtr = 0;



unsigned int FileNumber = 0;
unsigned int TXDataBuffPtr = 0;
unsigned int TXsync = 0;




typedef struct {
    uint8_t file_count;                     // Number of files saved in flash
    char * file_pointers[MAX_FILES];      // Array of pointers to each file beginning in flash
    unsigned int * file_sizes[MAX_FILES];         // Array of file sizes
} FlashFilesStruct;

FlashFilesStruct flash_files;

        // centralized Xval = 464, Yval = 496

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMER0_A0_config();
    TIMER1_A0_config();
	ADCconfig();
	UARTconfig();
	Motorconfig();
	JSconfig();
	FLASHconfig();
	LCDconfig();
}

void FLASHconfig(void){
    flash_files.file_pointers[0] = (char*)FLASH_FILE0_DATA;
    flash_files.file_pointers[1] = (char*)FLASH_FILE1_DATA;
    flash_files.file_pointers[2] = (char*)FLASH_FILE2_DATA;
    flash_files.file_sizes[0] = (unsigned int*)FLASH_FILE0_SIZE;
    flash_files.file_sizes[1] = (unsigned int*)FLASH_FILE1_SIZE;
    flash_files.file_sizes[2] = (unsigned int*)FLASH_FILE2_SIZE;
}

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(void){
    _BIS_SR(LPM0_bits); // Enter Low Power Mode 0
}

//-------------------------------------------------------------
//   start/shutdown ADC
//-------------------------------------------------------------
void startADC10(){
     ADC_CTL0 |= ENC + ADC10SC;  //   [ADC10 enable] + [Start Sample and Conversion]
}

void stopADC10(){
    ADC_CTL0 &= ~ADC10ON;
}
//-------------------------------------------------------------
//   Joystick Sample ADC
//-------------------------------------------------------------
void GetJSvals(int *VRX_avg, int *VRY_avg){
        int VRX = 0;
        int VRY = 0;
        *VRX_avg = 0;
        *VRY_avg = 0;
        int i;
        for(i = 0; i < 32; i++){
            readJS_x_y(&VRX,&VRY);
            *VRX_avg += VRX;
            *VRY_avg += VRY;
        }
        *VRX_avg = *VRX_avg >> 5;
        *VRY_avg = *VRY_avg >> 5;
}

void readJS_x_y( int* Vx, int* Vy){
    startADC10(); // ADC10CTL0 =| ENC + ADC10SC
    enterLPM();
    *Vx = ADC10MEM;
    while (ADC10CTL1 & ADC10BUSY){
        ADC10CTL0 &= ~ADC10SC;
        ADC10CTL0 |= ADC10SC;
        enterLPM();
    }
    *Vy = ADC10MEM;
    ADC10CTL0 &= ~(ENC + ADC10SC);
}


//---------------------------------------------------------------------
//            JoyStick Functions
//---------------------------------------------------------------------
void JoyStickIE(void){
    JSIntEn |= 0x01;
}

void JoyStickID(void){
    JSIntEn &= ~0x01;
}

// A B C D 1000 0100 0010 0001
void MotorCWStep(unsigned int *Pos){
    if ((*Pos) == 8)
        (*Pos) = 1;
    else (*Pos) = (*Pos) << 1;
    MotorPortOut &= ~0xF0;
    MotorPortOut |= (*Pos) << 4;
    if (state != state3){
        currStep ++;
        if (currStep > FullRoundSteps)
            currStep -= FullRoundSteps;
    }
}
void MotorCCWStep(unsigned int *Pos){
    if ((*Pos) == 1)
        (*Pos) = 8;
    else (*Pos) = (*Pos) >> 1;
    MotorPortOut &= ~0xF0;
    MotorPortOut |= (*Pos) << 4;
    if (state != state3)
        if (currStep == 0)
            currStep += FullRoundSteps;
        else
            currStep -= 1;
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------

//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
        else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);
    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
        else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm(" nop");
  asm(" nop");
  LCD_EN(0);
}

//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
 

//-------------------------------------------------------------
//              StartTimer and FinishTimer For Count UpDown
//-------------------------------------------------------------
void startTimerA0(){
    Timer0_CTL |= MC_1 + TACLR; //  [SMCLK] + [Up/Down]  [/8]
    Timer0_CTL |= TAIE;         //  TimerA0 Interrupt Enable
    enterLPM();
}

void stopTimerA0(){
    Timer0_CTL |= TASSEL_2 + MC_0 + ID_3 + TACLR;
    Timer0_CTL &= ~TAIE;
}

//---------------------------------------------------------------------
//   start/shutdown timer and enable/disable interrupts
//---------------------------------------------------------------------
void startTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_1 + ID_0 + TACLR; //[SMCLK] + [UPMODE UP->TACCR0] + [/1] + [TIMERA CLEAR]
}

void stopTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_0 + ID_0 + TACLR; // [SMCLK] + [STOP MODE-HALTED] + [/1] + [TIMERA CLEAR]
}

void enableTimerA1_bzr(){
    Timer1_CCTL2 |=  OUTMOD_7; // [RESET/SET]
}

void disableTimerA1_bzr(){
    Timer1_CCTL2 &=  ~OUTMOD_7;
}
//---------------------------------------------------------------------
//            UART Functions
//---------------------------------------------------------------------
void UART_Receive(){
    UARTIntReg |= UARTRecIE;
    EndReceive = 0;
}

void UART_Transmit(){
    EndTransmit = 0;
    UARTIntReg |= UARTTransIE;
    enterLPM();
}

void UART_TransmitInt(const int * val){
    TXuint = 1;
    tx_buffer = *val;
    UART_Transmit();
}

void UART_TransmitCmd(uint8_t val){
    TXcmd = 1;
    if (TXsync)
        TXcmd++;
    tx_buffer = val;
    UART_Transmit();
}

void UART_BlockTransmit(){// UART TRANSMISSION DISABLE
    UARTIntReg &= ~UARTTransIE;
}
void UART_TransmitByte(char chr)
{
    tx_buffer = chr;
    TXFile = 1;
    UART_Transmit();
}
//---------------------------------------------------------------------
//   Script Mode Functions
//---------------------------------------------------------------------

void write_Seg(void)
{
    char *Flash_ptr_write;                // Flash pointer
    unsigned int k;

    // Initialize Flash pointer
    Flash_ptr_write = (char*)flash_files.file_pointers[FileNumber];

    // Erase the Flash segment
    FCTL1 = FWKEY + ERASE;                // Set Erase bit
    FCTL3 = FWKEY;                        // Clear Lock bit
    *Flash_ptr_write = 0;                 // Dummy write to erase Flash segment

    // Enable writing to the Flash memory
    FCTL1 = FWKEY + WRT;                  // Set WRT bit for write operation

    // Write the data to Flash
    for (k = 0; k < *(flash_files.file_sizes[FileNumber]); k++)
    {
        *Flash_ptr_write++ = RXDataBuffer[k]; // Write value to flash
    }

    // Clear the data buffer
    //memset(file_content, 0, sizeof(file_content));

    // Disable writing to Flash and lock the memory
    FCTL1 = FWKEY;                        // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                 // Set LOCK bit
}

//*********************************************************************
//            Port2 Interrupt Service Routine (Joystick PB)
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){

      delay(debounceVal);

      if (JSIntPend & JSPB){
          JSIntPend &= ~JSPB;

          if (state == state2)
                TXcmd = 1;

          else if (state == state3){
              FullRoundSteps = count;
              if (!CalibToggle)
              {
                  count = 0;
              }
              CalibToggle ++;
          }
          }

      LPM0_EXIT;
  }


//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void){
if (Timer0_CTL & TAIFG){
  Timer0_CTL &= ~TAIFG;            //turn off flag
  LPM0_EXIT;
}
}

//---------------------------------------------------------------------
//            ADC10 Vector Interrupt Service Routine
//---------------------------------------------------------------------

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
   LPM0_EXIT;
}

//---------------------------------------------------------------------
//                      UART TX ISR
//---------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    char * currChar;

    //UART_Transmit_Int
    if (TXuint == 1)
    {
        UART0TransBuf = (unsigned char)(tx_buffer & 0xFF);
        TXuint++;
    }
    else if (TXuint == 2)
    {
        tx_buffer = (unsigned char)((tx_buffer >> 8) & 0xFF);
        UART0TransBuf = tx_buffer;
        EndTransmit = 1;
    }

    //Transmit Command

    if (TXcmd == 1){
        UART0TransBuf = (uint8_t)(tx_buffer & 0xFF);
        TXcmd ++;
    }
    else if (TXcmd == 2){
        UART0TransBuf = (uint8_t)(tx_buffer & 0xFF);
        EndTransmit = 1;
    }

    //Transmit File
    if (TXFile == 1)
    {
        if (TXFilePtr < 64) // send the whole file 0 data
        {
            currChar = (char*)(FLASH_FILE0_DATA + TXFilePtr);
            UART0TransBuf = *currChar;
            TXFilePtr ++;
        }
        else // send end of file
        {
            UART0TransBuf = (unsigned char)('\n');
            TXFile ++;
            TXFilePtr = 0;
        }
    }
    else if (TXFile == 2) // send first file data
    {
        if (TXFilePtr < 64) // send the whole file data while its not EOF (TXFilePtr == 64)
        {
            currChar = (char*)(FLASH_FILE1_DATA + TXFilePtr);
            UART0TransBuf = *currChar;
            TXFilePtr ++;
        }
        else // send end of file
        {
            UART0TransBuf = (unsigned char)('\n');
            TXFile ++;
            TXFilePtr = 0;
        }
    }
    else if (TXFile == 3) // send first file data
    {
        if (TXFilePtr < 64) // send the whole file data while its not EOF (TXFilePtr == 64)
        {
            currChar = (char*)(FLASH_FILE2_DATA + TXFilePtr);
            UART0TransBuf = *currChar;
            TXFilePtr ++;
        }
        else // send end of file
        {
            UART0TransBuf = (unsigned char)('\n');
            TXFile ++;
            TXFilePtr = 0;
        }
    }
    else if (TXFile == 4)
    {
        UART0TransBuf = (unsigned char)('\n');
        EndTransmit = 1;
    }




    //Transmit Acknowledge
    if (TXAck == 1)
    {
        UART0TransBuf = 'A';
        EndTransmit = 1;
    }

    //Transmit Routine Ended:
    if (EndTransmit == 1)
    {
        TXuint = 0;
        TXAck = 0;
        TXcmd = 0;
        TXsync = 0;
        TXFile = 0;

        UARTIntReg &= ~UARTTransIE; // Disable UART TX interrupt to prevent re-entry
        LPM0_EXIT; // Ensure MCU wakes up after each transmission
    }
}


//*********************************************************************
//                      UART RX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    ReceivedData = UART0RecBuf;
    if (RXType == 3)
    {
        FileNumber = ReceivedData - '0';
        RXType = 1;
        RXExecute = 1;
        EndReceive = 1;
    }

    else if (RXType == 2){ // [LOAD MODE] We're in script mode expecting file data and saving it into the flash
        //receive a file name

        if (FileReceiveStep == 1)
        { // get a file number
            //choose the file number in the file slot and initiate pointers(receiving buffer pointer at 0)
            FileNumber = ReceivedData - '0';
            FileReceiveStep++;

        }
        else if (FileReceiveStep == 2)
        {
            if (ReceivedData != eEndOfData) // receive the file data
            { // received a file raw data
                RXDataBuffer[RXDataBuffPtr] = ReceivedData;
                RXDataBuffPtr++;
            }
            else
            { // received EOF
                RXDataBuffer[RXDataBuffPtr] = ReceivedData;
                RXDataBuffPtr++;
                write_Seg(); // save the file data into the flash
                RXDataBuffPtr = 0;
                RXType = 1;
                TXAck = 1;
                EndReceive = 1;

            }
        }
    }
    else if (RXType == 1)
    {  // We're in script mode and expecting script mode communication
        EndReceive = 0;
        if (ReceivedData == eScriptLoad) { // memory load into flash
        // handle a memory load now
            RXType = 2; // receive a file data mode
            FileReceiveStep = 1; // receive its name

        }
        else if (ReceivedData == eScriptExecute)
        { // execute existing program from flash
            RXType = 3;
        }
        else if (ReceivedData == eQuitScriptMode)
        { // Quit script mode
            RXType = 0;
            ReceivedData = eEnterSleepMode;
        }
    }
    if (RXType == 0) {
        // Handle other commands
        if (ReceivedData == eEnterSleepMode)  {
            state = state0;
            EndReceive = 1;
        } else if (ReceivedData == eEnterManualMode)  {
            state = state1;
            EndReceive = 1;
        } else if (ReceivedData == eEnterPaintMode) {
            state = state2;
            EndReceive = 1;
        } else if (ReceivedData == eEnterCalibrationMode) {
            state = state3;
            CalibToggle = 0;
            EndReceive = 1;
        } else if (ReceivedData == eEnterScriptMode) {
            RXType = 1;
            state = state4;
            EndReceive = 1;
        }
    }

    if (EndReceive)
    {
        LPM0_EXIT;
    }
}


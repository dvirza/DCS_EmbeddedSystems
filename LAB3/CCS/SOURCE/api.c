#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer


int timerEnded;
int TimerBDone = 0;


char    LCDarray[32];
char data_matrix[10][40] = {
        "An apple a day keeps the doctor away ",
        "climb on the bandwagon ",
        "Dot the i's and cross the t's ",
        "He who pays the piper calls the tune ",
        "The pen is mightier than the sword ",
        "The pot calling the kettle black ",
        "shed crocodile tears ",
        "Close but no cigar " ,
        "Cut from the same cloth " ,
        "Strike while the iron’s hot "};

int State3Arrray[9] = {128,64,32,16,8,4,23,13,40};

int sizes_matrix[10][9] = {
           {3,6,2,4,6,4,7,5,0},
           {6,3,4,10,0,0,0,0,0},
           {4,4,4,4,6,4,4,0,0},
           {3,4,5,4,6,6,4,5,0},
           {4,4,3,9,5,4,6,0,0},
           {4,4,8,4,7,6,0,0,0},
           {5,10,6,0,0,0,0,0,0},
           {6,4,3,6,0,0,0,0,0},
           {4,5,4,5,6,0,0,0,0},
           {7,6,4,7,4,0,0,0,0}};
int Input1;
int Input2;



//*****************************************************************************
// STATE 1 FUNCTIONS
//*****************************************************************************

//--------------------------------------------------------------------
//            Prints Frequency measurement template to LCD
//--------------------------------------------------------------------
void LCD_State1(){
  lcd_clear();
  lcd_home();
  lcd_puts(LCD_state1_txt);
}


//--------------------------------------------------------------------
//            State 1 Keypad Handler
//--------------------------------------------------------------------
void idiomrecord(void){
        char howMuchCharsToKey [16] = {2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3};
        char KeyToChars [16][3] = { "1G" , "2H" , "3I" , "CJ" , "4K" , "5L" , "6M" , "DN" , "7O" , "8P" , "9Q" , "ER" , "AST" , "0UV" , "BWX" , "FYZ"};
        char lastChosen = 'r';
        char currIndex = 0;
        char numOfletters = 0;
        timerEnded = 0;
        lcd_clear();
        lcd_home();
        lcd_puts("Keypad Type Mode");
        KB = 'r';
        enable_TimerBCCIE();
        enable_keypad();
        enterLPM(lpm_mode);
        lcd_clear();
        lcd_home();
        while (state == state1 && numOfletters<32){
            if (numOfletters == 16){
                lcd_home();
                lcd_new_line;
            }
            if (KB != 'r'){
                if (lastChosen == 'r'){ //this is when start new session on a key
                    timerB1Sec();
                    lastChosen = KB;
                    currIndex = 0;
                    numOfletters += 1;
                    lcd_putchar(KeyToChars[KB-'a'][currIndex]);
                    timerEnded = 0;
                }
                else if(lastChosen != KB){
                    timerBclear();
                    idiom_recorder[numOfletters-1] = KeyToChars[lastChosen][currIndex];
                    currIndex = 0;
                    numOfletters++;
                    lastChosen = KB;
                    lcd_putchar(KeyToChars[KB-'a'][currIndex]);
                    timerEnded = 0;

                }
                else{ //lastChosen == KB
                    currIndex = (currIndex + 1) % howMuchCharsToKey[KB-'a'];
                    lcd_cursor_left();
                    lcd_putchar(KeyToChars[KB-'a'][currIndex]);
                    timerBclear();
                    timerEnded = 0;
                }
            }
            enterLPM(lpm_mode);
            if (timerEnded == 1){
                disable_TimerB();
                idiom_recorder[numOfletters-1] = KeyToChars[KB-'a'][currIndex];
                lastChosen = 'r';
                enterLPM(lpm_mode);
            }
        }
        disable_TimerBCCIE();
        disable_keypad();

        state = state0;
        lcd_clear();
}

//--------------------------------------------------------------------
//            State 2 Merging Strings
//--------------------------------------------------------------------
void Merge(){
    char* SrcAdd1;
    char* SrcAdd2;
    char strMerge[80];

    mask_PBs();
    //ask an input from user
    ChooseSentence();
    //convert inputs to data_matrix pointers
    SrcAdd1 = data_matrix[Input1];
    SrcAdd2 = data_matrix[Input2];
    //dma operation
    DMAstate2(SrcAdd1,SrcAdd2,strMerge); // still leaves garbage in the string end
    //show_merge string
    showString(strMerge);
    unmask_PBs();
    lcd_clear();
    state = state0;
}

void ChooseSentence(){
    lcd_clear();
    lcd_home();
    lcd_puts("Choose 2 nums:");
    lcd_new_line;
    KB = 'r';
    enable_keypad();
    while (KB == 'r'){
        enterLPM(lpm_mode);
    }
    Input1 = KB - '0';
    lcd_putchar(KB);

    lcd_cursor_right();
    KB = 'r';
    enable_keypad();
    while (KB == 'r'){
        enterLPM(lpm_mode);
    }
    Input2 = KB - '0';
    lcd_putchar(KB);
}

void DMAstate2(char* Src1, char* Src2, char* Dest){
    int bias1 = 0;
    int bias2 = 0;
    int i;
    int j;
    //initiate dma pointers
    for (i = 0;i < 9; i ++){

        if (sizes_matrix[Input1][i] != 0){
            DMAPrepare(Src1 + bias1,Dest+ bias1 + bias2,sizes_matrix[Input1][i]);
            bias1 = bias1 + sizes_matrix[Input1][i];
            DMA0vamos();
        }



        if (sizes_matrix[Input2][i] != 0){
            DMAPrepare(Src2 + bias2,Dest+ bias1 + bias2,sizes_matrix[Input2][i]);
            bias2 = bias2 + sizes_matrix[Input2][i];
            DMA0vamos();
        }
    }
}

void showString(char* str){
    int j;
    // ensures full scroll of the strMerge
    lcd_clear();

    for(j = 0; j < 32 ; j ++ ){
        lcd_putchar(str[j]);
        if (j == 15)
            lcd_new_line;
    }

    cursor_off;
    j = j-16;
    while ( j < 80 ){
        enterLPM(lpm_mode);
        lcd_home();
        cursor_on;
        int i;
        for( i = j ; i < j+32 ; i ++ ){
                lcd_putchar(str[i]);
                if (i == j+15){
                    lcd_home();
                    lcd_new_line;
                }
        }
        j = j + 16;
    }

}


void LedSwitch(){
    int i = 0;
    enable_TimerBCCIE();
    while (state == state3){
        TimerBDone = 0;
        timerBHsec();
        enterLPM(lpm_mode);
        if (TimerBDone) {
        DMAPrepareST3(&(State3Arrray[i % 9]));
        DMA1vamos();
        timerBclear();
        i ++;
        }
        }
    LEDsArrPort = 0;
    disable_TimerBCCIE();

}




//--------------------------------------------------------------------
//            Updates the freq meas val on lcd screen
//--------------------------------------------------------------------


//*****************************************************************************
// STATE 2 FUNCTIONS
//*****************************************************************************



//*****************************************************************************
// STATE 3 FUNCTIONS
//*****************************************************************************


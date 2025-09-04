#include "mbed.h"
#include "LCD_includes.h"
#include <cstdint>

BusInOut lcd(D10, D9, D7, D6, D5, D4, D3, D2, D1, D0); 
DigitalOut EN(D8);

uint16_t init_arr[5] = {
    START, CLEAR_LCD_DISPLAY, RETURN_CURSOR_HOME, LCD_BLNK_SHRT, DEFAULT_ENTRY
}; 

/*
Function: Initialises the LCD into 8-bit mode
NOTE: Must be called before any LCD operations are performed. 
Usage/Example: 
int main()
{
    _INITIALISE_LCD(8); 
}
*/
void _INITIALISE_LCD(int _mode)
{
    if(_mode == 8)
    {
        // set pins to output 
        lcd.output(); 
        for(int i = 0; i<5; i++)
        { 
            lcd.write(init_arr[i]);
            thread_sleep_for(5);
            EN = 1; 
            thread_sleep_for(5); 
            EN = 0; 
            thread_sleep_for(5);
        } 
        return;

    } else if(_mode == 4)
    {
        thread_sleep_for(5);
        return;  

    } else 
    {
        printf("INVALID MODE - DEFAULT TO 8-BIT MODE");
        // set pins to output 
        lcd.output(); 
        for(int i = 0; i<5; i++)
        { 
            lcd.write(init_arr[i]);
            thread_sleep_for(5);
            EN = 1; 
            thread_sleep_for(5); 
            EN = 0; 
            thread_sleep_for(5);

    }
    return; 

}
}

/*
Function: Print a specific character from ROM to the LCD screen at the current position 
letter = ROM address of the character 
Usage/Example: 
int main()
{
    _LCD_WRITE_CHAR(0b0001 0010); // print the letter H to the screen
};
*/
void _LCD_WRITE_CHAR(uint8_t letter)
{
    uint16_t _append_command; 
    // concatenate commands to create the full command: 
    _append_command = ((letter << 2) | (RS_SET)); // shift left by two places, OR with RS_SET  
    
    lcd.output(); 
    EN = 1; 
    thread_sleep_for(5); 
    lcd.write(_append_command);  
    thread_sleep_for(5);
    EN = 0; 
    return; 

}

/*
Function: Print a custom string of characters to the display. 
arr_buff[] = pointer to custom arra & size_arr = size of array. 
Usage/Example: 
int main()
{
    uint8_t arr[8] = {
   0b00010010, 0b00010010, 0b00010010, 0b00010010, 0b00010010, 0b00010010 , 0b00010010, 0b00010010 // print character 'H' six times
   
   _LCD_PRINT_STR(arr, 8); // pass array pointer and size
}; 

*/
void _LCD_PRINT_STR(uint8_t arr_buff[], int size_arr) // accept pointer to an array as an input 
{
    uint16_t _append_command_prnt;
    lcd.output(); 
    // for each element in the array that *arr_buff points to, write to the lcd, wait 5ms, then repeat
    for(int i = 0; i < size_arr; i++)
    {
        // concatenate to get the appropriate command 
        _append_command_prnt = ((arr_buff[i] << 2) | (RS_SET));
        EN = 1; 
        thread_sleep_for(5);
        lcd.write(_append_command_prnt);
        thread_sleep_for(5);
        EN = 0;  
    }
    return; 

} 

/*
Function: Move the cursor left or right by specifying the number of shifts. 
int shift = number of shifts & left_right = specify left or right shift (1 = left, 0 = right).
Usage/example: 
int main()
{
    _LCD_MOVE_CRSR_R(3, 1); // shifts the cursor 3 times to the left 
    _LCD_MOVE_CRSR_R(3, 0); // shifts the cursor 3 times to the right 

};

*/
void _LCD_MOVE_CRSR(int shift, int left_right) // may have to edit this 
{
    lcd.output(); 
    if(left_right == 1)
    {
        // shift left 
         for(int i = 0; i < shift; i++)
         {
            EN = 1; 
            thread_sleep_for(5); 
            lcd.write(0b0000100000); // shift cursor but not display
            thread_sleep_for(5);
            EN = 0;   
         }
         return; 

    } else {
    
    // shift right 
    for(int i = 0; i < shift; i++)
    {
       EN = 1; 
       thread_sleep_for(5); 
       lcd.write(0b0010100000); // shift cursor but not display
       thread_sleep_for(5);
       EN = 0;   
    }
    return; 
    }
}

/*
Function: Clears the entire LCD screen 
Usage/Example: 

int main()
{
    _LCD_CLEAR(); 
}

*/
void _LCD_CLEAR()
{
    lcd.output(); 
    EN = 1; 
    thread_sleep_for(5);
    lcd.write(CLEAR_LCD_DISPLAY);
    thread_sleep_for(5);
    EN = 0; 
    return;    
}

/*
Function: Load data for a custom character from an array to the desired CGRAM addr. 
Usage/Example: 
int main()
{
    uint8_t myCharData[8] = {myData};

    _LCD_LOAD_CSTM_CHAR(myCharData, 0); // load char data into CGRAM addr. 0 
}

*/
void _LCD_LOAD_CSTM_CHAR(uint8_t player_buff[], uint8_t CGRAM_addr)
{ 
    uint16_t concatenate_command_CGRAM;    
    
    // CGRAM 
    uint16_t CGRAM_addr_lut[8] = {0b0000000000001000, 0b0000000001001000, 0b0000000000101000, 0b0000000001101000, 0b0000000000011000, 0b0000000001011000, 0b0000000000111000, 0b0000000001111000}; // 0x00, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78  
    concatenate_command_CGRAM = CGRAM_addr_lut[CGRAM_addr]; 
    concatenate_command_CGRAM = (concatenate_command_CGRAM | RS_RESET);

    EN = 1; 
    thread_sleep_for(5);
    lcd.write(concatenate_command_CGRAM); 
    EN = 0; 

    for(int i = 0; i<8; i++)
    {
        EN = 1; 
        thread_sleep_for(5);
        lcd.write(((player_buff[i] << 2) | RS_SET)); 
        thread_sleep_for(5);
        EN = 0; 

    }

    return; 
}; 

/*
Function: Loads data from CGRAM address and write to desired DDRAM address to display custom char. 
Usage/Example: 
int main()
{
    _LCD_LOAD_CSTM_CHAR(myCharData, 0);
    _LCD_PRNT_CSTM_CHAR(1, 0); // CGRAM data at addr. 0 will be placed into DDRAM addr. 1 and displayed in tile #1  
}
*/
void _LCD_PRNT_CSTM_CHAR(uint8_t DDRAM_addr, uint8_t disp_addr)
{
    uint8_t rbit_lut[16] = { 0b00000000, 0b00001000, 0b00000100, 0b00001100, 0b00000010, 0b00001010, 0b00000110, 0b00001110, 0b00000001, 0b00001001, 0b00000101, 0b00001101, 0b00000011, 0b00001011, 0b00000111, 0b00001111
    };

    uint16_t concatenate_command_DDRAM;
    uint16_t concatenate_command_disp;

    // DDRAM
    DDRAM_addr = ((rbit_lut[(DDRAM_addr&0b00001111)] << 4) | rbit_lut[DDRAM_addr >> 4]); 
    concatenate_command_DDRAM = DDRAM_addr;  
    concatenate_command_DDRAM = (concatenate_command_DDRAM << 2); 
    concatenate_command_DDRAM = (concatenate_command_DDRAM | 0b0000000000000100);
    concatenate_command_DDRAM = (concatenate_command_DDRAM | RS_RESET);

    // set DDRAM addr 
    EN = 1; 
    thread_sleep_for(5); 
    lcd.write(concatenate_command_DDRAM);
    thread_sleep_for(5);
    EN = 0; 

    // disp 
    disp_addr = ((rbit_lut[(disp_addr&0b00001111)] << 4) | rbit_lut[disp_addr >> 4]); 
    concatenate_command_disp = disp_addr; 
    concatenate_command_disp = (concatenate_command_disp << 3);
    concatenate_command_disp = (concatenate_command_disp | RS_SET);  
    lcd.output(); 

    // Write to DDRAM  
    EN = 1; 
    thread_sleep_for(5);
    lcd.write(concatenate_command_disp); 
    thread_sleep_for(5);
    EN = 0; 

    return; 


}

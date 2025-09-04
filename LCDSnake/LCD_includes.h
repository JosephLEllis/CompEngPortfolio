#ifndef LCD_includes_H 
#define LCD_includes_H

#include <cstdint> // allows for uint8_t

// D9 = RS
// D10 = RW

// defined commands
#define BLANK                0b0000000000
#define CLEAR_LCD_DISPLAY    0b1000000000
#define RETURN_CURSOR_HOME   0b0100000000
#define SET_ENTRY_MODE       0b0010000000
#define DISPLAY_ON_OFF       0b0001000000
#define CURSOR_DISPLAY_SHIFT 0b0000100000
#define FUNCTION_SET         0b0000010000
#define SET_CGRAM_ADDR       0b0000001000
#define SET_DDRAM_ADDR       0b0000000100

// command flags 
#define LCD_RIGHT_ENTRY      0b0000000000
#define LCD_LEFT_ENTRY       0b0100000000
#define LCD_ENTRY_INCR       0b1000000000
#define LCD_ENTRY_DECR       0b0000000000

// flags display on/off
#define LCD_DISP_ON          0b0001000000
#define LCD_DISP_OFF         0b0000000000
#define LCD_CRSR_ON          0b0100000000
#define LCD_CRSR_OFF         0b0000000000
#define LCD_BLNK_ON          0b1000000000
#define LCD_BLNK_OFF         0b0000000000

// flags for cursor shift
#define MOVE_LCD             0b0001000000
#define LCD_CRSR_MOVE        0b0000000000
#define MOVE_LCD_RIGHT       0b0001000000
#define MOVE_LCD_LEFT        0b0000000000

// function set flags 
#define EIGHT_BIT_MODE       0b0000100000
#define FOUR_BIT_MODE        0b0000000000
#define TWO_LINE_LCD         0b0001000000
#define ONE_LINE_LCD         0b0000000000
#define LCD_5X10             0b0010000000
#define LCD_5X8              0b0000000000

// RS and RW defines; 
#define RS_SET               0b0000000010 
#define RS_RESET             0b0000000000
#define RW_SET               0b0000000001
#define RW_RESET             0b0000000000

// enable defines 
#define EN_SET               0b0000000010
#define EN_RESET             0b0000000000

// bulk commands: 
#define START                0b0001110000 
#define LCD_BLNK_SHRT        0b1111000000 
#define DEFAULT_ENTRY        0b0110000000

// declare function prototypes: 

void _INITIALISE_LCD(int _mode); // initialise the LCD

void _LCD_WRITE_CHAR(uint8_t letter); // print a character to the screen 

void _LCD_PRINT_STR(uint8_t arr_buff[], int size_arr); // include pointer to an array - leave as void for now

void _LCD_LOAD_CSTM_CHAR(uint8_t player_buff[], uint8_t CGRAM_addr); // print a custom character uint8_t player_buff[], int size_pb

void _LCD_PRNT_CSTM_CHAR(uint8_t DDRAM_addr, uint8_t disp_addr);

void _LCD_MOVE_CRSR(int shift, int left_right); // 1 = left, 0 = right  

void _LCD_CLEAR(void); // clear and reset the LCD screen

#endif

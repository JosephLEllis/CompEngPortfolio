#include "mbed.h"
#include "LCD_includes.h"
#include <cstdint>
#include <cstdlib>

// ROM SECTION:

// snake struct: 
struct snake_ROM_data {

// snake ROM data right:
uint8_t snake_frame_1_fs[8] = { // only first 5 bits correspond to the row data. This is the first segment 
    0b00000000, 0b00000000, 0b11100000, 0b11100000, 0b00101000, 0b00101000, 0b00101000, 0b00111000
}; 

uint8_t snake_frame_1_ss[8] = {
    0b00000000,  0b00000000,  0b00000000,  0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b11110000 
};

// snake ROM data left: 
uint8_t snake_frame_1_fs_left[8] = { 
    0b00000000, 0b00000000, 0b00111000, 0b10111000, 0b10100000, 0b10100000, 0b10100000, 0b11100000
 };

uint8_t snake_frame_1_ss_left[8] = {
    0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b11111000
};

// snake ROM data up: 
uint8_t snake_frame_1_fs_up[8] = {
  0b01110000, 0b01110000, 0b01110000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000
};

uint8_t snake_frame_1_ss_up[8] = {
  0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000
};

// snake ROM data down: 
uint8_t snake_frame_1_fs_down[8] = {
  0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b01110000, 0b01110000, 0b01110000
};

uint8_t snake_frame_1_ss_down[8] = {
  0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000
};
};

struct snake_title_screen {

// snake title screen font ROM data: 
uint8_t customS[8] = { 0b11111000, 0b11111000, 0b00001000, 0b11111000, 0b10000000, 0b10000000, 0b11111000, 0b11111000
};

uint8_t customN[8] = { 0b10011000, 0b10011000, 0b10011000, 0b10101000, 0b11001000, 0b11001000, 0b11001000, 0b11001000
};

uint8_t customA[8] = { 0b11111000, 0b10001000, 0b10001000, 0b11111000, 0b11011000, 0b11011000, 0b11011000, 0b11011000
};

uint8_t customK[8] = { 0b11011000, 0b11011000, 0b01011000, 0b00111000, 0b00111000, 0b01011000, 0b11011000, 0b11011000
};

uint8_t customE[8] = { 0b11111000, 0b11111000, 0b00011000, 0b11111000, 0b11111000, 0b00011000, 0b11111000, 0b11111000
};

// Title screen strings: 
uint8_t Start[5] = { 0b11001010, 0b00101010, 0b10000010, 0b01001010, 0b00101010 
};

uint8_t Blank[5] = { 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000
}; 
};

struct game_over_ROM_data {

// Game over letters: 
uint8_t Game_over_G[8] = { 0b11111000, 0b11111000, 0b00001000, 0b11101000, 0b11101000, 0b10001000, 0b11111000, 0b11111000
};

uint8_t Game_over_M[8] = { 0b10001000, 0b11001000, 0b11011000, 0b10101000, 0b10001000, 0b11011000, 0b11011000, 0b11011000
};

uint8_t Game_over_O[8] = { 0b11111000, 0b11111000, 0b11011000, 0b11011000, 0b11011000, 0b11011000, 0b11111000, 0b11111000
}; 

uint8_t Game_over_V[8] = { 0b11011000, 0b11011000, 0b11011000, 0b11011000, 0b10001000, 0b10001000, 0b01010000, 0b00100000
};

uint8_t Game_over_R[8] = { 0b11111000, 0b10001000, 0b11111000, 0b11111000, 0b00101000, 0b01001000, 0b11001000, 0b11001000
}; 

uint8_t Game_over_blank[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000
};

};

// Apple ROM data: 
uint8_t apple_data[8] = {
    0b00110000, 0b00100000, 0b01110000, 0b11101000, 0b11111000, 0b11111000, 0b11111000, 0b01110000
}; 

// VARIABLE SECTION: 

// declare instances of structs, access structs using the dot operatpr ".": 
snake_ROM_data snake; 
snake_title_screen snake_title;
game_over_ROM_data gameOver_ROM;  

// Menu Timer:
int menu_Timer = 0; 

// pseudo-random vars: 
uint8_t seed = 5; 

// Declare button: 
DigitalIn myButton(D11);

// global vars: 
bool startedGame = false; 
bool hasCollided = false; 
bool applePlaced = false; 
uint8_t direction = 0; // 0 = right, 1 = left, 2 = up, 3 = down;
uint8_t snake_x = 0; 
uint8_t snake_y = 0; 
uint8_t snake_ss_pos = 0;
uint8_t apple_x = 0; 
uint8_t apple_y = 0;  
uint8_t score_digit_1 = 0b00001100; // keep the upper nibble const.
uint8_t score_digit_2 = 0b00001100;  
uint8_t score_counter_digit_1 = 0; 
uint8_t score_counter_digit_2 = 0; 

// score table: 
uint8_t score_digits_LCD[10] = {
    0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000, 0b00010000, 0b10010000
};

// GAME SECTION: 

// setup function prototypes 
void initRAM(); 
void placeApple(); 
void menuCutscene(); 
void menu(); 
void initGame(); 
void checkDirStat(); 
void moveRight(); 
void moveLeft(); 
void moveUp(); 
void moveDown(); 
void moveSnake(uint8_t direction_mode); 
void checkCollision(); 
void gameOver(); 

// initialise RAM and setup sprites for start 
void initRAM()
{
    _INITIALISE_LCD(8); 
    _LCD_CLEAR(); // clear LCD screen 

    // Load snake sprite data into RAM
   _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss, 0);
   _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs, 1);
   _LCD_LOAD_CSTM_CHAR(snake_title.customS, 2); 
   _LCD_LOAD_CSTM_CHAR(snake_title.customN, 3); 
   _LCD_LOAD_CSTM_CHAR(snake_title.customA, 4);
   _LCD_LOAD_CSTM_CHAR(snake_title.customK, 5);
   _LCD_LOAD_CSTM_CHAR(snake_title.customE, 6); 
   return; 
};

// Main menu cutscene: 
void menuCutscene()
{
    menu_Timer = 0; 

    _LCD_CLEAR(); 

    // slowly print snake on screen: 
    _LCD_PRNT_CSTM_CHAR(5, 4); 
    thread_sleep_for(800); 
    _LCD_PRNT_CSTM_CHAR(6, 6);
    thread_sleep_for(800); 
    _LCD_PRNT_CSTM_CHAR(7, 8);
    thread_sleep_for(800); 
    _LCD_PRNT_CSTM_CHAR(8, 10);
    thread_sleep_for(800); 
    _LCD_PRNT_CSTM_CHAR(9, 12);
    thread_sleep_for(800);  
    
    menu(); 

}

// Main start menu:  
void menu()
{
    _LCD_CLEAR(); 
    // display title "snake":
    _LCD_PRNT_CSTM_CHAR(5, 4); 
    _LCD_PRNT_CSTM_CHAR(6, 6);
    _LCD_PRNT_CSTM_CHAR(7, 8);
    _LCD_PRNT_CSTM_CHAR(8, 10);
    _LCD_PRNT_CSTM_CHAR(9, 12); 
    
    // "Press button to start":
    _LCD_MOVE_CRSR(35, 0);
    while(myButton != 1)
    {
        if(menu_Timer == 20)
        {
            menuCutscene(); 

        } else {
            _LCD_PRINT_STR(snake_title.Start, 5);
            _LCD_MOVE_CRSR(5, 1);
            thread_sleep_for(700); 
            _LCD_PRINT_STR(snake_title.Blank, 5);
            thread_sleep_for(700); 
            _LCD_MOVE_CRSR(5, 1);
            menu_Timer+=1;  
        }
    }
    return; 
 
}; 

void checkDirStat()
{
    if(startedGame == false)
    {
        direction = 0; 
        startedGame = true; 

    } else if((direction >= 3) && (myButton == 1))
    {
        direction = 0; 

    } else if(myButton == 1)
    {  
        direction += 1; 
    } 

};

void moveRight()
{
    _LCD_CLEAR(); 
        snake_ss_pos += 1; 
         // update snake x: 
         snake_x += 1; 
        _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss, 0);
        _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs, 1);  
        _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)-1), 0); // tail 
        _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head  
        return; 
};

void moveLeft()
{
    _LCD_CLEAR();
        snake_ss_pos -= 1; 
        // update snake x: 
        snake_x -= 1; 
        _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss_left, 0);
        _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs_left, 1);  
        _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head 
        _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)+1), 0); // tail
        return; 

};

void moveUp()
{
    _LCD_CLEAR(); 
        if(snake_y == 0)
        { 
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss_up, 0);
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs_up, 1); 
            _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head 
            _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)+64), 0); // tail  
            return; 

        } else {
            snake_ss_pos -= 64; 
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss_up, 0);
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs_up, 1); 
            _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head 
            _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)+64), 0); // tail  
            // udpate snake y: 
            snake_y = 0;
            return; 
        }

};

void moveDown()
{
    _LCD_CLEAR(); 
        // update snake y: 
        if(snake_y == 1)
        {
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs_down, 0);
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss, 1);  
            _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head
            _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)-64), 0); // tail
            return; 

        } else {
            snake_ss_pos += 64; 
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_fs_down, 0);
            _LCD_LOAD_CSTM_CHAR(snake.snake_frame_1_ss, 1);  
            _LCD_PRNT_CSTM_CHAR((snake_ss_pos), 2); // head
            _LCD_PRNT_CSTM_CHAR(((snake_ss_pos)-64), 0); // tail 
            snake_y = 1;
            return;  
        }

}

void moveSnake(uint8_t direction_mode)
{
    if(direction_mode == 0)
    {
        moveRight(); 
        //direction = 0; 
        return; 

    } else if(direction_mode == 1)
    {
        moveLeft();
        //direction = 1; 
        return;  
    } else if(direction_mode == 2)
    {
        moveUp(); 
        moveLeft();  
        return; 

    } else if(direction_mode == 3)
    {
        moveDown();
        moveRight(); 
        return; 

    }

};

void checkCollision()
{
    // check for collision with apple: 
    if(((snake_x == apple_x) && (snake_y == apple_y)))
    {
        applePlaced = false; 
        
        // check or increment score: 
        if(score_counter_digit_1 == 9)
        {
            score_counter_digit_1 = 0; 
            score_counter_digit_2 = 1; 
        } else if((score_counter_digit_1 & score_counter_digit_2) == 9)
        {
            gameOver(); 
        } else {
            score_counter_digit_1+=1; 
        }
    }  

    // check for collision with the edge of the screen
    if((snake_x > 15))
    {
        gameOver(); 
    } 

    return; 
}; 

void placeApple()
{
    applePlaced = true; 
    int temp; 
    // use xorshift to generate a pseudo-random number 
    seed ^= (seed << 7);
    seed ^= (seed >> 5);
    seed ^= (seed << 3);
    temp = seed;  
    // mask lower nibble for x and y:
    apple_x = (temp & 0b00001111); 
    apple_y = (temp & 0b00000001); 
    
    if(apple_y == 0)
    {
        _LCD_PRNT_CSTM_CHAR((apple_x), 4);
    } else {
        _LCD_PRNT_CSTM_CHAR(((apple_x)+40), 4);
    }
    
    return; 
};

void initGame()
{
    // clear LCD and load character data in: 
    _LCD_CLEAR(); 

    _LCD_LOAD_CSTM_CHAR(apple_data, 2); 
    
    _LCD_PRNT_CSTM_CHAR(0, 0);  
    
    _LCD_PRNT_CSTM_CHAR(1, 2); 
    
    return; 
}; 


// main() runs in its own thread in the OS
int main()
{  
    // setup:  
    initRAM(); 
    menu(); 
    initGame();  
    
    // main game loop: 
    while(hasCollided == false)
    { 
        checkDirStat(); 
        moveSnake(direction);  
        checkCollision(); 
        if(applePlaced == false)
        {
            placeApple(); 
        } else {
            _LCD_PRNT_CSTM_CHAR((apple_x), 4);
            thread_sleep_for(500); 
        }
    } 
    while (true) {
    

    }
}

void gameOver()
{
    _LCD_CLEAR(); 
    snake_ss_pos = 0; // reset pos 
    direction = 0; 
    snake_x = 0; 
    snake_y = 0; 
    apple_x = 0; 
    apple_y = 0; 
    startedGame = false;

    // load game over screen data:
    _LCD_LOAD_CSTM_CHAR(gameOver_ROM.Game_over_G, 0);
    _LCD_LOAD_CSTM_CHAR(snake_title.customA, 1); 
    _LCD_LOAD_CSTM_CHAR(gameOver_ROM.Game_over_M, 2); 
    _LCD_LOAD_CSTM_CHAR(snake_title.customE, 3); 
    _LCD_LOAD_CSTM_CHAR(gameOver_ROM.Game_over_O, 4); 
    _LCD_LOAD_CSTM_CHAR(gameOver_ROM.Game_over_V, 5); 
    _LCD_LOAD_CSTM_CHAR(snake_title.customE, 6);
    _LCD_LOAD_CSTM_CHAR(gameOver_ROM.Game_over_R, 7); 

    // print game over screen data:
    _LCD_PRNT_CSTM_CHAR(3, 0); // G
    _LCD_PRNT_CSTM_CHAR(4, 2); // A
    _LCD_PRNT_CSTM_CHAR(5, 4); // M
    _LCD_PRNT_CSTM_CHAR(6, 6); // E
    _LCD_MOVE_CRSR(1, 0);
    _LCD_PRNT_CSTM_CHAR(8, 8);  // O
    _LCD_PRNT_CSTM_CHAR(9, 10);  // V
    _LCD_PRNT_CSTM_CHAR(10, 12);  // E
    _LCD_PRNT_CSTM_CHAR(11, 14);  // R

    // display score (max 99):
    _LCD_MOVE_CRSR(36, 0);
    _LCD_WRITE_CHAR((score_digit_1 | score_digits_LCD[score_counter_digit_1]));
    _LCD_MOVE_CRSR(2, 1);
    _LCD_WRITE_CHAR((score_digit_2 | score_digits_LCD[score_counter_digit_2])); 

    // reset score:  
    score_counter_digit_1 = 0;
    score_counter_digit_2 = 0; 
     
    thread_sleep_for(3000); 
    main(); // restart and re-initialize RAM
}


#ifndef TFLuna_H 
#define TFLuna_H

#include <mbed.h>

//using namespace mbed; 
using mbed::I2C;

// register definitions: 

// (include all register definitions later)

#define SLAVE_ADDR (0x10 << 1) 
#define DIST_LOW 0x00      
#define DIST_HIGH 0x01

const char MUX_CMD = 0x01; // default MUX addr
const char MUX_ADDR = 0xEE;

static bool AVAILABLE_IR_SLOTS[4] = {true, true, true, true};

static const char MUX_ADDR_LINES[4] = {0x01, 0x02, 0x04, 0x08};

class TFLuna{
    
	public: 
	  
	  TFLuna(PinName SDA, PinName SCL); 
	  
	  int getDistance();

      void setupPort(int port_choice); 

      void switchPort(int port_num);
	  
	  void read(int slave_addr_read, char *data_arr, int data_read_length); 
	  
	  void write(int slave_addr_write, char *cmd_arr, int data_write_length); 
	  
	  void calculateDistance();	  
	  
	  void DEBUG(); 
	  
	private: 
	  
	  I2C i2c; 
	  
	  int distance; 
	
};























#endif
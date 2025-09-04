#include "TFLuna.h"

TFLuna::TFLuna(PinName SDA, PinName SCL) : i2c(SDA, SCL) {
	
	distance = 0; 
};

/**
* Return the distance from the TFLuna sensor. 
* @param NONE 
*
* Code example: 
* TFLuna LIDAR(P0_31, P0_2);
*
* LIDAR.calculateDistance();
* 
* Serial.println(LIDAR.getDistance());
*/

int TFLuna::getDistance(){
	
	return distance;  
};

/** 
 * Set up an i2c port by selecting your port choice. Port 0 is treated as port choice 1, e.g. gyro.setUpPort(1); is the first i2c port on the board. 
 * If a zero is given, the next available i2c port will be chosen.
 * 
 * @param port_choice: the port choice (1-4)
 * 
 * Code example: 
 * TFLuna LIDAR(P0_31, P0_2);
 * 
 * LIDAR.setUpPort(1);
 * **/

void TFLuna::setupPort(int port_choice){
	
	switch(port_choice){
		
		case NULL:
          
        for(int i = 0; i < (sizeof(AVAILABLE_IR_SLOTS)/sizeof(AVAILABLE_IR_SLOTS[0])); i++){
	        
			if(AVAILABLE_IR_SLOTS[i] == true){
			     i2c.write(MUX_ADDR, &MUX_ADDR_LINES[i], 1); 
				 Serial.println("NULL ADDR SELECTED");
                 AVAILABLE_IR_SLOTS[i] = false;
                 break; 				 
	 	     }		  
		
		}
		
		break; 
		
		case 1: 
		
		i2c.write(MUX_ADDR, &MUX_ADDR_LINES[0], 1);
        AVAILABLE_IR_SLOTS[0] = false; 		
		
		break; 
		
		case 2: 
		
		i2c.write(MUX_ADDR, &MUX_ADDR_LINES[1], 1);
		AVAILABLE_IR_SLOTS[1] = false; 

        break; 

        case 3:
   
        i2c.write(MUX_ADDR, &MUX_ADDR_LINES[2], 1);
		AVAILABLE_IR_SLOTS[2] = false; 

        break; 

        case 4: 
   
        i2c.write(MUX_ADDR, &MUX_ADDR_LINES[3], 1);
		AVAILABLE_IR_SLOTS[3] = false; 

        break; 		
		
	}
	
	return; 
};

/** 
 * Switch to a desired i2c port.  
 * 
 * @param port_choice: the port choice (1-4)
 * 
 * Code example: 
 * TFLuna LIDAR(P0_31, P0_2);
 *
 * LIDAR.setUpPort(1);
 * 
 * LIDAR.switchPort(2);
 * 
 * **/

void TFLuna::switchPort(int port_num){
	
	switch(port_num){

    case 1: 
    
	i2c.write(MUX_ADDR, &MUX_ADDR_LINES[0], 1); 

	break; 

	case 2: 

	i2c.write(MUX_ADDR, &MUX_ADDR_LINES[1], 1); 

	break; 

    case 3: 

	i2c.write(MUX_ADDR, &MUX_ADDR_LINES[2], 1); 

	break; 

	case 4: 

	i2c.write(MUX_ADDR, &MUX_ADDR_LINES[3], 1); 

	break; 

	}
	
};

/** 
 * Read data from an i2c port. You must write to the i2c device before reading the contents of a register.
 * 
 * @param slave_addr_read: The slave address of the device you want to read from
 * @param *data_arr: Pointer to a char data array. Data read from the i2c device will be deposited in the array. 
 * @param data_read_length: How much data to read from the given starting point provided by *data_arr
 * 
 * Code example: 
 * TFLuna LIDAR(P0_31, P0_2);
 *
 * LIDAR.setUpPort(1);
 * LIDAR.write(SLAVE_ADDR, cmd, 1);
 * LIDAR.read(SLAVE_ADDR, data, 1); // read 1 byte 
 * **/
void TFLuna::read(int slave_addr_read, char *data_arr, int data_read_length){
	
	i2c.read(slave_addr_read, data_arr, data_read_length);
	
	return; 
};

/** 
 * Write data from an i2c port. 
 * 
 * @param slave_addr_read: The slave address of the device you want to read from
 * @param *cmd_arr: Pointer to char array that contains a command. This could be the address of a register. 
 * @param data_read_length: How many bytes to write to the address specified by *cmd_arr. 
 * 
 * Code example: 
 * TFLuna LIDAR(P0_31, P0_2);
 * 
 * LIDAR.setUpPort(1);
 * LIDAR.write(SLAVE_ADDR, cmd, 1);
 * **/
 
void TFLuna::write(int slave_addr_write, char *cmd_arr, int data_write_length){
	
	i2c.write(slave_addr_write, cmd_arr, data_write_length); 
	
	return;
};

/** 
 * Write data from an i2c port. 
 * 
 * @param NONE 
 *
 * Code example: 
 * TFLuna LIDAR(P0_31, P0_2);
 * 
 * LIDAR.setUpPort(1);
 * LIDAR.calculateDistance();
 * **/

void TFLuna::calculateDistance(){

    char low_byte[1]; 
    char high_byte[1];

    char dist_low[1] = {DIST_LOW}; 
    char dist_high[1] = {DIST_HIGH};

    i2c.write(SLAVE_ADDR, dist_low, 1, true);

    i2c.read(SLAVE_ADDR, low_byte, 1); 

    i2c.write(SLAVE_ADDR, dist_high, 1, true);

    i2c.read(SLAVE_ADDR, high_byte, 1); 
	
	// size calc arr for use at a later time 
	distance = ((high_byte[0] << 8) | low_byte[0]);
	
	return; 
};

void TFLuna::DEBUG(){
	
	Serial.println("Distance from IR sensor is: ");
    Serial.println(this->getDistance()); 	
	
	Serial.println("Is port 1 available?: ");
	Serial.println(AVAILABLE_IR_SLOTS[0]);
	
	Serial.println("Is port 2 available?: ");
	Serial.println(AVAILABLE_IR_SLOTS[1]);
	
	Serial.println("Is port 3 available?: ");
	Serial.println(AVAILABLE_IR_SLOTS[2]);
	
	Serial.println("Is port 4 available?: ");
	Serial.println(AVAILABLE_IR_SLOTS[3]);
 	
};
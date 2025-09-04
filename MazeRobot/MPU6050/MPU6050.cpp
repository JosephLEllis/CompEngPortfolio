#include "MPU6050.h"

MPU6050::MPU6050(PinName SDA, PinName SCL) : i2c(SDA, SCL){
};

/** 
 * Set up an i2c port by selecting your port choice. Port 0 is treated as port choice 1, e.g. gyro.setUpPort(1); is the first i2c port on the board. 
 * If a zero is given, the next available i2c port will be chosen.
 * 
 * @param port_choice: the port choice (1-4)
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * 
 * gyro.setUpPort(1);
 * **/

void MPU6050::setupPort(int port_choice){
	
	switch(port_choice){
		
		case NULL:
          
        for(int i = 0; i < (sizeof(AVAILABLE_IR_SLOTS_)/sizeof(AVAILABLE_IR_SLOTS_[0])); i++){
	        
			if(AVAILABLE_IR_SLOTS_[i] == true){
			     i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[i], 1); 
				 Serial.println("NULL ADDR SELECTED");
                 AVAILABLE_IR_SLOTS_[i] = false;
                 break; 				 
	 	     }		  
		
		}
		
		break; 
		
		case 1: 
		
		i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[0], 1);
        AVAILABLE_IR_SLOTS_[0] = false; 		
		
		break; 
		
		case 2: 
		
		i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[1], 1);
		AVAILABLE_IR_SLOTS_[1] = false; 

        break; 

        case 3:
   
        i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[2], 1);
		AVAILABLE_IR_SLOTS_[2] = false; 

        break; 

        case 4: 
   
        i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[3], 1);
		AVAILABLE_IR_SLOTS_[3] = false; 

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
 * MPU6050 gyro(P0_31, P0_2);
 * MPU6050 gyro2(P0_31, P0_2);
 * 
 * gyro.setUpPort(1);
 * gyro2.setUpPort(2);
 * 
 * Serial.println("Gyro z is: ");
 * Serial.println(gyro.getZ());
 * 
 * gyro.switchPort(2);
 * Serial.println("Gyro 2 z is: ");
 * Serial.println(gyro2.getZ());
 * 
 * **/

void MPU6050::switchPort(int port_num){
	
	switch(port_num){

    case 1: 
    
	i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[0], 1); 

	break; 

	case 2: 

	i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[1], 1); 

	break; 

    case 3: 

	i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[2], 1); 

	break; 

	case 4: 

	i2c.write(MUX_ADDR_, &MUX_ADDR_LINES_[3], 1); 

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
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * data[2] = {0x00, 0x00};
 * 
 * gyro.setUpPort(1);
 * gyro.write(SLAVE_ADDR, cmd, 1);
 * gyro.read(SLAVE_ADDR, data, 1); // read 1 byte 
 * **/

void MPU6050::read(int slave_addr_read, char *data_arr, int data_read_length){
	
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
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * gyro.write(SLAVE_ADDR, cmd, 1);
 * **/

void MPU6050::write(int slave_addr_write, char *cmd_arr, int data_write_length){
	
	i2c.write(slave_addr_write, cmd_arr, data_write_length); 
	
	return;
};

/** 
 * Read data from the gyro. This will deposit the results in x, y, and z variables. 
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.write(SLAVE_ADDR, cmd, 1);
 * gyro.read(SLAVE_ADDR, data, 1); // read 1 byte 
 * **/

void MPU6050::readRawDataGyro(){

    char data[6];

    char cmd[2] = {GYRO_XOUT_H, 0x00};

    i2c.write(SLAVE_ADDR, cmd, 1);

    i2c.read(SLAVE_ADDR, data, 6);

    x = (int16_t)(data[0] << 8 | data[1]);
    y = (int16_t)(data[2] << 8 | data[3]);
    z = (int16_t)(data[4] << 8 | data[5]);

    return; 
};

/** 
 * Set gyro X to a value. 
 * 
 * @param x: value to set gyro x value to 
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.setX(0);
 * **/

void MPU6050::setX(int16_t x){

    char cmd[2] = {GYRO_XOUT_H, x}; 

    i2c.write(SLAVE_ADDR, cmd, 2);
   
   return; 
};

/** 
 * Set gyro Y to a value. 
 * 
 * @param y: value to set gyro x value to 
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.setY(0);
 * **/

void MPU6050::setY(int16_t y){

    char cmd[2] = {GYRO_YOUT_H, y}; 

    i2c.write(SLAVE_ADDR, cmd, 2);
   
   return; 
};


/** 
 * Set gyro Z to a value. 
 * 
 * @param y: value to set gyro x value to 
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.setZ(0);
 * **/

void MPU6050::setZ(int16_t z){

    char cmd[2] = {GYRO_YOUT_H, z}; 

    i2c.write(SLAVE_ADDR, cmd, 2);
   
   return; 
};

/** 
 * Initialise the gyro for usage.
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.initialise(); // set up the gyro
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.write(SLAVE_ADDR, cmd, 1);
 * gyro.read(SLAVE_ADDR, data, 1); // read 1 byte 
 * **/

void MPU6050::initialise(){

    // turn off sleep mode: 
    char cmd[2] = {PWR_MGMT_1, 0x00};
    
    char config[2] = {GYRO_CONFIG, 0x00};

    i2c.write(SLAVE_ADDR, cmd, 2);

    delay(1);

    i2c.write(SLAVE_ADDR, config, 2);

    return; 
};

/** 
 * Return the sensitivity of the gyro
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.initialise(); 
 * 
 * Serial.println(gyro.getSensitivity());
 * **/

int MPU6050::getSensitivity(){
    
    char config[1] = {0x00};

    i2c.read(SLAVE_ADDR, config, 1);

    return config[1]; 
};

/** 
 * Return the value of x.
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.getX();
 * **/

int MPU6050::getX(){

    return x; 
};

/** 
 * Return the value of y.
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.getY();
 * **/

int MPU6050::getY(){

    return y; 
};

/** 
 * Return the value of z.
 * 
 * @param NONE
 * 
 * Code example: 
 * MPU6050 gyro(P0_31, P0_2);
 * char cmd[2] = {GYRO_XOUT_H, 0x00};
 * 
 * gyro.setUpPort(1);
 * 
 * gyro.readRawDataGyro();
 * gyro.getZ();
 * **/

int MPU6050::getZ(){

    return z; 
};


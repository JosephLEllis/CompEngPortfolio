//#include <ctype.h>
#include "HCSR04.h"

/**
* HCSR04 constructor for HCSR04 class. 
* This constructor takes a trigger pin (defined in MBed include PinNames.h) or alternatively any available IO pin  
*
* @param trig_pin: trigger pin for HCSR04 (WRITE ONLY) 
* @param echo_pin: echo pin for HCSR04 (READ ONLY)
* @param interrupt_pin: interrupt pin 
* 
* Code example: 
* (Define in main arduino sketch (.ino file)): 
* HCSR04 sonar(P0_23, P0_3, P0_3);
*/
HCSR04::HCSR04(PinName trig_pin, PinName echo_pin, PinName interrupt_pin) : TRIG(trig_pin), ECHO(echo_pin), INTERRUPT(interrupt_pin){
	
	// initialize static vars: 
	distance = 0;
	recorded_time = 0;
	time_arr_ptr = 0;
	
	averageTimerReading = 0; 
	totalTime = 0;
	timeSampleSize = 0; 
	sample_size = 0;
	
};

/**
* Select the default ISR for HCSR04 class. Use this option if you do not want to specify custom ISRs. 
* Each time a pulse is detected on the specified echo pin (and the interrupt is assigned to the interrupt pin) 
* calculateDistance() will be called. 
*
* @param : NONE 
*
* Code example: 
* HCSR04 sonar(P0_23, P0_3,P0_3);
* sonar.defaultInterrupts();  
*/
void HCSR04::defaultInterrupts(){
	
    INTERRUPT.rise(callback(this, &HCSR04::calculateDistance));	
	INTERRUPT.fall(NULL);
	
	return; 
};

/**
* Specify and set up custom interrupts to be called instead of default ISR calculateDistance() 
* The addr. of each ISR passed to setUpCustomInterrupts(); is pushed onto ISR_addr_stack (space allocated on the heap)
* for later retrieval should interrupts be disabled or re-enabled 
*
* If you do not wish to pass a function pointer for specific use, pass NULL
*
* @param void(*ISR_RISING)(): Pointer to void function to be called on rising edge of signal (if detected) on echo pin
* @param void(*ISR_FALLING)(): Pointer to void function to be called on falling edge of signal (if detected) on echo 
* pin
*
* Code example: 
*/
void HCSR04::setUpCustomInterrupts(void (*ISR_RISING)(), void (*ISR_FALLING)()){
	
	 INTERRUPT.rise(ISR_RISING); // pointer contains memory address of function for rising ISR etc...
     INTERRUPT.fall(ISR_FALLING);
	 
	 // push ISR to stack: 
	 if(this->ISR_addr_stack_ptr != 1){
		 ISR_addr_stack[0] = ISR_RISING; 
		 ISR_addr_stack[1] = ISR_FALLING;
		 ISR_addr_stack_ptr = 1; 
	 } else {
		 // check if addresses match, if they don't, then they are new custom ISRs that must be pushed onto stack.
		 return;
	 }

     return; 	
};

/**
* Disable all rising and falling interrupts listening on the echo pin.
*
* @param ; NONE 
*
* Code example: 
* HCSR04 sonar(P0_23, P0_3, P0_3); 
* sonar.defaultInterrupts();
* sonar.disableInterrupts();
*/
void HCSR04::disableInterrupts(){
	INTERRUPT.rise(NULL);
    INTERRUPT.fall(NULL);
	
	return; 
	
};

/**
* Send a 10us pulse to the trigger pin to initiate HCSR04 operation
*
* @param : NONE 
*
* Code example: 
*
* HCSR04 sonar(P0_23, P0_3, P0_3);
* sonar.defaultInterrupts(); 
* sonar.pulseOut();  
*/
void HCSR04::pulseOut(){
	
	TRIG = 0; 
	
	wait_us(10);
	
	TRIG = 1; 
	
	wait_us(10);
	
	TRIG = 0;
	
	return; 
	
 };

 /**
* Set the sample size to collect from the sensor at each interrupt instance. During default interrupt operation, a *certain number of samples are collected and stored for later operations. 
*
* @param sample_size (int): Sample size
*
* Code example:  
* HCSR04 sonar(P0_23, P0_3, P0_3);
* sonar.defaultInterrupts(); 
* sonar.setSampleSize(3); 
* sonar.pulseOut();  
* 
*/
void HCSR04::setSampleSize(int sample_size){
	
			sample_size = sample_size; 
		
}; 


int HCSR04::uSecToCM(int uSec){
	// Speed of sound is approx. 340 m/s or 29 uSec per cm
	// The length of the pulse is the there and back travel time,
	// so we take half of the distance travelled
	return uSec / 29 / 2;
}

/**
* Calculate the distance from HCSR04 data. Note that this function need not be called directly and is automatically * called by the interrupt pin at the appropriate time (either rising or falling edge).  
*
* @param : NONE 
*
* Code example: 
* HCSR04 sonar(P0_23, P0_3, P0_3);
* sonar.defaultInterrupts(); 
* sonar.setSampleSize(3); 
* sonar.pulseOut(); 
*/ 
void HCSR04::calculateDistance(){ 
		
	   TIMER.reset(); 
	   TIMER.start();
	
	   while(ECHO == 1){}
	
	   TIMER.stop();
	   
	   recorded_time = TIMER.elapsed_time().count();
	  
	
	   distance = uSecToCM(recorded_time);

	return; 
	
};

/**
* Start the timer for the specific instance of the HCSR04 class 
*
* @param : NONE 
*
* Code example: 
*/
void HCSR04::startTimer(){
	
	TIMER.start(); 
	
	return; 
};

/**
* Stop the timer for the specific instance of the HCSR04 class 
*
* @param : NONE 
*
* Code example: 
*/
void HCSR04::stopTimer(){
	
	TIMER.stop();
	
	return; 
}; 

/**
* Reset the timer for the specific instance of the HCSR04 class 
*
* @param : NONE 
*
* Code example: 
*/
void HCSR04::resetTimer(){
	
	TIMER.reset(); 
	
	return; 
};

int HCSR04::getDistance(){
	
	delay(5); // wait for distance to be updated
	
	return distance; 
}

/**
*
* Debug function for HCSR04 class for distance. 
*
* @param : NONE 
* 
* Code example: 
* HCSR04 sonar(P0_23, P_03, P_03);
* sonar.calculateDistance(); 
* sonar.DEBUG();
*/
void HCSR04::DEBUG(){
	
	Serial.println("Distance is: ");
	Serial.println(distance);
}; 

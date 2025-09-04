#ifndef HCSR04_H
#define HCSR04_H

#include <mbed.h>

using namespace mbed; 

/**  
*   Class explanation + example go here: 
*   note -> use doxygen to generate docs at some point 
*   
*
*
*
*
*/ 
class HCSR04 {
	
	public: 
	 
	 /// class constructor declaration
	 HCSR04(PinName trig_pin, PinName echo_pin, PinName interrupt_pin);  

	 /// calculate the distance 
	 void calculateDistance(); 
	 
	 /// set default interrupts (apply if no specific interrupt conditions needed)
	 void defaultInterrupts();  	
	 
      /// custom interrupt setup	  
     void setUpCustomInterrupts(void (*ISR_RISING)(), void (*ISR_FALLING)());  
     
	 /// completely disable interrupts on the pins associated with the HCSR04 sensor 
     void disableInterrupts(); 
     
	 /// send out a 10us pulse on the trigger pin 
     void pulseOut(); 
	 
	 /// Set the sample size for storage by the sensor
	 void setSampleSize(int sample_size);
	 
     /// Start the echo pulse timer 
     void startTimer(); 
     
	 /// Stop the echo pulse timer
     void stopTimer();  
     
	 /// Reset echo pulse the timer 
     void resetTimer(); 
	 
	 // Get the distance
	 int getDistance();

	 int uSecToCM(int uSec);
	 
	 /// Use the serial interface to print out the inputs and outputs of a specific HCSR04 instance
	 void DEBUG();  	 
    
	private: 
	
	 DigitalOut TRIG; 
	 DigitalIn ECHO;
	 
	 Timer TIMER; 
	 
	 InterruptIn INTERRUPT;
	 
	 void (*ISR_addr_stack[1])(); // array of function pointers of type void 	 
	 
	 /// Raw distance obtained by the specific HCSR04 instance 
	 volatile int distance; 
	 
	 // Holding variable to hold written value from ISR
	 volatile int temp_distance; 
	 
	 /// Raw recorded time of echo pulse by the specific HCSR04 instance 
     volatile int recorded_time;
	  
	 /// Tracks the current empty position in the time data array 
	 volatile int time_arr_ptr;
	 
	 /// Tracks the current empty position in the ISR stack 
	 volatile int ISR_addr_stack_ptr; 
	 
	 /// Average timer reading (calculated from time data array)
	 volatile int averageTimerReading; 
	 
	 /// Total time (calculated from time data array)
	 volatile int totalTime;
	 
	 volatile int sample_size;
	 
	 /// Sample size set by the user to specify how many samples should be stored in the time data array 
	 /// before it is deleted 
	 volatile int timeSampleSize;
	
}; 

#endif
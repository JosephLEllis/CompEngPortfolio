#ifndef TB6612FNG_H 
#define TB6612FNG_H

#include <mbed.h>

using mbed::DigitalOut;
using mbed::DigitalIn; 
using mbed::PwmOut;
using mbed::InterruptIn; 
using mbed::Ticker;
using mbed::callback;

using rtos::Mutex; 

using namespace std::chrono;


class TB6612FNG {

    public: 
       
        TB6612FNG(PinName dir_pin, PinName pwm_pin, PinName encdr_pin, PinName encdr_interrupt_pin);
		
        void set_period_us(int microseconds);
		
		void setMotorDirection(int direction);
		
		void setRadius(float radius);
		
		void updateSteps();

        void changeVelocity(float newVel);
		
		void calculateDistance();
		
		void calculateLinearVelocity();
		
		void calculateAngularVelocity();

		void attachTicker(); 

		void removeTicker();
	   
	    // getters and setters: 

		float getVel();
		
		int getSteps();

		int getDirection();

		int getPulsesPerRev();
		
		float getRadius();
		
		float getVelocity();
		
		float getVelocityPWM();
		
        float getDistance();
		
		float getAngularVelocity();
		
		void setSteps(int new_steps);
		
		void setPulsesPerRev(int ppr);
		
    private: 

      DigitalOut MotorDir;
      
	  DigitalIn Encoder; 

	  InterruptIn EncoderInt;
	  
      PwmOut MotorPWM;
	  
	  Ticker TICKER;

	  PlatformMutex motor_mutex;

	  volatile bool foundVel = false; 
	  
	  volatile int steps;
	  
	  volatile int last_pulse_count;

	  volatile int direction; 

	  volatile int pulsesPerRev; 
	  
	  volatile float lastTime; 

	  volatile float pulse_distance;
	  
	  volatile float radius; 
	  
	  volatile float distance; 
	  
	  volatile float velocity_PWM;
	  
	  volatile float velocity;	  

      volatile float angular_velocity;

}; 

#endif
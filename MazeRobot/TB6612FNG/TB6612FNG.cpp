#include "TB6612FNG.h"

TB6612FNG::TB6612FNG(PinName dir_pin, PinName pwm_pin, PinName encdr_pin, PinName encdr_interrupt_pin) : MotorDir(dir_pin), MotorPWM(pwm_pin), Encoder(encdr_pin), EncoderInt(encdr_interrupt_pin){
	
	EncoderInt.rise(callback(this, &TB6612FNG::updateSteps));
	
	steps = 0; 
	
	last_pulse_count = 0;
	velocity = 0; 
	
	direction = 0; 
	
	distance = 0;

	pulsesPerRev = 0; 
	
};

/** 
 * Attach a ticker for calculating velocity to the motor instance. 
 * To avoid interrupt conflicts, tickers should be attached and detached  for each separate motor instance. 
 * Tickers are used to calculate the velocity of a given motor instance in cm/s. 
 * Tickers are called every seconds once attached. Once the velocity has been obtained, the ticker MUST be detached. 
 * 
 * @param NONE
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.attachTicker();
 * **/

void TB6612FNG::attachTicker(){
   
   TICKER.attach(callback(this, &TB6612FNG::calculateLinearVelocity), 1.0); // 1000000us = 1 sec, test for 60 secs

   return; 
};

/** 
 * Remove a ticker for calculating velocity to the motor isntance. 
 * To avoid interrupt conflicts, tickers should be attached and detached  for each separate motor instance. 
 * Tickers are used to calculate the velocity of a given motor instance in cm/s. 
 * Tickers are called every seconds once attached. Once the velocity has been obtained, the ticker MUST be detached. 
 * 
 * @param NONE
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.attachTicker();
 * 
 * MotorA.detachTicker();
 * **/

void TB6612FNG::removeTicker(){
   
   TICKER.detach();

   return; 
};

/** 
 * Set the period in microseconds for the PWM signal going to the motors. 
 * 
 * @param microseconds: PWM pulse width in microseconds (int)
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.period_us(1000); // 1ms period
 * **/


void TB6612FNG::set_period_us(int microseconds){
	
	MotorPWM.period_us(microseconds);
	
	return; 
};

/** 
 * Set the number of pulses per rev of a motor instance
 * 
 * @param NONE 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.setPulsesPerRev(330); 
 * **/

void TB6612FNG::setPulsesPerRev(int ppr){

    pulsesPerRev = ppr; 

	return; 

};

/** 
 * Set the direction for the motors to turn. 1 = backwards, 0 = forwards  
 * 
 * @param direction: Direction to turn motors (int)
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.setMotorDirection(0); // forwards
 * **/

void TB6612FNG::setMotorDirection(int direction){
	
	if(direction > 1 || direction < 0){
		
		return;
		
	} else {
       
	   MotorDir = direction;
	
	   return; 		
	}

}; 

/** 
 * Specify the radius of the wheel attached to the motor in cm.
 * 
 * @param radius_wheel: radius of the wheel in cm (float)
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.setRadius(12); 
 * **/

void TB6612FNG::setRadius(float radius_wheel){
	
	this->radius = radius_wheel; 
	
}

/** 
 * Change the velocity of rotation of the motor by specifying the pulse width going to the motors.   
 * Specify velocity as the PWM duty cycle. E.g. 0.5 = 50% duty cycle and 1 = 100% duty cycle.
 * 
 * @param newVel: New PWM duty cycle for motors (float) 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.changeVelocity(newVel); 
 * **/

void TB6612FNG::changeVelocity(float newVel){

	   EncoderInt.rise(NULL);
	   TICKER.detach();
	   
	   velocity_PWM = newVel;

	   motor_mutex.lock(); // use mutexes to avoid race conditions 
	   
	   MotorPWM.write(newVel); 	

	   motor_mutex.unlock();

	   EncoderInt.rise(callback(this, &TB6612FNG::updateSteps));
	   TICKER.attach(callback(this, &TB6612FNG::calculateLinearVelocity), 1.0); // 1000000us = 1 sec, test for 60 secs

	return; 
};

/** 
 * Calculate the distance travelled by the motor in cm.   
 * 
 * @param NONE
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.calculateDistance(); 
 * **/

void TB6612FNG::calculateDistance(){
	distance = (steps*3.14*(radius*2)/330);
	return; 
};

/** 
 * Calculate the linear velocity of the motor in cm/s. 
 * 
 * @param NONE 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.calculateLinearVelocity(); // forwards
 * **/

void TB6612FNG::calculateLinearVelocity(){
    
    // Calculate pulses within the time interval
    int pulses = steps - last_pulse_count;
    last_pulse_count = steps;
	
	pulse_distance = ((pulses*3.14*(radius*2))/pulsesPerRev);
	
    if(direction == 0){ // forwards 
	    velocity = (pulse_distance);	
	} else { // backwards
		velocity = -(pulse_distance);
	}
		
	return; 
};

/** 
 * Calculate the velocity of the motor in cm/s
 * 
 * @param NONE 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.changeVelocity(0.5);
 * 
 * MotorA.getVel();
 * **/

float TB6612FNG::getVel(){
    
	Serial.println("Velocity is: ");
	Serial.println(velocity);

	return pulse_distance;
};

/** 
 * Calculate the angular velocity of the motors in rad/s 
 * 
 * @param NONE 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.calculateAngularVelocity(); // forwards
 * **/

void TB6612FNG::calculateAngularVelocity(){
	
	// w = v/r 
	
	angular_velocity = (velocity / radius);
	
	
	return; 
};

/** 
 * Function used by ISR to update the steps of the motor. You should not need to call this function yourself. 
 * 
 * @param NONE
 * **/

void TB6612FNG::updateSteps(){
	
	if(direction == 0){
		steps++; // backshaft motor revolutions 
		
	}
	else {
		steps--;
		
	}
	
	return; 
};

/** 
 * Get the current number of steps travelled by the motor.   
 * 
 * @param NONE 
 * 
 * Code example: 
 * TB6612FNG MotorA; 
 * 
 * MotorA.getSteps(); // forwards
 * **/

int TB6612FNG::getSteps(){

	 motor_mutex.lock();
     
	 delay(20); 
	 int motorSteps = steps;
	 
	 motor_mutex.unlock();

	return motorSteps; 
};

/**
 * Get the current direction (forwards = 0, backwards = 1) of a motor instance
 * @param NONE 
 * 
 * Code example:
 * TB6612FNG MotorA; 
 * 
 * MotorA.setMotorDirection(0);
 * 
 * Serial.println("Direction is: ");
 * Serial.println(MotorA.getDirection());
 */

int TB6612FNG::getDirection(){

	return direction; 
};

/**
 * Get the pulses per revolution of a motor instance
 * 
 * @param NONE 
 * 
 * Code example:
 * TB6612FNG MotorA; 
 * 
 * MotorA.setPulsesPerRev(330);
 * 
 * Serial.println("Direction is: ");
 * Serial.println(MotorA.getPulsesPerRev());
 */

int TB6612FNG::getPulsesPerRev(){

	return pulsesPerRev; 
};

/** 
 * Get the current linear velocity of the motor in cm/s.  
 * 
 * @param : NONE 
 * **/

float TB6612FNG::getVelocity(){

	return velocity; 
};

/** 
 * Get the current distance travelled by the motors in cm.  
 * 
 * @param : NONE 
 * **/

float TB6612FNG::getDistance(){
	return distance; 
};

/** 
 * Get the PWM value currently in use. 
 * 
 * @param : NONE 
 * **/

float TB6612FNG::getVelocityPWM(){
	return velocity_PWM;
};

/** 
 * Get the angular velocity of the motors in rad/s.
 * 
 * @param : NONE 
 * **/

float TB6612FNG::getAngularVelocity(){
	return angular_velocity; 
};

/** 
 * Get the radius of the motor in cm.   
 * 
 * @param : NONE 
 * **/

float TB6612FNG::getRadius(){
	return radius; 
};

/** 
 * Set the number of steps for the motor. 
 * 
 * @param : NONE 
 * **/

void TB6612FNG::setSteps(int new_steps){
	EncoderInt.rise(NULL);
	steps = new_steps; 
	EncoderInt.rise(callback(this, &TB6612FNG::updateSteps));
	return;
};

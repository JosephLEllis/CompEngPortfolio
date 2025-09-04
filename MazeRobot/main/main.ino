#include "HCSR04.h"
#include "TFLuna.h"
#include "TB6612FNG.h"
#include "MPU6050.h"
#include "RobotBody.h"

// create a new TFLuna I2C instance: 
TFLuna LIDAR(P0_31, P0_2); 

std::vector<TFLuna*> list_of_lidars = {&LIDAR}; // sc0 = front lidar, sc1 = red motor lidar, sc2 = yellow motor lidar, sc3 = back lidar

// create a new GYRO I2C instance: 
MPU6050 gyro(P0_31, P0_2);

std::vector<MPU6050*> list_of_gyros = {&gyro};

// create a new motorA instance: 
TB6612FNG RedMotor(P0_4, P0_27, P1_11, P1_11);

TB6612FNG YellowMotor(P0_5, P1_2, P1_12, P1_12);

std::vector<TB6612FNG*> list_of_motors = {&RedMotor, &YellowMotor};

// create a new sonar instance: 
HCSR04 RedMotorUltrasonic(P0_23, P0_3, P0_3); 
HCSR04 YellowMotorUltrasonic(P1_14, P0_28, P0_28);
HCSR04 FrontUltrasonic(P1_13, P0_29, P0_29);

std::vector<HCSR04*> list_of_ultrasonics = {&FrontUltrasonic, &RedMotorUltrasonic, &YellowMotorUltrasonic};

// list of sensor orientations: 
std::vector<int> orientations = {0, 270, 90, 180}; // angle relative to east (east = 0)

// Assemble components into a robot body
RobotBody robot_base(list_of_motors, list_of_ultrasonics, list_of_lidars, list_of_gyros, orientations);

 
void setup() {

  Serial.begin(9600);
  
  RedMotorUltrasonic.defaultInterrupts();
  YellowMotorUltrasonic.defaultInterrupts();
  FrontUltrasonic.defaultInterrupts();

  // put your setup code here, to run once:
  RedMotor.set_period_us(1000); // 10ms

  YellowMotor.set_period_us(1000); // 10ms

  // Motor A:
  RedMotor.setMotorDirection(0);

  RedMotor.setRadius(2.4);

  RedMotor.setPulsesPerRev(330);

  // Motor B: 
  YellowMotor.setMotorDirection(0);

  YellowMotor.setRadius(2.4);

  YellowMotor.setPulsesPerRev(330);

  // gyro: 

  gyro.initialise();
  
  // Robot base: 
  robot_base.setTheta(0);

  robot_base.setWheelBase(9);

  robot_base.setSampleSize(50);
  
  robot_base.setCMPerCell(10);

  robot_base.setStartPoint(1, 8);

  robot_base.setGoal(1, 1);

}

void loop() {

  robot_base.updateState();

  delay(1000);
  
}
 
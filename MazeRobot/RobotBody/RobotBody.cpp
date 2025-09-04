#include "RobotBody.h"

RobotBody::RobotBody(const std::vector<TB6612FNG*>& motors, const std::vector<HCSR04*>& ultrasonics, 
                    const std::vector<TFLuna*>& lidars, const std::vector<MPU6050*>& gyros, 
                     std::vector<int> orientations) : 
                     motor_list(motors), 
					 ultrasonic_list(ultrasonics), 
					 lidar_list(lidars), 
					 gyro_list(gyros),
					 orientation_list(orientations),
					 MotorPID(&normalisedError, &PD_output, &setPointSpeed, 4, 2, 1, DIRECT), 
					 mapMetaData(10, std::vector<GridCellData>(10)), 
					 currentState(State::updateObstacleMap) { // Kp = 4, ki = 2, Kd = 1 

	// WORLD COORDINATE RELATED VARIABLE INITIALISATION: 

	previous_x = 0;
	
	world_grid_coordinate_row, world_grid_coordinate_column = 0; 

	world_coordinate_x, world_coordinate_y = 0; 

	// MAP AND COLLISION DETECTION RELATED VARIABLE INITIALISATION:

	cellSize = 0;

	row, column = 0; 

	diagonal_row, diagonal_column = 0; 

	obst_row, obst_col = 0;

	delta_distance, old_distance_travelled_in_cell_magnitude  = 0;

	turn_angle = 0;

	avoidingObstacle = false;
	avoidedObstacle = false; 
    
	reachedGoal = false; 
	isGoalReachable = true;  

	foundCorner = false; 

   // MOTOR RELATED VARIABLE INITIALISATION: 

	previousDirA, previousDirB = 0; 

	wheel_radius = 0;

	prevSpeed = 0;

	wheelBase = 0;

	isMovingForwards = false; 
    
	// PID RELATED VARIABLE INITIALISATION: 
	
	setPoint, setPointSpeed = 0;

	// TURN RELATED VARIABLE INITIALISATION: 

	theta, targetAngle = 0;

	// SENSOR RELATED VARIABLE INITIALISATION: 
	
    sum_distance = 0;
    temp = 0;
	average_distance = 0;

    
	// initialise the map: 

	initialiseMap();

};

/** 
 * Initialise the map. This will make sure that each 1 included in the border of the map will be counted as occupied by the robot. 
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.initialiseMap();
**/
void RobotBody::initialiseMap(){
	
	for (int row = 0; row < OCCUPANCY_MAP_Y; row++ ) { // rows 
      // loop through columns of current row
      
	  for (int column = 0; column < OCCUPANCY_MAP_X; column++ ) { // columns 
          
		  if(occupancy_map[row][column] == 1){
              mapMetaData[row][column].isOccupied = true; 
		  }         
	  }
	} 
   
};

/** 
 * Function to determine whether a value is within a range. Returns true if the val is within range, and false if the val isn't within range. 
 * 
 * @param low: lower bound of check. 
 * @param high: upper bound of check. 
 * @param val: The value that is being checked. 
 * 
 * Code example: 
 * 
 * if(robot_base.inRange(1, 9, 3) == true){
 *    Serial.println("True");
 * } else {
 *   Serial.println("False"); 
 * }
**/

bool RobotBody::inRange(unsigned low, unsigned high, unsigned val){
	
	return (low <= val && val <= high); 
};

/** 
 * Force a value between two values. If x is above or below the upper and lower values, it will be clamped to the upper or lower.  
 * 
 * @param x: value to be clamped. 
 * @param upper: upper bound that x can take.
 * @param lower: lower bound that x can take. 
 * 
 * Code example: 
 * 
 * robot_base.clamp(9, 1, 8); // clamp to upper (8)
**/

float RobotBody::clamp(float x, float upper, float lower){
	return min(upper, max(x, lower));
};

/** 
 * Actions for the robot to perform based off of it's current state. The FSM logic is as follows: 
 * 
 * updateObstacleMap -> This is the default initial state of the robot. The robot will poll all the sensors and update the occupancy map in memory. The robot will
 * check if the end point is reachable. If it is, then it will calculate a path based off of the current obstacle data in the obstacle map. If it isn't, the robot 
 * will stop and exit out of the FSM. Obstacles are indicated by a 1 on the occupancy map. The robot's position in the map is marked by an 8.
 * 
 * calculatePath -> If the destination is reachable, the robot will calculate a path to the given goal coordinates using a BFS. This is an initial path given based 
 * off of the current obstacle data in the occupancy map. The old path is initially cleared in order to reset data on the map. If a new obstacle is encountered, then the 
 * path is recalculated. This is also where the robot checks to see whether it has reached the end point. If it has, then it will stop. Otherwise, it will adjust it's position 
 * in order to move along the calculated path. 
 * 
 * adjustForPath -> Based off of the calculated path and the given obstacle data, the robot will attempt to determine which cell it should move to in the next instance. 
 * checkSquare() checks in all directions perpendicular to the robot's estimated position in the occupancy map. If a 2/path is found (there should only ever be one path tile in every perpendicular direction),
 * then the robot will turn towards the direction of the next cell it should move towards. The robot also attempts to avoid corners by moving 2 cells ahead if a potential 
 * corner is detected in the ocucpancy map. If this happens, then the map is updated again in order to ensure that the robot hasn't move in front of any other obstacle. 
 * If no corners are detected, then the robot immediately moves on to the next state. 
 * 
 * moveAlongPath -> Once the robot has turned the correct direction, it will move one cell along the intended path.
 * 
 * goalReached -> If the robot has reached it's goal, it will stop and output the occupancy map. 
 * 
 * goalNotReachable -> If the robot has not reached it's goal, it will stop and output the occupancy map. 
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * void loop(){
 * 
 *   robot_base.updateState();
 * 
 *   delay(1000); // wait 1s 
 * 
 * }
 * 
**/

void RobotBody::updateState(){

   switch(currentState){
      
	  case State::updateObstacleMap:

	    Serial.println("Updating obstacle map...");

	    updateMap(orientation_list);

		printMap();

		if(isGoalReachable == false){
           Serial.println("Goal is not reachable, stopping robot...");
		   currentState = State::goalNotReachable;
		   break; 
		} 

	  case State::calculatePath: 

		// clear old path before plotting new path: 

		Serial.println("Calculating new path...");

		clearOldPath();

	    BFS(world_grid_coordinate_column, world_grid_coordinate_row, goal_x, goal_y); // column, row format (opposite for BFS on grid map)

		// check if goal has been reached: 
		if(world_grid_coordinate_column == goal_x && world_grid_coordinate_row == goal_y){
            Serial.println("Reached goal!");
			currentState = State::goalReached;
			break; 
		} else {
			Serial.println("Have not reached goal");
			currentState = State::adjustForPath;
			break; 
		}

	  case State::adjustForPath: 
       
	    Serial.println("Adjusting orientation for path...");

		checkSquare(occupancy_map, world_grid_coordinate_column, world_grid_coordinate_row, 1); 

		if(foundCorner == true){

			move(cellSize, 0.5, FORWARDS); // move forwards 2 cells in order to avoid corner
			move(cellSize, 0.5, FORWARDS); 

			if(turn_angle != 0){

                turn(turn_angle, 0.5);

				turn_angle = 0;

				currentState = State::updateObstacleMap;

				break;
					
				} 

		 } else {

			if(turn_angle != 0){

                turn(turn_angle, 0.5);

				turn_angle = 0;
					
				} 

		 }

	   currentState = State::moveAlongPath; 

	  break; 

	  case State::moveAlongPath: 

	     Serial.println("Moving along path");

         move(cellSize, 0.5, FORWARDS); // move 1 cell before rescanning

		 currentState = State::updateObstacleMap; 

	  break; 

	  case State::goalReached: 
        
		stop();

		printMap();

	  break;  

	  case State::goalNotReachable: 
	     
		Serial.println("Stopped robot because it has been determined that the goal is not reachable...");

		stop(); 
        
		printMap();

	  break; 
   }

   return;

}; 

/** 
 * Clear the old path (and data) on the occupancy map   
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.clearOldPath();
**/

void RobotBody::clearOldPath(){

   for (int row = 0; row < OCCUPANCY_MAP_Y; row++) { // rows 
         // loop through columns of current row
           for (int column = 0; column < OCCUPANCY_MAP_X; column++) { // columns 

		       mapMetaData[row][column].hasBeenVisited = false;

	 		   mapMetaData[row][column].parentX = -1; 
			   mapMetaData[row][column].parentY = -1;

               if(occupancy_map[row][column] == 2){
				   occupancy_map[row][column] = 0; 
			   }  
	        }
        } 
};

/** 
 * Since the motor encoders are potentially unreliable, use the data obtained from the gyroscope in order to calculate the offset (if there is any offset) from the target
 * angle within a range. The robot will adjust once depending on the difference between the yaw angle and the target angle. 
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.turn(45, 0.5); // robot turn automatically calls checkOrientation();
**/

void RobotBody::checkOrientation(){
   
   // if the yaw angle is not equal to the target angle (plus some offset), correct the robot position: 

   float lowerBound = targetAngle - (targetAngle*0.05);
   float upperBound = targetAngle + (targetAngle*0.05);

   float error = 0; 

   if(inRange(lowerBound, upperBound, yaw_angle) != true){

      // we are not in the given range, turn the specified amount: 
      error = (targetAngle - yaw_angle);
      
	  // if error is greater than 0, then we have overshot, so turn the opposite way
	  if(error > 0){

		float radians = abs((error * (M_PI/180)));

	    float distance_arc = (radians * (wheelBase/2)); 

		int number_steps_required = (distance_arc/(M_PI*(motor_list[0]->getRadius()*2)))*motor_list[0]->getPulsesPerRev();

		motor_list[0]->setMotorDirection(BACKWARDS);  // Left turn
        motor_list[1]->setMotorDirection(FORWARDS);

		motor_list[0]->changeVelocity(0.5); 
		motor_list[1]->changeVelocity(0.5);

		while(((motor_list[0]->getSteps() + motor_list[1]->getSteps())/2) <= number_steps_required){ 
 
		}

		motor_list[0]->changeVelocity(0); 
		motor_list[1]->changeVelocity(0);
	   
        motor_list[0]->setMotorDirection(FORWARDS);  // Left turn
        motor_list[1]->setMotorDirection(FORWARDS);

      
	  }
      
	  // if error is less than 0, then we have undershot, so turn the opposite way
	  if(error < 0){

		float radians = abs((error * (M_PI/180)));

	    float distance_arc = (radians * (wheelBase/2)); 

		int number_steps_required = (distance_arc/(M_PI*(motor_list[0]->getRadius()*2)))*motor_list[0]->getPulsesPerRev();

		motor_list[0]->setMotorDirection(FORWARDS);  // Left turn
        motor_list[1]->setMotorDirection(BACKWARDS);

		motor_list[0]->changeVelocity(0.5); 
		motor_list[1]->changeVelocity(0.5);

		while(((motor_list[0]->getSteps() + motor_list[1]->getSteps())/2) <= number_steps_required){ 
 
		}

		motor_list[0]->changeVelocity(0); 
		motor_list[1]->changeVelocity(0);
	   
        motor_list[0]->setMotorDirection(FORWARDS);  // Left turn
        motor_list[1]->setMotorDirection(FORWARDS);

	  }

   } 

   return; 
};

/** 
 * As the robot is moving, it will poll the ultrasonic sensors (ultrasonic sensors are used for close range detection) once for fast reaction time. 
 * If any of the ultrasonic sensors detect an obstacle within 1-9cm, the robot will initiate the obstacle avoidance logic. If an obstacle has been avoided, 
 * the robot will stop moving in the current direction and break out of the while() loop in move(). This is in order to prevent the robot from constantly moving 
 * back towards the obstacle once it has backed off. 
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.move(10, 0.5, 0); // avoidObstacle() is automatically called in move()
**/

void RobotBody::avoidObstacle(){

	std::vector<float> ultrasonic_sensor_readings = pollUltrasonicSensors(MIN_SAMPLE_SIZE);

	for(int i = 0; i < ultrasonic_sensor_readings.size(); i++){

		// Use ultrasonics for close range:
        
         if(inRange(1, 9, ultrasonic_sensor_readings[0])){ // Front ultrasonic

              Serial.println("Obstacle detected in front, moving backwards...");

			  avoidingObstacle = true;  

			  stop(); 

			  move(cellSize, 0.5, BACKWARDS);

			  Serial.println("Stopped moving backwards");

			  avoidedObstacle = true;

			  avoidingObstacle = false; 

			  break; 

		 }

		 if(inRange(1, 9, ultrasonic_sensor_readings[1])){ // Red motor/left 

			Serial.println("Obstacle detected by red motor, moving...");

			avoidingObstacle = true; 

			turn(45, 0.5); 

			move(cellSize, 0.5, FORWARDS);

	        turn(-45, 0.5);

			avoidedObstacle = true; 

            avoidingObstacle = false; 

			break; 

		 }

		 if(inRange(1, 9, ultrasonic_sensor_readings[2])){ // Yellow motor/right
            
            Serial.println("Obstacle detected by yellow motor, moving...");

			avoidingObstacle = true; 

			turn(-45, 0.5); 

			move(cellSize, 0.5, FORWARDS);

	        turn(45, 0.5);

			avoidedObstacle = true; 

			avoidingObstacle = false; 

			break;

		 }
      

	}

	return; 
    
};

/** 
 * Check the cells perpendicular to the robot's position in the occupancy map. Initially, the checkSquare will check for a path cell and calculate the direction 
 * the robot is required to turn in order to continue. It will then attempt to detect any obstacles around the path cell by checking diagonally for an obstacle cell (1).
 * If an obstacle cell is found diagonally, then it will check to see if there is a path cell perpendicular in all directions to itself -> if there is a path cell, 
 * there is a high chance that it is a corner. A boolean is set in order to alear the robot that it has has encountered a potential corner. 
 * 
 * @param grid: Pointer to the occupancy map. 
 * @param centre_x: The x-position of the robot in the occupancy map. 
 * @param centre_y: The y-position of the robot in the occupancy map. 
 * @param radius: How many tiles in each perpendicular direction to check around the robot's position.
 * 
 * Code example: 
 * 
 * robot_base.checkSquare(occupancy_map, world_grid_coordinate_x, world_grid_coordinate_y, 1);
**/

void RobotBody::checkSquare(const std::vector<std::vector<char>>& grid, int centre_x, int centre_y, int radius){

	std::vector<std::pair<int, int>> directions = {{-radius, 0}, {radius, 0}, {0, -radius}, {0, radius}};

	for(const auto& dir : directions){

		row = centre_y + dir.second; 
		column = centre_x + dir.first; 

		 if(row>= 0 && row < OCCUPANCY_MAP_X && column >= 0 && column < OCCUPANCY_MAP_Y){
             if(grid[row][column] == 2){ // should only ever be 1 two in any of the four directions

				float angle = (atan2((row - centre_y), (column - centre_x)) * 180/M_PI);
                
                float delta = (angle - theta);

               // Calculate delta and normalize to [-180, 180)
                turn_angle = fmod(delta + 180, 360) - 180;

				turn_angle = static_cast<int>(round(turn_angle));

				// check for L-shapes here (corners) -> has to be relative to orientation: 

				// There will always be a 1 diagonal to the found 2, check around the 2 for a 1-> if found, we are in a corner
				std::vector<std::pair<int, int>> diagonal_directions = {{-1, -1}, {1, -1}, {-1, 1}, {1,1}};

				for(const auto& dir_diag : diagonal_directions){

					diagonal_row = row + dir_diag.second; 
					diagonal_column = column + dir_diag.first; 


					if(occupancy_map[diagonal_row][diagonal_column] == 1){

						// found a 1, check around the 1, if a 2 is found, then we are at a corner

						std::vector<std::pair<int, int>> dir_obstacle = {{-radius, 0}, {radius, 0}, {0, -radius}, {0, radius}};

						for(const auto& dir_obstacle : dir_obstacle){

							obst_row = diagonal_row + dir_obstacle.second; 
						    obst_col = diagonal_column + dir_obstacle.first; 

							 if(obst_row >= 0 && obst_row < OCCUPANCY_MAP_X && obst_col >= 0 && obst_col < OCCUPANCY_MAP_Y){

								if(occupancy_map[obst_row][obst_col] == 2){

						            foundCorner = true; 

								}

						    }


						}
					} else {
						Serial.println("Corner not found in path");
					}



				}

				break; 
                
			 } else {
				Serial.println("Did not find path (2)");
			 }
		 } else {
			Serial.println("Out of bounds access attempted");
		 }
		
	}

	Serial.println("Finished");


};

/** 
 * Move the robot a specified number of cm, at a certain PWM, forwards or backwards. move() automatically checks for obstacles and will attempt to keep the robot 
 * moving in a straight line by using a PID controller to adjust the PWM of the motors. The PID controller will attempt to keep the difference between speed of the motors 
 * as close to zero as possible by increasing the PWM of one wheel if one is moving at a slower rate compared to the other. move() also updates the world x and y coordinates. 
 * 
 * Code example: 
 * 
 * robot_base.move(10, 0.5, 0); // move 10cm at PWM 0.5 forwards. 
**/

void RobotBody::move(int distance, float speed, int forw_backw){ // 0 = forwards, 1 = backwards 

    if(forw_backw == 1){
       isMovingForwards = false; 
	} else {
		isMovingForwards = true;
	}
	
	//(forw_backw == BACKWARDS ?) : isMovingForwards = false : isMovingForwards = true;  
	
	int old_steps_A = motor_list[0]->getSteps(); // hardcode for now 
    
	int old_steps_B = motor_list[1]->getSteps();

    motor_list[0]->setSteps(0);

    motor_list[1]->setSteps(0);	

	// use float to reduce errors (e.g. could be 44.5, but if int will be rounded down to 44):
	
	float distance_in_steps = round(((distance/(M_PI*motor_list[0]->getRadius()*2))*motor_list[0]->getPulsesPerRev()));  

	// convert to int for comparison after rounding: 

	int distanceSteps = static_cast<int>(distance_in_steps); 

	// set the motor direction depending on the given parameter (1 = backwards, 0 = forwards):
	
	motor_list[0]->setMotorDirection((forw_backw == BACKWARDS) ? 1 : 0);
	
	motor_list[1]->setMotorDirection((forw_backw == BACKWARDS) ? 1 : 0);

	// change the velocity of the wheels as required: 
	
	motor_list[0]->changeVelocity(speed); // left 
	
	motor_list[1]->changeVelocity(speed); // right

	// set up PID controller: 

    MotorPID.SetMode(AUTOMATIC);
	
	setPointSpeed = 0; 

    MotorPID.SetOutputLimits(0, 1);	
	
	MotorPID.SetSampleTime(SAMPLE_RATE_PID); // in ms

	// keep the motors moving in the given direction whilst we have not met the required number of steps: 
	
	while(((motor_list[0]->getSteps() + motor_list[1]->getSteps())/2) <= distanceSteps){ // E.g. 219+219/2 = 219
      
     // check for obstacles at the start: 
	 
     if(avoidingObstacle == false){
		
		avoidObstacle();

		if(avoidedObstacle == true){

			// if we have avoided an obstacle, do not continue forwards -> break out of the loop 

			break; 

		}
        
	  }
	  
	  current_error = (motor_list[0]->getSteps() - motor_list[1]->getSteps());
	  
	  // normalise data: 
	  
	  float maxError = motor_list[0]->getPulsesPerRev(); // max error occurs when each encoder is 330 counts behind the other	  
	  
	  float min = 0; // minimum error that can occur 
	  
	  normalisedError = ((current_error - min)/maxError); // convert to a percentage 
		
	  normalisedError = -1*abs(normalisedError);

      MotorPID.Compute(); 

	 // adjust motor speeds depending on the PID output: 
	  
	 if(current_error > 0){

		  float newSpeedRed = motor_list[0]->getVelocityPWM() - (PD_output/SMOOTHING_FACTOR);
		  newSpeedRed = clamp(newSpeedRed, 1, speed); // minimum speed must be 0.3
		  motor_list[0]->changeVelocity(newSpeedRed);
		  
		  float newSpeedYellow = motor_list[1]->getVelocityPWM() + (PD_output/SMOOTHING_FACTOR);
		  
		  newSpeedYellow = clamp(newSpeedYellow, 1, speed); // minimum speed must be 0.3
		  motor_list[1]->changeVelocity(newSpeedYellow); // speed with no float newSpeedYellow
		  
      } else if(current_error < 0){
		  
		  float newSpeedYellow = motor_list[1]->getVelocityPWM() - (PD_output/SMOOTHING_FACTOR); 
		  
		  newSpeedYellow = clamp(newSpeedYellow, 1, speed); // minimum speed must be 0.3
		  motor_list[1]->changeVelocity(newSpeedYellow); // speed with no float newSpeedYellow
		  
		  float newSpeedRed = motor_list[0]->getVelocityPWM() + (PD_output/SMOOTHING_FACTOR); 
		  
		  newSpeedRed = clamp(newSpeedRed, 1, speed); // minimum speed must be 0.3
		  motor_list[0]->changeVelocity(newSpeedRed);
		  
		  
      }

	  // delay for 10 milliseconds depending on the sampling time of the PID controller (for consistent velocity sampling)

	  thread_sleep_for(SAMPLE_RATE_PID);
	  
	}

	if(avoidedObstacle == true){

		// if we have broken out of the loop, we have already updated the steps, so we do not need to update them again
       
	    avoidedObstacle = false; 

	   return; 
	}

	// stop after moving: 
	
	motor_list[0]->changeVelocity(0); // left 
	
	motor_list[1]->changeVelocity(0); // right 
	
	// reset steps + add/subtract new: 

	motor_list[0]->setSteps((forw_backw == FORWARDS) ? (old_steps_A + (int)distance_in_steps) : (old_steps_A - (int)distance_in_steps));

    motor_list[1]->setSteps((forw_backw == FORWARDS) ? (old_steps_B + (int)distance_in_steps) : (old_steps_B - (int)distance_in_steps));

	// update world x and y:
	
    float theta_radians = (theta * (M_PI/180));
	
	float angle_cos = cos(theta_radians); // cos requires radian input, so convert to rads
	
	float angle_sin = sin(theta_radians);
	
    world_coordinate_x += (distance * angle_cos); 
	
	world_coordinate_y += (distance * angle_sin); 

	return; 
};

/** 
 * Mark a detected obstacle on the occupancy map. Depending on the data passed, the obstacle will be plotted on the occupancy map. Should an already plotted obstacle 
 * be along the current column or along the current row, then do not replot, as it is likely a duplicate value. 
 * 
 * @param row: The y-coordinate of the robot in the occupancy map. 
 * @param column: The x-coordinate of the robot in the occupancy map. 
 * @param deltaRow: Offset from the current y-coordinate in which the obstacle will be placed on the occupancy map. 
 * @param deltaCol: Offset from the current x-coordinate in which the obstacle will be placed on the occupancy map.
 * @param value: Value to represent obstacle. 
 * @param index: Index from which the sensor value was read. 
 * @param orientation: orientation of the sensor from which the data was taken. 
 * 
 * Code example: 
 * 
 * robot_base.initialiseMap();
**/

void RobotBody::markObstacle(int row, int column, int deltaRow, int deltaCol, int value, int index, int orientation){
	
	int newRow = row + deltaRow; 
	int newCol = column + deltaCol;

	bool foundObstacleInRayCast = false; 
	
	// array bounds checking: 
	if(newRow >= 0 && newRow < OCCUPANCY_MAP_X && newCol >= 0 && newCol < OCCUPANCY_MAP_Y){
			
			// raycast check: (depending on orientation):

			switch(orientation/90){
             
			   case 0: 
               //Serial.println("Checking along line East");
            
			   // check along columns: 
			   for(int column = world_grid_coordinate_column; column < OCCUPANCY_MAP_X-1; column++){
                   if(occupancy_map[world_grid_coordinate_row][column] == 1){ // -1 to account for the edge
                      
					  foundObstacleInRayCast = true; 
					  break;   
				   }
				} 
			   
			   if(foundObstacleInRayCast == false){
                   
                   occupancy_map[newRow][newCol] = value;
		
		           // update map metadata: 
	               mapMetaData[newRow][newCol].isOccupied = true; 
	               
			       break;  
			   } else {
				 
				   break; 
			   }

			   case 1: 
			   
			      // check along rows: 
			   for(int row = world_grid_coordinate_row; row < OCCUPANCY_MAP_Y-1; row++){
                   if(occupancy_map[row][world_grid_coordinate_column] == 1){ // -1 to account for the edge
                      
					  foundObstacleInRayCast = true; 
					  break;   
				    }
				} 
			   
			   if(foundObstacleInRayCast == false){
                   
                   occupancy_map[newRow][newCol] = value;
		
		           // update map metadata: 
	               mapMetaData[newRow][newCol].isOccupied = true; 
	               
			       break;  
			   } else {
				   //Serial.println("Obstacle found in raycast south, not replotting");
				   break; 
			   }

			   case 3: 
			   //Serial.println("Checking along line North");
			    //  check along rows: 
			   for(int row = world_grid_coordinate_row; row != 0; row--){
                   if(occupancy_map[row][world_grid_coordinate_column] == 1){ // -1 to account for the edge
                      
					  foundObstacleInRayCast = true; 
					  break;   
				   }
				} 
			   
			   if(foundObstacleInRayCast == false){
                   
                   // update map:
				   occupancy_map[newRow][newCol] = value;

				  // update map metadata: 
	              mapMetaData[newRow][newCol].isOccupied = true; 
	              
			       break;  
			   } else {
				   
				   break; 
			   }


			}
	
	} else {
		Serial.println("Out of bounds access attempt on occupancy map");
		return; 
	}
	
};

/** 
 * Used to check if an obstacle is within a cell distance between the sensors and the obstacle. 
 * 
 * @param row: Current Y-coordinate in occupancy map. 
 * @param column: Current X-coordinate in the occupancy map.  
 * @param deltaRow: Offset from y-coordinate to place obstacle. 
 * @param deltaCol: Offset from x-coordinate to place obstacle. 
 * @param value: Value to mark obstacle. 
 * @param measured_val: The original reading of the sensor. 
 * @param orientation: orientation of the sensor from which the data was taken. 
 * 
 * Code example: 
 * 
 * robot_base.initialiseMap();
**/

void RobotBody::markObstacleLessThan10(int rw, int col, int deltR, int deltC, int val, int measured_val, int orientation_small){
	
	    int newR = rw + deltR;
		int newC = col + deltC;
        
		
		bool foundObstacleInRayCast = false; 
		
		if(newR >= 0 && newR < OCCUPANCY_MAP_X && newC >= 0 && newC < OCCUPANCY_MAP_Y){
		 
		
		  if(inRange(1, cellSize-1, measured_val) == true){
			
			// raycast check: (depending on orientation):

			switch(orientation_small/90){
             
			   case 0: 
              
            
			   // check along columns: 
			   for(int column = world_grid_coordinate_column; column < OCCUPANCY_MAP_X-1; column++){

                   if(occupancy_map[world_grid_coordinate_row][column] == 1){ // -1 to account for the edge
                      
					  foundObstacleInRayCast = true; 
					  break;   
				   }
				   } 
			   
			   if(foundObstacleInRayCast == false){
                 
                   // update map:
				   occupancy_map[newR][newC] = val;

				   // update map metadata: 
	               mapMetaData[newR][newC].isOccupied = true; 
				  
			       break;  
			   } else {
				  
				   break; 
			   }

			   case 1: 

			 
			      // check along rows: 
			   for(int row = world_grid_coordinate_row; row < OCCUPANCY_MAP_Y-1; row++){

                   if(occupancy_map[row][world_grid_coordinate_column] == 1){ // -1 to account for the edge
                      
					  foundObstacleInRayCast = true; 
					  break;   
				   }
				} 
			   
			   if(foundObstacleInRayCast == false){
                 
                   // update map:
				   occupancy_map[newR][newC] = val;

				  // update map metadata: 
	              mapMetaData[newR][newC].isOccupied = true; 
				 
	              
			       break;  
			   } else {
				 
				   break; 
			   }

			   case 3: 

			      // check along rows: 
			   for(int row = world_grid_coordinate_row; row != 0; row--){

                   if(occupancy_map[row][world_grid_coordinate_column] == 1){ // -1 to account for the edge
                     
					  foundObstacleInRayCast = true; 
					  break;   
				   }
				} 
			   
			   if(foundObstacleInRayCast == false){
                  
                   // update map:
				   occupancy_map[newR][newC] = val;

				  // update map metadata: 
	              mapMetaData[newR][newC].isOccupied = true; 
	              
			       break;  
			   } else {
				   
				   break; 
			   }
			      
			}  
					  
		  } else {
			  //Serial.println("Value was zero, not adding to map");	  
			  return; 
		  }
		  
		} else {
			Serial.println("Out of bounds access attempt on occupancy map, marked less than 10");
		    return; 
		}
	
};

/** 
 * Marking an obstacle on the map requires the orientations of the sensors in order (e.g. for the front sensor, the orientation would be zero). In terms of orientation, 
 * the front of the robot is taken as East, with the clockwise direction from the front (East) being South etc... In terms of degrees, clockwise is taken as positive 
 * and anticlockwise is taken as negative. The LIDAR sensors and sonar sensors are both polled, with each returning an average value for each individual sensor.
 * 
 * Sensor orientations are always taken to be multiples of 90.
 * 
 * LIDAR averages are taken if the distance received is greater than 30cm, and ultrasonic values are used if LIDAR values are taken to be less than 20cm. 
 * 
 * If a cell value for an obstacle is zero, there is a possibility that the obstacle is less than 10cm away and is less than a grid cell away. This is accounted for by 
 * markObstacleLessThan10(). Otherwise, the obstacle is marked on the map. 
 * 
 * updateMap() also updates the robot's marker on the occupancy map, depending on whether it has moved a cell sie or not. 
 * 
 * @param sensor_orientations: List of sensor orientations (in order)  
 * 
 * 
 * Code example: 
 * 
 * robot_base.updateMap()
**/
void RobotBody::updateMap(std::vector<int> sensor_orientations){

	//convert to grid coordinates:
    std::vector<int> grid_readings; 
	std::vector<int> original_vector_position;
	std::vector<int> original_measured_value; 
	std::vector<float> lidar_sensor_readings = pollLidarSensors(sampleSize, 3);
	std::vector<float> ultrasonic_sensor_readings = pollUltrasonicSensors(sampleSize); 

    // map for key value pair: 1 key = 2 values, (measured distance, orientation)
	std::unordered_map<int, std::vector<int>> mapped_sensor_values; 
	
	for(int reading_index = 0; reading_index < lidar_sensor_readings.size(); reading_index++){
		
		// find out how many cells away the obstacle is:
		if(lidar_sensor_readings[reading_index] <= 20 && ultrasonic_sensor_readings[reading_index] <= 30){
            
			int ultrasonic_measured_value= (round(ultrasonic_sensor_readings[reading_index])/10);
			grid_readings.push_back(ultrasonic_measured_value);	
            original_vector_position.push_back(reading_index); // save the reading index for later checking
			original_measured_value.push_back(ultrasonic_sensor_readings[reading_index]);	

		} else {

			int lidar_measured_value = (round(lidar_sensor_readings[reading_index])/cellSize);
			grid_readings.push_back(lidar_measured_value);	
            original_vector_position.push_back(reading_index); // save the reading index for later checking
			original_measured_value.push_back(lidar_sensor_readings[reading_index]);
		}	
	}
	
	// check both are the same size later: 
	// map key-value pairs: 
	for(int x = 0; x < grid_readings.size(); x++){
		mapped_sensor_values[x] = {grid_readings[x], sensor_orientations[x], original_vector_position[x], original_measured_value[x]};
	}
	
	// iterate through key value pairs -> depending on the orientation, apply different mappings:
   // FORMAT: key->distance (grid distance), orientation, position in original where it was found	
	for(auto &pair : mapped_sensor_values){

		if((pair.second[1] % 90 == 0)){ // if multiple of 90 orientation and value of first does not equal 0

			switch((pair.second[1]/90)){ // find out what multiple of 90 we are: 
				
				case 0: 
				 
				 
                 (pair.second[0] == 0)				 
				 ? markObstacleLessThan10(world_grid_coordinate_row, world_grid_coordinate_column, 0, 1, 1, pair.second[3], pair.second[1]) 
				 : markObstacle(world_grid_coordinate_row, world_grid_coordinate_column, 0, pair.second[0], 1, pair.second[2], pair.second[1]);
				 
				 
				break; 
				
				
				case 1: 
				 
				 
				 (pair.second[0] == 0)				 
				 ? markObstacleLessThan10(world_grid_coordinate_row, world_grid_coordinate_column, 1, 0, 1, pair.second[3], pair.second[1]) 
				 : markObstacle(world_grid_coordinate_row, world_grid_coordinate_column, pair.second[0], 0, 1, pair.second[2], pair.second[1]);
		
				
				break; 
				 
				
				case 2:
				
				  
				
				 (pair.second[0] == 0)				 
				 ? markObstacleLessThan10(world_grid_coordinate_row, world_grid_coordinate_column, 0, -1, 1, pair.second[3], pair.second[1]) 
				 : markObstacle(world_grid_coordinate_row, world_grid_coordinate_column, 0, -pair.second[0], 1, pair.second[2], pair.second[1]);
				
				break; 
                			 
				
				case 3: 
				
				 
				 
				 (pair.second[0] == 0)
				 ? markObstacleLessThan10(world_grid_coordinate_row, world_grid_coordinate_column, -1, 0, 1, pair.second[3], pair.second[1])
                 : markObstacle(world_grid_coordinate_row, world_grid_coordinate_column, -pair.second[0], 0, 1, pair.second[2], pair.second[1]);

                break; 				 
				
				default: 
				Serial.println("ERROR IN MAP OBJECT UPDATE");
			}
		 
		} else {
			Serial.println("Angle is not a multiple of 90 degrees for key value pair, or the value for the recorded distance is zero");
		}
	}

	int cells_moved_x = 0; 
	int cells_moved_y = 0;

    float delta_x = world_coordinate_x - previous_x;
	
	float delta_y = world_coordinate_y - previous_y; 
	
	if(fabs(delta_x) >= cellSize){
		
		cells_moved_x = fabs((delta_x/cellSize));

        previous_x = world_coordinate_x; 
		previous_y = world_coordinate_y; 
	}

	if(fabs(delta_y) >= cellSize){
       cells_moved_y = fabs((delta_y/cellSize));
	  
	   previous_x = world_coordinate_x; 
	   previous_y = world_coordinate_y; 

	}

	// as we move 10cm move the robot marker into the next cell: 
	// I.e. get distance in cm and divide by 10 to find the number of cells we have traversed: 
	
    if(cells_moved_x >= 1 || cells_moved_y >= 1) {
		
	
	if(theta % 90 == 0){
		
		// normalise to between 0 - 360 if -ve: 
		int temp_theta = 0; 
		
		// if -ve, add 360 to normalise: 
	    temp_theta = (std::signbit(theta) ? (temp_theta = theta + 360) : (temp_theta = theta)); 		
	
		int quadrant = (temp_theta/90);
	    
        		
		switch(quadrant){
			
			case 0: // 1st quadrant, facing east, 0 
			   

			   if(isMovingForwards == true){

				 occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			     occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column+(1*cells_moved_x)] = 8; 
			     world_grid_coordinate_column+=(1*cells_moved_x);

			   } else {

				 occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			     occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column-(1*cells_moved_x)] = 8; 
			     world_grid_coordinate_column-=(1*cells_moved_x); 
				  
			   }
	
	          
			break; 

            case 1: // 2nd quadrant, facing South, 90 
              

			   if(isMovingForwards == true){

				occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			    occupancy_map[world_grid_coordinate_row+(1*cells_moved_y)][world_grid_coordinate_column] = 8; 
			    world_grid_coordinate_row+=(1*cells_moved_y); 

			   } else {
				 
                occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			    occupancy_map[world_grid_coordinate_row-(1*cells_moved_y)][world_grid_coordinate_column] = 8; 
			    world_grid_coordinate_row-=(1*cells_moved_y); 

			   }
			   
			   
            break; 

            case 2:  // 3rd quadrant, facing West, 180
             

			  if(isMovingForwards == true){
                
				occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			    occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column-(1*cells_moved_x)] = 8; 
			    world_grid_coordinate_column-=(1*cells_moved_x); 
			  
			  } else {

				occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			    occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column+(1*cells_moved_x)] = 8; 
			    world_grid_coordinate_column+=(1*cells_moved_x);
			  }
			
            break; 

            case 3:  // 4th quadrant, facing North, 270
              

			 if(isMovingForwards == true){

               occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			   occupancy_map[world_grid_coordinate_row-(1*cells_moved_y)][world_grid_coordinate_column] = 8; 
			   world_grid_coordinate_row-=(1*cells_moved_y); 

			 } else {

			   occupancy_map[world_grid_coordinate_row][world_grid_coordinate_column] = 0; 
			   occupancy_map[world_grid_coordinate_row+(1*cells_moved_y)][world_grid_coordinate_column] = 8; 
			   world_grid_coordinate_row+=(1*cells_moved_y); 

			 }  
			
            break;			

            default: 
               Serial.println("ERROR->Likely due to theta being over 360 degrees (multiple of 360)");			
			  
		}
	} else {
		Serial.println("Angle not multiple of 90 degrees");
	}

	}

	
	return; 
};

/** 
 * Calculate a possible path to the end of the occupancy map. Initially taken from: https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/ and modified.
 * 
 * @param startY: Start position y-coordinate in occupancy map. 
 * @param startX:  Start position X-coordinate in occupancy map.
 * @param goalX: Goal X-coordiante in occupancy map. 
 * @param goalY: Goal Y-coordiante in occupancy map. 
 * 
 * Code example: 
 * 
 * robot_base.BFS(1,1,1,8);
**/

void RobotBody::BFS(int startY, int startX, int goalX, int goalY) {
    
	int rows = OCCUPANCY_MAP_Y;
    int cols = OCCUPANCY_MAP_X;

    // Create a queue for BFS
    std::queue<std::pair<int, int>> q;

    // Start from the initial position
    q.push({startX, startY});
    mapMetaData[startX][startY].hasBeenVisited = true;  // Mark start as visited

	// define directions for BFS: 
	std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // BFS loop
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;

        // Check if we reached the goal
        if (x == goalX && goalY == y) {
			reachedGoal = true; 
			Serial.println("Goal reached at: ");
			Serial.println(x);
			Serial.println(y);
			reconstructPath(goalX, goalY);
			printMap();
            return;  // Goal found, path exists
        }

        // Check all 4 possible neighbors
        for (auto dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            // Check if the neighbor is within bounds, not occupied, and not visited
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && !mapMetaData[nx][ny].isOccupied && !mapMetaData[nx][ny].hasBeenVisited) {
                
				// Mark the neighbor as visited & set parent as current cell:
                mapMetaData[nx][ny].hasBeenVisited = true;
                
				mapMetaData[nx][ny].parentX = x; 
				mapMetaData[nx][ny].parentY = y; 

                // Add the neighbor to the queue
                q.push({nx, ny});
            }
        }
    }

    // If we exhausted the queue without finding the goal, return false
    Serial.println("Goal not reachable");
	isGoalReachable = false; 
    return;
}


/** 
 * Print the path on the occupancy map, with each path cell being represented by a 2. 
 * 
 * @param goalX: Goal X-coordiante in occupancy map. 
 * @param goalY: Goal Y-coordiante in occupancy map. 
 * 
 * Code example: 
 * 
 * robot_base.BFS(1,1,1,8);
 * robot_base.reconstructPath(1,1);
 * robot_base.printMap();
**/

void RobotBody::reconstructPath(int goalX, int goalY) {
    int x = goalX;
    int y = goalY;

    // Backtrack from the goal to the start, printing the path
    while (mapMetaData[x][y].parentX != -1 && mapMetaData[x][y].parentY != -1) {
        // Mark this cell as part of the path:
		if(occupancy_map[x][y] == 8){
           // leave robot marker on map: 
		   continue; 
		} else {
          occupancy_map[x][y] = 2;
		}

        // Move to the parent cell
        int tempX = mapMetaData[x][y].parentX;
        int tempY = mapMetaData[x][y].parentY;
        x = tempX;
        y = tempY;
    }
}

/** 
 * Turn a set number of degrees at a certain PWM speed. Positive angles are in the clockwise direction and negative angles are in the anti-clockwise direction. 
 * The variable target_angle is used to check the robot has matched the correct orientation within a certain range within checkOrientation(). The ICC of the robot is
 * assumed to be at the centre, since the robot will stop moving before performing a turn. 
 * 
 * @param degrees: Number of degrees to turn. 
 * @param turn_speed: PWM-speed to turn (max 1)
 * 
 * Code example: 
 * 
 * robot_base.turn(45, 0.5);
**/

void RobotBody::turn(int degrees, float turn_speed){
       
	   // set target angle for later checking: 

	   targetAngle = degrees;

	  // quickly store velocity val.
	   float current_vel = motor_list[0]->getVelocity(); // pick any motor vel-> the only time in which wheels will be different velocities is when we are turning whilst moving. 


	   if(current_vel > 0){

		  for (float pwm = turn_speed; pwm > 0; pwm -= 0.1) {
          motor_list[0]->changeVelocity(pwm);
		  }

		  for (float pwm = turn_speed; pwm > 0; pwm -= 0.1) {
          motor_list[1]->changeVelocity(pwm);
     
          }


        } 
	    // stop then turn: 
	    motor_list[0]->changeVelocity(0); // left 
	
	    motor_list[1]->changeVelocity(0); // right
	   
	   // calculate relevant quantities: 
	   
	   float radians = abs((degrees * (M_PI/180)));

	   float distance_arc = (radians * (wheelBase/2)); 
	   
	   int old_steps_A = motor_list[0]->getSteps(); // hardcode for now 
	   
	   int old_steps_B = motor_list[1]->getSteps(); 
	   
	   // // reset steps:
	   motor_list[0]->setSteps(0);
	   
	   motor_list[1]->setSteps(0);

       int number_steps_required = (distance_arc/(M_PI*(motor_list[0]->getRadius())))*motor_list[0]->getPulsesPerRev();

	   if (degrees >= 0) {
            motor_list[0]->setMotorDirection(FORWARDS);  // Right turn
            motor_list[1]->setMotorDirection(BACKWARDS);
        } else {
            motor_list[0]->setMotorDirection(BACKWARDS);  // Left turn
            motor_list[1]->setMotorDirection(FORWARDS);

            }
        
			motor_list[0]->changeVelocity(turn_speed); // left
            
	        motor_list[1]->changeVelocity(turn_speed);

			unsigned long previousMillis = 0; 

			unsigned long currentMillis = 0;

			float dt = 0; 

			gyro_list[0]->setZ(0);

	    while(((motor_list[0]->getSteps() + motor_list[1]->getSteps())/2) <= number_steps_required){ 

		      // calculate yaw angle from z-axis of gyroscope (need to integrate over time):

			   gyro_list[0]->readRawDataGyro();

			   currentMillis = millis(); 

			   dt = (currentMillis - previousMillis); 

			   previousMillis = currentMillis; 

			   yaw_angle += (((float)gyro_list[0]->getZ() / gyro_list[0]->getSensitivity()) * dt); // convert z to degrees per sec by dividing by default sensitivity in degrees/s
 
		}
	   
	    // // reset direction: 
	   motor_list[0]->setMotorDirection(0);

	   motor_list[1]->setMotorDirection(0);
	   
	   // restore old steps: 
	   motor_list[0]->setSteps(old_steps_A);
	   
	   motor_list[1]->setSteps(old_steps_B);

	   motor_list[0]->changeVelocity(0); // left 
	
	   motor_list[1]->changeVelocity(0); // right

	   // update degrees of whole body:
	   theta += degrees; 
       
	   // check orientation using gyroscope module:
	   checkOrientation();

	   yaw_angle = 0;

	   return; 
	   
};

/** 
 * Poll the ultrasonic sensors a set number of times. Returns a list of averaged ultrasonic values, with the index being the order in which they were polled. 
 * 
 * @param required_samples_U: The required number of samples from ultrasonic.
 * 
 * Code example: 
 * 
 * robot_base.pollUltrasonicSensors(50); // poll each sensor 50 times 
**/

std::vector<float> RobotBody::pollUltrasonicSensors(int required_samples_U){
	
	std::vector<float>ultrasonic_sensor_results; // return a list of the ultrasonic index and the distance obtained after the index. E.g. 0, 8 => ultrasonic 0, distance = 8
	
	for(int sensor_index = 0; sensor_index < ultrasonic_list.size(); sensor_index++){
       

	   // get ultrasonic sensor info: Front, Red, Yellow
		for(int num_samples = 0; num_samples < required_samples_U; num_samples++){
			
			ultrasonic_list[sensor_index]->pulseOut();
		    ultrasonic_list[sensor_index]->calculateDistance();
		    temp = ultrasonic_list[sensor_index]->getDistance();
			
		    sum_distance += ultrasonic_list[sensor_index]->getDistance();
		}

		average_distance = (sum_distance/required_samples_U);

		ultrasonic_sensor_results.push_back(average_distance);
		
		sum_distance = 0; 
		
		temp = 0; 
		
		average_distance = 0;
		
	}
	
	return ultrasonic_sensor_results; 
};

/** 
 * Poll the LIDAR sensors a set number of times. Returns a list of averaged LIDAR values, with the index being the order in which they were polled. 
 * 
 * @param required_samples_L: The required number of samples from LIDAR sensors. 
 * 
 * Code example: 
 * 
 * robot_base.pollLidarSensors(50, 3); // poll each of the three LIDARs 50 times. 
**/

std::vector<float> RobotBody::pollLidarSensors(int required_samples_L, int num_sensors) {
    
   std::vector<float>lidar_sensor_results;

   for(int sensor_index = 0; sensor_index < num_sensors; sensor_index++){

	    lidar_list[0]->switchPort(sensor_index+1); 

	   // get ultrasonic sensor info: 
		for(int num_samples = 0; num_samples < required_samples_L; num_samples++){
			
		    lidar_list[0]->calculateDistance();
			sum_distance += lidar_list[0]->getDistance();

		}

		average_distance = (sum_distance/required_samples_L);

		lidar_sensor_results.push_back(average_distance);
		
		sum_distance = 0; 
		
		temp = 0; 
		
		average_distance = 0;
   }

   return lidar_sensor_results; 

};

/** 
 * Stop the robot.  
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.stop(); 
**/

void RobotBody::stop(){
	
	previousPWM_A = motor_list[0]->getVelocityPWM(); 
	
	previousPWM_B = motor_list[1]->getVelocityPWM();

	previousDirA = motor_list[0]->getDirection();

	previousDirB = motor_list[1]->getDirection();
	
	motor_list[0]->changeVelocity(0); // left 

	delay(20);
	
	motor_list[1]->changeVelocity(0); // right
	
	return; 
};

/** 
 * Start the robot from rest.
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.start(); 
**/

void RobotBody::start(){

	motor_list[0]->setMotorDirection(previousDirA);

	motor_list[1]->setMotorDirection(previousDirB);
	
	motor_list[0]->changeVelocity(previousPWM_A);

	delay(20);
	
    motor_list[1]->changeVelocity(previousPWM_B);
	
    return; 	
};

/** 
 * Print the occupancy map.
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.printMap();
**/

void RobotBody::printMap(){
	
	for (int i = 0; i < OCCUPANCY_MAP_Y; i++) { // rows 
      // loop through columns of current row
      for (int j = 0; j < OCCUPANCY_MAP_X; j++) { // columns 
      Serial.print(occupancy_map[i][j], DEC);
	  }
	  
	  Serial.println("");
	  
   } 
	return; 
};

/** 
 * Print the occupancy map metadata. 
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.printMetaData();
**/

void RobotBody::printMetaData(){
	
	for(int x = 0; x < mapMetaData.size(); x++){
		
		for(int y = 0; y < mapMetaData[x].size(); y++){
			Serial.print("Grid[");
            Serial.print(x);
            Serial.print("][");
            Serial.print(y);
            Serial.print("] -> Distance: ");
            Serial.print(mapMetaData[x][y].distance);
            Serial.print(", Is Occupied: ");
            Serial.println(mapMetaData[x][y].isOccupied ? "Yes" : "No");
		}
		
	}
	
	return; 
};


// getters and setters: 

/** 
 * Set the wheel base of the robot
 * 
 * @param wheel_base: Wheel base in cm. 
 * 
 * Code example: 
 * 
 * robot_base.setWheelBase(9); 
**/

void RobotBody::setWheelBase(float wheel_base){

	wheelBase = wheel_base; 

	return;
};

/** 
 * Set the starting orientation of the robot. 
 * 
 * @param newTheta: New value of theta in degrees. 
 * 
 * Code example: 
 * 
 * robot_base.setTheta(90); 
**/

void RobotBody::setTheta(int newTheta){
	
	theta = newTheta;
	return;
};

/** 
 * Set the number of cm per occupancy map cell. 
 * 
 * @param cell_size: Size of cell in cm.  
 * 
 * Code example: 
 * 
 * robot_base.setCMPerCell(10);
**/

void RobotBody::setCMPerCell(int cell_size){
    
	cellSize = cell_size; 

	return; 
};

/** 
 * Set the goal coordinates for the robot. 
 * 
 * @param x: Goal coordinate x. 
 * @param y: Goal coordiante y. 
 * 
 * Code example: 
 * 
 * robot_base.setGoal(1,1);
**/

void RobotBody::setGoal(int x, int y){

	goal_x = x; 

    goal_y = y; 

	return;
};

/** 
 * Set the start point for the robot.
 * 
 * @param startX: Starting x-coordinate on the occupancy map. 
 * @param startY: Starting y-coordinate on the occupancy map. 
 * 
 * Code example: 
 * 
 * robot_base.setStartPoint(1,8);
**/

void RobotBody::setStartPoint(int startX, int startY){

	start_x = startX; 

	start_y = startY;
    
	// insert start point into map: 
	occupancy_map[start_y][start_x] = 8;

	world_grid_coordinate_row = startY; 

	world_grid_coordinate_column = startX;

	return; 
};

/** 
 * Set the sample size for polling of sensors.
 * 
 * @param sample_size: Starting x-coordinate on the occupancy map. 
 * 
 * Code example: 
 * 
 * robot_base.setSampleSize(50);
**/

void RobotBody::setSampleSize(int sample_size){

	sampleSize = sample_size;
};

/** 
 * Get the current robot orientation
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.getTheta();
**/

int RobotBody::getTheta(){
	return theta; 
};

/** 
 * Get the robot's current x-coordinate
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.getGridXCoordinate();
**/

int RobotBody::getGridCoordinateX(){
	return world_grid_coordinate_column; 
};

/** 
 * Get the robot's current y-coordinate
 * 
 * @param NONE
 * 
 * Code example: 
 * 
 * robot_base.getGridYCoordinate();
**/

int RobotBody::getGridCoordinateY(){
	return world_grid_coordinate_row;
};
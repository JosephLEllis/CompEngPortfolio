#ifndef RobotBody_H
#define RobotBody_H

#include <vector>
#include <limits>
#include <unordered_map>
#include <queue>
#include <cmath>

// Not mine, taken from https://github.com/br3ttb/Arduino-PID-Library/blob/master/PID_v1.h through the arduino library manager. 
#include <PID_v1.h>

// custom libraries: 
#include "TB6612FNG.h"
#include "HCSR04.h"
#include "TFLuna.h"
#include "MPU6050.h"

using namespace std::chrono;

struct GridCellData;

class RobotBody {
	
	public: 
	
	RobotBody(const std::vector<TB6612FNG*>& motors, const std::vector<HCSR04*>& ultrasonics, 
	          const std::vector<TFLuna*>& lidars, const std::vector<MPU6050*>&gyros, 
			  std::vector<int> orientations);

    enum class State {
       
	   updateObstacleMap, 
	   calculatePath, 
	   adjustForPath, 
	   moveAlongPath, 
	   goalReached, 
	   goalNotReachable

	}; 

	State getCurrentState() const; 

	void updateState();

	void clearOldPath();

	void initialiseMap();

	void checkOrientation();

	void checkSquare(const std::vector<std::vector<char>>& grid, int centre_x, int centre_y, int radius);  

	void move(int distance, float speed, int forw_backw);

	void turn(int degrees, float turn_speed);
	
	void stop();
	
	void start();
	
	void markObstacle(int row, int column, int deltaRow, int deltaCol, int value, int index, int orientation);
	
	void markObstacleLessThan10(int rw, int col, int deltR, int deltC, int val, int measured_val, int orientation_small);

	void avoidObstacle(); 
	
	void updateMap(std::vector<int> sensor_orientations);
	
	void printMap();
	
	void printMetaData();

	void BFS(int startX, int startY, int goalX, int goalY); 

	void reconstructPath(int goalX, int goalY);

	std::vector<float> pollUltrasonicSensors(int required_samples_U);

	std::vector<float> pollLidarSensors(int required_samples_L, int num_sensors);

	bool inRange(unsigned low, unsigned high, unsigned val); 
	
	float clamp(float x, float upper, float lower);
	
    // getters and setters:
	
	int getTheta(); 

	int getGridCoordinateX();

	int getGridCoordinateY(); 

	void setWheelBase(float wheel_base);

	void setSampleSize(int sample_size);

	void setTheta(int newTheta); 

	void setCMPerCell(int cell_size);

	void setGoal(int x, int y);

	void setStartPoint(int startX, int startY); 
	
	private: 
	
	PID MotorPID;

	State currentState;
	
	static const int SMOOTHING_FACTOR = 255;

	static const int MIN_SAMPLE_SIZE = 2; 

	static const int SAMPLE_RATE_PID = 10; 

    static const int OCCUPANCY_MAP_X = 10; 

    static const int OCCUPANCY_MAP_Y = 10;

	static const int FORWARDS = 0; 
	
	static const int BACKWARDS = 1; 

	volatile bool reachedGoal; 

	volatile bool isGoalReachable; 

	volatile bool isMovingForwards; 

	volatile bool avoidingObstacle; 

	volatile bool avoidedObstacle; 

	volatile bool foundCorner; 

	double normalisedError; 
	
	double setPointSpeed; 
	
	double PD_output; // change to PID later 
	
	double current_error;

	volatile int targetAngle; 

	volatile int sampleSize;

	volatile int cellSize; 

	volatile int goal_x; 

	volatile int goal_y;

	volatile int start_x; 

	volatile int start_y; 
	
	volatile int world_coordinate_x; 
	
	volatile int world_coordinate_y; 

	volatile int world_grid_coordinate_row; 
	
	volatile int world_grid_coordinate_column; 
	
    volatile int theta; 	
	
	volatile int sum_distance; 
	
	volatile int temp; 
	
	volatile int average_distance; 
	
	volatile int delta_distance; 

    volatile int previousDirA; 

	volatile int previousDirB;

	volatile int row, column; 

	volatile int diagonal_row, diagonal_column; 

	volatile int obst_row, obst_col;

	volatile float turn_angle;
    
	volatile float prevSpeed;

	volatile float wheelBase;   

	volatile float motorVelA; 
   
    volatile float motorVelB;

    volatile float old_distance_travelled_in_cell_magnitude;

	volatile float previous_x; 

	volatile float previous_y;
	
	volatile float setPoint; 
	
	volatile float previousPWM_A; 
	
	volatile float previousPWM_B;
	
	volatile float wheel_radius;

	volatile float yaw_angle; 

	// occupancy map metadata struct: 
	struct GridCellData {
		
		float distance; 

		bool isOccupied, hasBeenVisited;
		
        int parentX, parentY;

		GridCellData(float dist = std::numeric_limits<float>::infinity(), bool occupied = false, bool visited = false)
        : distance(dist), isOccupied(occupied), hasBeenVisited(visited), parentX(-1), parentY(-1) {}

	};
	
	std::vector<std::vector<char>> occupancy_map = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // rows, columns
		                                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
										            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}	
	}; // only need 8 bytes per array, 0 for nothing, 1 for an obstacle. 1 byte = 1*10x10cm cell   
	   // 8 = robot position, 1 = obstacle detected 
	   
	std::vector<std::vector<GridCellData>> mapMetaData; // mapMetadata = row, individual row columns = individual row columns 

	//std::vector<float> vel_list = {0, 0, 0, 2.19, 5.44, 8.91, 12.19, 15.71, 19.32, 23.61, 27.27}; // in cm/s

	std::vector<int> orientation_list; 
	
	std::vector<TB6612FNG*> motor_list;
	
	std::vector<HCSR04*> ultrasonic_list; 
	
	std::vector<TFLuna*> lidar_list; 

	std::vector<MPU6050*> gyro_list; 
	
};

#endif
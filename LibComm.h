#ifndef __LIBCOMM_H
#define __LIBCOMM_H

#define CENTER 0
#define LEFT 1
#define RIGHT 2
#define BACK 3

typedef struct {
    bool IRSensorRequest[4];
    bool BeaconRequest[10];
    bool GroundRequest;
    bool CompassRequest;
} request_t;

/*! Initializes Robot and Connects to Simulator 
 *  Parameters : 
 *          name - Robot name
 *          host - Host where simulator is running 
 *  Returns -1 in case of error
 */
// int InitRobot (char *name, int id, char *host);

/*! Initializes Robot and Connects to Simulator 
 *  Parameters : 
 *          name - Robot name
 *          host - Host where simulator is running 
 *          IRSensorAngles - contais the angles of the 4 IR Sensors 
 *                           with respect to the front of the robot (in degrees).
 *   Returns -1 in case of error
 */
int InitRobot2(char* name, int id, double IRSensorAngles[4], char* host);

/*! Gets the next Sensor Values sent by Simulator 
 *  if no message is present waits 
 *  Returns -1 in case of error 
 */
int ReadSensors(void);

/*  The following functions access values that have been read by ReadSensors() 
 *  they do not read new values 
 */

/*  simulation time */
unsigned int GetTime(void);
unsigned int GetNumberOfBeacons(void);

/*! GetObstacleSensor value is inversely proportional to obstacle distance  
 *  id may be one of LEFT, RIGHT, CENTER or OTHER1 
 */
double GetObstacleSensor(int id);
bool IsObstacleSensorValid(int i);

/*! Indicates if a new beacon measure has arrived. 
 *  The value of GetBeaconSensor is invalid when IsBeaconReady returns false
 */
bool IsBeaconReady(int id);

struct beaconMeasure {
    bool beaconVisible; /* true if robot can see beacon */
    double beaconDir; /* direction of beacon */
    /*   only valid if beaconVisible is true */
};

/* GetBeaconSensor value is the direction of Beacon 
 * in Robot coordinates (-180.0, 180.0) 
 */
struct beaconMeasure GetBeaconSensor(int id);

/* GetCompassSensor value is the direction of Robot in Ground 
 * coordinates (-180.0, 180.0) 
 */
double GetCompassSensor(void);
bool IsCompassValid(void);

/* active when in Beacon area */
int GetGroundSensor(void);
bool IsGroundSensorValid(void);

/* active when robot collides */
bool GetBumperSensor(void);

/* Start */
bool GetStartButton(void);

/* Stop */
bool GetStopButton(void);

bool GetVisitingLed(void);
/* Verify if SetReturningLed() was executed */
bool GetReturningLed(void);

/* Verify if Finish() was executed */
bool GetFinished(void);

/* Return the simulation cycle time */
int GetCycleTime(void);

/* Return the total simulation time */
int GetFinalTime(void);

/* GPS sensor - can be used for debug, invoke simulator with "-gps" option */
double GetX(void);
double GetY(void);
double GetDir(void);

/* 
 * The following functions command Robot behavior 
 */

/* Drive right motor with rPow and left motor with lPow - Powers in (-0.1,0.1) */
bool DriveMotors(double lPow, double rPow);
void SetRequest(request_t r);

bool SetVisitingLed(void);
bool SetVisitingLed2(double lPow, double rPow);
/* Signal the end of phase 1 (go to target) */
bool SetReturningLed(void);
bool SetReturningLed2(double lPow, double rPow);

/* Finish the round */
bool Finish(void);
bool Finish2(double lPow, double rPow);

/*****
        Functions returning noise levels
******/

/* Returns maximum additive noise of infra-red sensors */
double GetNoiseObstacleSensor(void);

/* Returns maximum additive noise of beacon angular direction */
double GetNoiseBeaconSensor(void);

/* Returns maximum additive noise of compass */
double GetNoiseCompassSensor(void);

/* Returns maximum multipicative noise of motors */
double GetNoiseMotors(void);

#endif

#ifndef __LIBCOMMPRIVATE_H
#define __LIBCOMMPRIVATE_H

#include "LibComm.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_PACKET_LENGTH 1500
#define DEFAULT_PORT 6000

#define send_packet(s) (sendto(comm_socket, (s), strlen((s)), 0, (struct sockaddr*)&sv_addr, sizeof(struct sockaddr)) != -1)

typedef struct {
    int SimTime, CycleTime, NBeacons;
    double CompassNoise, BeaconNoise, ObstacleNoise;
    double MotorsNoise;
} simParams_t;

typedef struct {
    int Time;
    double Compass;
    bool ValidCompass;
    bool BeaconsReady[10]; // BARRETE
    struct beaconMeasure Beacons[10]; // BARRETE
    bool Collision;
    int Ground;
    bool GroundReady;
    double IRSensor[4];
    bool IRSensorReady[4];
    double GPSX, GPSY, GPSDir;
    bool VisitingLed, EndLed, ReturningLed;
    bool Start, Stop;
} measures_t;

void* socket_thread(void* arg);
static void parse_measures_packet(const char* packet, measures_t* new_measures);
bool ReadObstacleSensors(const char* packet, measures_t* new_measures);
bool ReadBeaconSensors(const char* packet, measures_t* new_measures);

//
static bool parse_params_packet(const char* packet);
static bool ConnectToServer(const char* hostname, int port, const char* rob_name, int pos, const double IRSensorAngles[4]);
static bool recv_packet(char* buf);
static bool PerformAction(bool visitingLed, bool returningLed, bool endLed, double lPow, double rPow);
static bool GetXmlAttribInt(const char* xml, const char* attrib_name, int* dest);
static bool GetXmlAttribDouble(const char* xml, const char* attrib_name, double* dest);
static bool GetXmlAttribYesNo(const char* xml, const char* attrib_name, bool* dest);
static bool GetXmlAttribOnOff(const char* xml, const char* attrib_name, bool* dest);
static bool GetXmlAttribStr(const char* xml, const char* attrib_name, char* dest);
static void strtolwrcase(char* s);

#endif

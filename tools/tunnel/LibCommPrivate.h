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
    int SimTime, CycleTime, RunningTimeout;
    double CompassNoise, BeaconNoise, ObstacleNoise;
    double MotorsNoise;
} simParams_t;

typedef struct {
    int Time;
    double Compass;
    bool BeaconReady;
    struct beaconMeasure Beacon;
    bool Collision, Ground;
    double IRSensor[4];
    double GPSX, GPSY, GPSDir;
    bool EndLed, ReturningLed;
    bool Start, Stop;
} measures_t;

void* socket_thread(void* arg);
void parse_measures_packet(char* packet, measures_t* new_measures);
bool ReadObstacleSensors(char* packet, measures_t* new_measures);
//
static bool parse_params_packet(const char* packet);
bool ConnectToServer(const char* hostname, int port, const char* rob_name, int pos, const double IRSensorAngles[4], char* packet);
static bool recv_packet(char* buf);
static bool PerformAction(bool returningLed, bool endLed, double lPow, double rPow);
bool GetXmlAttribInt(const char* xml, const char* attrib_name, int* dest);
bool GetXmlAttribDouble(const char* xml, const char* attrib_name, double* dest);
bool GetXmlAttribYesNo(const char* xml, const char* attrib_name, bool* dest);
bool GetXmlAttribOnOff(const char* xml, const char* attrib_name, bool* dest);
bool GetXmlAttribStr(const char* xml, const char* attrib_name, char* dest);
static void strtolwrcase(char* s);

#endif

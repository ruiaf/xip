#include "Connection.h"

Connection::Connection(char* h, char* n, int p)
{
    strcpy(host, h);
    strcpy(name, n);
    rob_id = p;
}

void Connection::connect()
{
    double angles[4] = { LEFTPOS, CENTERLEFTPOS, CENTERRIGHTPOS, RIGHTPOS };

    /* Connect Robot to simulator */
    if (InitRobot2(name, rob_id, angles, host) == -1) {
        printf("%s Failed to connect\n", name);
        exit(1);
    } else
        puts("Connected. 8)");
}

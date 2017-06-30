#include "LibComm.h"
#include "LibCommPrivate.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXBUFLEN 5000

#define MYPORT 6000

#define max(a, b) ((a) > (b) ? (a) : (b))

extern int comm_socket;
extern struct sockaddr_in sv_addr;

int main(int argc, char** argv)
{
    int rob_socket, rob_socket2, addr_len, numbytes, pos;
    struct sockaddr_in my_addr; // my address information
    struct sockaddr_in their_addr; // connector's address information
    struct sockaddr_in rob_addr; // connector's address information
    char buf[MAXBUFLEN + 1], packet[100000], rob_name[20];
    const double sensor_angles[4] = { 0, 60, -60, -180 };
    fd_set fdset;
    measures_t measures;
    struct beaconMeasure prev_beacon;
    char beacon_str[1170];

    if (argc != 2) {
        puts("Usage: tunnel server");
        return -1;
    }
    rob_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (rob_socket == -1) {
        puts("ConnectToServer: error creating socket");
        return false;
    }
    rob_socket2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (rob_socket2 == -1) {
        puts("ConnectToServer: error creating socket");
        return false;
    }

    my_addr.sin_family = AF_INET; // host byte order
    my_addr.sin_port = htons(MYPORT); // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(rob_socket, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);
    if ((numbytes = recvfrom(rob_socket, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&rob_addr, (socklen_t*)&addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    GetXmlAttribStr(buf, "Name", rob_name);
    GetXmlAttribInt(buf, "Id", &pos);
    ConnectToServer(argv[1], 6000, rob_name, pos, sensor_angles, packet);

    sendto(rob_socket2, packet, 1 + strlen(packet), 0, (struct sockaddr*)&rob_addr, sizeof(sv_addr));
    while (1) {
        FD_ZERO(&fdset);
        FD_SET(rob_socket2, &fdset);
        FD_SET(comm_socket, &fdset);
        select(max(rob_socket2, comm_socket) + 1, &fdset, NULL, NULL, NULL);
        if (FD_ISSET(rob_socket2, &fdset)) {
            addr_len = sizeof(struct sockaddr);
            if ((numbytes = recvfrom(rob_socket2, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&their_addr, (socklen_t*)&addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            sendto(comm_socket, buf, numbytes, 0, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
        }
        if (FD_ISSET(comm_socket, &fdset)) {
            addr_len = sizeof(struct sockaddr);
            if ((numbytes = recvfrom(comm_socket, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&their_addr, (socklen_t*)&addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            parse_measures_packet(buf, &measures);
            if (measures.BeaconReady) {
                prev_beacon = measures.Beacon;
            } else {
                measures.BeaconReady = true;
                measures.Beacon = prev_beacon;
            }
            if (measures.Beacon.beaconVisible)
                sprintf(beacon_str, "%lf", measures.Beacon.beaconDir);

            sprintf(buf, "<Measures Time=\"%d\">\n\t<Sensors Compass=\"%d\" Beacon=\"%s\" Collision=\"%s\" Ground=\"%s\">\n\t\t<IRSensor Id=\"0\" Value=\"%lf\"/>\n\t\t<IRSensor Id=\"1\" Value=\"%lf\"/>\n\t\t<IRSensor Id=\"2\" Value=\"%lf\"/>\n\t</Sensors>\n\t<Leds EndLed=\"%s\" ReturningLed=\"%s\"/>\n\t<Buttons Start=\"%s\" Stop=\"%s\"/>\n</Measures>\n", measures.Time, (int)measures.Compass, measures.Beacon.beaconVisible ? beacon_str : "NotVisible", measures.Collision ? "Yes" : "No", measures.Ground ? "Yes" : "No", measures.IRSensor[0], measures.IRSensor[1], measures.IRSensor[2], measures.EndLed ? "On" : "Off", measures.ReturningLed ? "On" : "Off", measures.Start ? "On" : "Off", measures.Stop ? "On" : "Off");
            //puts (buf);

            sendto(rob_socket2, buf, strlen(buf) + 1, 0, (struct sockaddr*)&rob_addr, sizeof(sv_addr));
        }
    }
    //	sendto
}

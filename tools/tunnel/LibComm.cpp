#include "LibCommPrivate.h"

#define MAX_PENDING_PACKETS 1000

int comm_socket;
struct sockaddr_in sv_addr;
static bool waiting_for_initial_reply, ignoring_packets;
static simParams_t simParams;

static bool returningLed=false, endLed=false;

static sem_t next_measures_sem;
static pthread_mutex_t next_measures_mutex = PTHREAD_MUTEX_INITIALIZER;
static measures_t next_measures[MAX_PENDING_PACKETS]; // controlled by next_measures_mutex
static int num_next_measures=0; // controlled by next_measures_mutex
//
static measures_t measures; // only used/modified by the functions callable from the bot

void parse_measures_packet (char *packet, measures_t *new_measures)
{
	char beacon_str[100];

	memset (new_measures, 0, sizeof (measures_t));
	
	strtolwrcase (packet);
	//
	GetXmlAttribInt (packet, "time", &new_measures->Time);
	//
	GetXmlAttribDouble (packet, "compass", &new_measures->Compass);
	GetXmlAttribDouble (packet, "x", &new_measures->GPSX);
	GetXmlAttribDouble (packet, "y", &new_measures->GPSY);
	GetXmlAttribDouble (packet, "dir", &new_measures->GPSDir);
	//
	GetXmlAttribOnOff (packet, "endled", &new_measures->EndLed);
	GetXmlAttribOnOff (packet, "returningled", &new_measures->ReturningLed);
	GetXmlAttribOnOff (packet, "start", &new_measures->Start);
	GetXmlAttribOnOff (packet, "stop", &new_measures->Stop);
	//
	GetXmlAttribYesNo (packet, "collision", &new_measures->Collision);
	GetXmlAttribYesNo (packet, "ground", &new_measures->Ground);
	// Check if beacon information is available
	if (GetXmlAttribStr (packet, "beacon", beacon_str)) {
		new_measures->BeaconReady = true;
		if (!strcmp (beacon_str, "notvisible"))
			new_measures->Beacon.beaconVisible = false;
		else {
			new_measures->Beacon.beaconVisible = true;
			new_measures->Beacon.beaconDir = atof (beacon_str);
		}
	}
	else
		new_measures->BeaconReady = false;
	// Read obstacle sensors
	if (!ReadObstacleSensors (packet, new_measures)) {
		puts ("ReadObstacleSensors - error");
	}
}

bool ReadObstacleSensors (char *packet, measures_t *new_measures)
{
	const char *search=packet, *begin, *end;
	char xml[500];
	int i, id, ids[4]={0,0,0,0};
	double val;

	for (i=0; i < 4; i++) {
		end = begin = strstr (search, "id");
		if (begin == NULL)
			return false;

		search = begin + 2;
		for (; *begin != '<'; begin--);
		for (; *end != '>'; end++);
		memcpy (xml, begin, end-begin+1);
		xml[end-begin+1] = '\0';
		if (!GetXmlAttribInt (xml, "id", &id))
			return false;
		if (!GetXmlAttribDouble (xml, "value", &val))
			return false;
		ids[id] = 1;
		new_measures->IRSensor[id] = val;
	}
	for (i=0; i < 4; i++)
		if (!ids[i])
			return false;
	return true;
}


bool ConnectToServer (const char *hostname, int port, const char *rob_name, int pos, const double IRSensorAngles[4], char *conn_packet)
{
	struct hostent *h;
/*	int optval;
	socklen_t optlen;*/
	
	// create the UDP socket
	comm_socket = socket (AF_INET, SOCK_DGRAM, 0);
	if (comm_socket == -1) {
		puts ("ConnectToServer: error creating socket");
		return false;
	}
/*	optlen = sizeof (int);
	if (-1 != getsockopt (comm_socket, SOL_SOCKET, SO_RCVBUF, &optval, &optlen))
		printf ("SO_RCVBUF = %d\n", optval);
	else
		puts ("getsockopt error");*/

	//
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons (port);
	memset (&sv_addr.sin_zero, 0, 8);

	// Resolve the hostname
	h = gethostbyname (hostname);
	if (h == NULL) {
		// TODO - try inet_aton (in case gethostbyname doesn't work well with ip addresses)
		close (comm_socket);
		return false;
	}
	else
		sv_addr.sin_addr = *((struct in_addr *) h->h_addr);

	sprintf (conn_packet, "<Robot Name=\"%s\" Id=\"%d\">"
			      "<IRSensor Id=\"0\" Angle=\"%g\"/>"
			      "<IRSensor Id=\"1\" Angle=\"%g\"/>"
			      "<IRSensor Id=\"2\" Angle=\"%g\"/>"
			      "<IRSensor Id=\"3\" Angle=\"%g\"/>"
			      "</Robot>", rob_name, pos, IRSensorAngles[0], IRSensorAngles[1], IRSensorAngles[2], IRSensorAngles[3]);

	waiting_for_initial_reply = ignoring_packets = true;
	if (!send_packet (conn_packet)) {
		puts ("ConnectToServer: error connecting to simulator");
		close (comm_socket);
		return false;
	}
	if (!recv_packet (conn_packet)) {
		puts ("ConnectToServer: error receiving initial reply from simulator");
		close (comm_socket);
		return false;
	}

	return true;
}

bool GetXmlAttribInt (const char *xml, const char *attrib_name, int *dest)
{
	char num[100];

	if (!GetXmlAttribStr (xml, attrib_name, num))
		return false;
	*dest = atoi (num); // TODO - use strtod? more robust, with error checking...
	return true;
}

bool GetXmlAttribDouble (const char *xml, const char *attrib_name, double *dest)
{
	char num[900];

	if (!GetXmlAttribStr (xml, attrib_name, num))
		return false;
	*dest = atof (num); // TODO - use strtod? more robust, with error checking...
	return true;
}

bool GetXmlAttribYesNo (const char *xml, const char *attrib_name, bool *dest)
{
	char str[100];

	if (!GetXmlAttribStr (xml, attrib_name, str))
		return false;

	if (!strcmp (str, "yes")) {
		*dest = true;
		return true;
	}
	else if (!strcmp (str, "no")) {
		*dest = false;
		return true;
	}
	else
		return false;
}

bool GetXmlAttribOnOff (const char *xml, const char *attrib_name, bool *dest)
{
	char str[100];

	if (!GetXmlAttribStr (xml, attrib_name, str))
		return false;

	if (!strcmp (str, "on")) {
		*dest = true;
		return true;
	}
	else if (!strcmp (str, "off")) {
		*dest = false;
		return true;
	}
	else
		return false;
}

bool GetXmlAttribStr (const char *xml, const char *attrib_name, char *dest)
{
	char *p, strfmt[100], str_search[100];

	sprintf (str_search, "%s=", attrib_name); // FIXME - permitir espaços
	p = strstr (xml, str_search);
	if (p == NULL)
		return false;

	sprintf (strfmt, "%s = \"%%[^\"]", attrib_name);

	return 1 == sscanf (p, strfmt, dest);
}

static bool recv_packet (char *buf)
{
	int len;
	struct sockaddr_in from;
	socklen_t fromlen = sizeof (struct sockaddr);

	len = recvfrom (comm_socket, buf, MAX_PACKET_LENGTH, 0, (struct sockaddr *) &from, &fromlen);

	// Check for packets from an invalid server/port, and print a warning
	// the packet is still processed in case some verification is not well thought out,
	// but the warning should be enough here...
	if (from.sin_addr.s_addr != sv_addr.sin_addr.s_addr) {
		printf ("Packet received from invalid server!! (address = %s)", inet_ntoa (from.sin_addr));
		printf (", correct address should have been %s\n", inet_ntoa (sv_addr.sin_addr));
	}
	else if (!waiting_for_initial_reply && from.sin_port != sv_addr.sin_port) {
		printf ("Packet received from correct server, but an invalid port!! (port = %d)", ntohs (from.sin_port));
		printf (", correct port should have been %d\n", ntohs (sv_addr.sin_port));
	}

	if (len <= 0)
		return false;

	if (waiting_for_initial_reply) {
		sv_addr.sin_port = from.sin_port; // all the subsequent messages should be sent to this address
		waiting_for_initial_reply = false;
	}

	buf[len] = '\0';

	return true;
}

static void strtolwrcase (char *s)
{
	for (; *s; s++)
		*s = tolower (*s);
}


#include "LibCommPrivate.h"

#define MAX_PENDING_PACKETS 1000

static int comm_socket;
static struct sockaddr_in sv_addr;
static bool waiting_for_initial_reply, ignoring_packets;
static simParams_t simParams;

static bool visitingLed=false, returningLed=false, endLed=false;

static sem_t next_measures_sem;
static pthread_mutex_t next_measures_mutex = PTHREAD_MUTEX_INITIALIZER;
static measures_t next_measures[MAX_PENDING_PACKETS]; // controlled by next_measures_mutex
static int num_next_measures=0; // controlled by next_measures_mutex
//
static measures_t measures; // only used/modified by the functions callable from the bot
static request_t cur_request;

// ******************************
// THE FUNCTIONS BELOW BELONG TO THE SOCKET-READING THREAD

void *socket_thread (void *arg)
{
	char measures_packet[MAX_PACKET_LENGTH+1];
	measures_t new_measures;

	while (1) {
		if (!recv_packet (measures_packet)) {
			puts ("socket_thread: error on recv_packet");
			continue;
		}
		if (ignoring_packets)
			continue;
		parse_measures_packet (measures_packet, &new_measures);
		//
		if (pthread_mutex_lock (&next_measures_mutex))
			puts ("erro pthread_mutex_lock");
		if (num_next_measures < MAX_PENDING_PACKETS) {
			next_measures[num_next_measures++] = new_measures;
			if (-1 == sem_post (&next_measures_sem))
				printf ("erro sem_post: %d\n", errno);
		}
		else
			puts ("Internal packet buffer overflow! Packet ignored...");
		//
		if (pthread_mutex_unlock (&next_measures_mutex))
			puts ("erro pthread_mutex_unlock");
	}
	return NULL;
}

static void parse_measures_packet (const char *packet, measures_t *new_measures)
{
	memset (new_measures, 0, sizeof (measures_t));
	

	//
	GetXmlAttribInt (packet, "time", &new_measures->Time);
	//
	if (!GetXmlAttribDouble (packet, "compass", &new_measures->Compass))
		new_measures->ValidCompass=false;
	else new_measures->ValidCompass=true;
	GetXmlAttribDouble (packet, "x", &new_measures->GPSX);
	GetXmlAttribDouble (packet, "y", &new_measures->GPSY);
	GetXmlAttribDouble (packet, "dir", &new_measures->GPSDir);
	//
	GetXmlAttribOnOff (packet, "visitingled", &new_measures->VisitingLed);
	GetXmlAttribOnOff (packet, "endled", &new_measures->EndLed);
	GetXmlAttribOnOff (packet, "returningled", &new_measures->ReturningLed);
	GetXmlAttribOnOff (packet, "start", &new_measures->Start);
	GetXmlAttribOnOff (packet, "stop", &new_measures->Stop);
	//
	GetXmlAttribYesNo (packet, "collision", &new_measures->Collision);
	if (GetXmlAttribInt (packet, "ground", &new_measures->Ground))
		new_measures->GroundReady=true;
	else new_measures->GroundReady=false;

	// Read obstacle sensors
	if (!ReadObstacleSensors (packet, new_measures)) {
		puts ("ReadObstacleSensors - error");
		puts (packet);
	}

	// Read beacon sensors
	if (!ReadBeaconSensors (packet, new_measures)) {
		puts ("ReadBeaconSensors - error");
		puts (packet);
	}
	//puts (packet);
}

bool ReadObstacleSensors (const char *packet, measures_t *new_measures)
{
	const char *search=packet, *begin, *end;
	char xml[500];
	int i, id=0;
	double val=0;

	for (i=0; i < 4; i++)
		new_measures->IRSensorReady[id]=false;

	for (i=0; i < 4; i++) {
		end = begin = strstr (search, "irsensor");
		if (begin == NULL)
			break;
	
		for (; *begin != '<'; begin--);
		for (; *end != '>'; end++);
		search = end+1; // place for searching for the next tag

		memcpy (xml, begin, end-begin+1);
		xml[end-begin+1] = '\0';
		if (!GetXmlAttribInt (xml, "id", &id))
			continue;
		if (GetXmlAttribDouble (xml, "value", &val))
			new_measures->IRSensorReady[id]=true;
		new_measures->IRSensor[id] = val;
	}
			
	return true;
}

bool ReadBeaconSensors (const char *packet, measures_t *new_measures)
{
	const char *search=packet, *begin, *end;
	char xml[500];
	int i, nbeacons, id=0;
	char beacon_str[100];

	nbeacons = GetNumberOfBeacons ();

	for (i=0; i < nbeacons; i++)
		new_measures->BeaconsReady[i] = false;

	for (i=0; i < nbeacons; i++) {
		end = begin = strstr (search, "beaconsensor");
		if (begin == NULL)
			return true;
	
		for (; *begin != '<'; begin--);
		for (; *end != '>'; end++);
		search = end+1; // place for searching for the next tag

		memcpy (xml, begin, end-begin+1);
		xml[end-begin+1] = '\0';
		if (!GetXmlAttribInt (xml, "id", &id))
			continue;
		if (GetXmlAttribStr (xml, "value", beacon_str)) {
			new_measures->BeaconsReady[id] = true;
			if (!strcmp (beacon_str, "notvisible")) {
				new_measures->Beacons[id].beaconVisible = false;
			}
			else {
				new_measures->Beacons[id].beaconVisible = true;
				new_measures->Beacons[id].beaconDir = atof (beacon_str);
			}
		}
	}
	
	return true;
}

// THE FUNCTIONS ABOVE BELONG TO THE SOCKET-READING THREAD
// ******************************

void SetRequest(request_t r) {
	cur_request.CompassRequest=r.CompassRequest;
	cur_request.GroundRequest=r.GroundRequest;
	memcpy(cur_request.IRSensorRequest,r.IRSensorRequest,sizeof(r.IRSensorRequest));
	memcpy(cur_request.BeaconRequest,r.BeaconRequest,sizeof(r.BeaconRequest));
}

int ReadSensors (void)
{
	int retval;

	ignoring_packets = false;
	while (-1 == sem_wait(&next_measures_sem))
		printf ("erro sem_wait: %d (nova tentativa...)\n", errno);

	if (pthread_mutex_lock (&next_measures_mutex))
		puts ("erro pthread_mutex_lock");
	measures = next_measures[0];
	if (num_next_measures <= 0)
		printf ("num_next_measures = %d!! (valor inválido) Time=%d\n", num_next_measures, measures.Time);
	retval = --num_next_measures;
	if (num_next_measures)
		memmove (next_measures, next_measures + 1, num_next_measures * sizeof (measures_t));
	if (pthread_mutex_unlock (&next_measures_mutex))
		puts ("erro pthread_mutex_unlock");

	return retval;
}

int InitRobot2 (char *name, int id, double IRSensorAngles[4], char *host)
{
	char hostname[1024], *colon_pos;
	int port;
	pthread_t tid;

	colon_pos = strchr (host, ':');
	if (colon_pos == NULL) { // default port
		strcpy (hostname, host);
		port = DEFAULT_PORT;
	}
	else {
		memcpy (hostname, host, colon_pos - host);
		hostname[colon_pos-host] = '\0';
		port = atoi (colon_pos+1);
	}
	// Connect to the server and start the socket-reading thread
	visitingLed = returningLed = endLed = false;
	num_next_measures = 0;

	if (-1 == sem_init (&next_measures_sem, 0, 0)) {
		puts ("erro sem_init");
		return -1;
	}

	if (ConnectToServer (hostname, port, name, id, IRSensorAngles) && !pthread_create (&tid, NULL, socket_thread,NULL))
		return 0;
	else // Error
		return -1;
}

unsigned int GetTime (void) { return measures.Time; }

unsigned int GetNumberOfBeacons (void) { return simParams.NBeacons; }

double GetObstacleSensor (int id) { return measures.IRSensor[id]; }
bool IsObstacleSensorValid(int id) { return measures.IRSensorReady[id]; }

bool IsBeaconReady (int id) { return measures.BeaconsReady[id]; }

struct beaconMeasure GetBeaconSensor (int id) { return measures.Beacons[id]; }

double GetCompassSensor (void) { return measures.Compass; }
bool IsCompassValid (void) { return measures.ValidCompass; }

int GetGroundSensor (void) { return measures.Ground; }
bool IsGroundSensorValid(void) { return measures.GroundReady; }

bool GetBumperSensor (void) { return measures.Collision; }

bool GetStartButton (void) { return measures.Start; }

bool GetStopButton (void) { return measures.Stop; }

bool GetVisitingLed (void) { return measures.VisitingLed; }

bool GetReturningLed (void) { return measures.ReturningLed; }

bool GetFinished (void) { return measures.EndLed; }

double GetX (void) { return measures.GPSX; }

double GetY (void) { return measures.GPSY; }

double GetDir (void) { return measures.GPSDir; }

bool DriveMotors (double lPow, double rPow)
{
	return PerformAction (false, returningLed, endLed, lPow, rPow);
}

bool SetVisitingLed (void)
{
	puts ("SetVisitingLed called, deprecated!");
	return SetVisitingLed2 (0.0, 0.0);
}

bool SetVisitingLed2 (double lPow, double rPow)
{
	if (PerformAction (true, returningLed, endLed, lPow, rPow)) {
		return true;
	}
	else
		return false;
}

bool SetReturningLed (void)
{
	puts ("SetReturningLed called, deprecated!");
	return SetReturningLed2 (0.0, 0.0);
}

bool SetReturningLed2 (double lPow, double rPow)
{
	if (PerformAction (false, true, endLed, lPow, rPow)) {
		returningLed = true;
		return true;
	}
	else
		return false;
}

bool Finish (void)
{
	puts ("Finished called, Deprecated!");
	return Finish2 (0.0, 0.0);
}

bool Finish2 (double lPow, double rPow)
{
	endLed = true;
	return PerformAction (visitingLed, returningLed, endLed, lPow, rPow);
}

static bool PerformAction (bool visitingLed, bool returningLed, bool endLed, double lPow, double rPow)
{
	char action_packet[MAX_PACKET_LENGTH+1];
	int semval;
	
	if (sem_getvalue (&next_measures_sem, &semval) == -1)
		puts ("erro sem_getvalue!");
	else {
		if (semval > 0) {
			if (semval == 1)
				printf ("Action 1 cycle late (ignored) : ");
			else
				printf ("Action %d cycles late (ignored) : ", semval);
			return false;
		}
	}

	if (lPow < -0.15)
		lPow = -0.15;
	else if (lPow > 0.15)
		lPow = 0.15;

	if (rPow < -0.15)
		rPow = -0.15;
	else if (rPow > 0.15)
		rPow = 0.15;

	sprintf (action_packet, "<Actions LeftMotor=\"%g\" RightMotor=\"%g\" VisitingLed=\"%s\" EndLed=\"%s\" ReturningLed=\"%s\">",
		 lPow, rPow, visitingLed ? "On" : "Off", endLed ? "On" : "Off", returningLed ? "On" : "Off");

	sprintf (action_packet, "%s <SensorRequests IRSensor0=\"%s\" IRSensor1=\"%s\" IRSensor2=\"%s\" IRSensor3=\"%s\" Ground=\"%s\" Compass=\"%s\" ",
		 action_packet,cur_request.IRSensorRequest[0]?"Yes":"No",cur_request.IRSensorRequest[1]?"Yes":"No",cur_request.IRSensorRequest[2]?"Yes":"No",
			cur_request.IRSensorRequest[3]?"Yes":"No",cur_request.GroundRequest?"Yes":"No",cur_request.CompassRequest?"Yes":"No");

	int nbeacons = GetNumberOfBeacons ();
	for (int i=0;i<nbeacons;i++) {
		sprintf (action_packet, "%s Beacon%d=\"%s\" ",action_packet,i,cur_request.BeaconRequest[i]?"Yes":"No");
	}
	sprintf (action_packet, "%s /> </Actions>",action_packet);

	if (!send_packet (action_packet)) {
		puts ("PerformAction: error sending packet");
		return false;
	}
	return true;
}

int GetCycleTime (void) { return simParams.CycleTime; }

int GetFinalTime (void) { return simParams.SimTime; }

double GetNoiseObstacleSensor (void) { return simParams.ObstacleNoise; }

double GetNoiseBeaconSensor (void) { return simParams.BeaconNoise; }

double GetNoiseCompassSensor (void) { return simParams.CompassNoise; }

double GetNoiseMotors (void) { return simParams.MotorsNoise; }

static bool ConnectToServer (const char *hostname, int port, const char *rob_name, int pos, const double IRSensorAngles[4])
{
	struct hostent *h;
	char conn_packet[MAX_PACKET_LENGTH+1];
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

	return parse_params_packet (conn_packet);
}

static bool parse_params_packet (const char *packet)
{
	char status[10];

	memset (&simParams, 0, sizeof (simParams));

	if (!GetXmlAttribStr (packet, "status", status))
		return false;

	if (!strcmp (status, "refused")) {
		puts ("LibComm::parse_params_packet - Connection explicitly refused by the simulator!");
		return false;
	}
	if (strcmp (status, "ok"))
		puts ("parse_params_packet: status != ok");
	//
	if (!GetXmlAttribInt (packet, "simtime", &simParams.SimTime))
		return false;
	if (!GetXmlAttribInt (packet, "cycletime", &simParams.CycleTime))
		return false;
	if (!GetXmlAttribInt (packet, "nbeacons", &simParams.NBeacons))
		return false;
	//
	if (!GetXmlAttribDouble (packet, "compassnoise", &simParams.CompassNoise))
		return false;
	if (!GetXmlAttribDouble (packet, "beaconnoise", &simParams.BeaconNoise))
		return false;
	if (!GetXmlAttribDouble (packet, "obstaclenoise", &simParams.ObstacleNoise))
		return false;
	if (!GetXmlAttribDouble (packet, "motorsnoise", &simParams.MotorsNoise))
		return false;
//	printf ("<%d\n%d\n%d\n%lf\n%lf\n%lf\n%lf\n>", simParams.SimTime, simParams.CycleTime, simParams.RunningTimeout, simParams.CompassNoise, simParams.BeaconNoise, simParams.ObstacleNoise, simParams.MotorsNoise);
	return true;
}

static bool GetXmlAttribInt (const char *xml, const char *attrib_name, int *dest)
{
	char num[100];

	if (!GetXmlAttribStr (xml, attrib_name, num))
		return false;
	*dest = atoi (num); // TODO - use strtod? more robust, with error checking...
	return true;
}

static bool GetXmlAttribDouble (const char *xml, const char *attrib_name, double *dest)
{
	char num[900];

	if (!GetXmlAttribStr (xml, attrib_name, num))
		return false;
	*dest = atof (num); // TODO - use strtod? more robust, with error checking...
	return true;
}

static bool GetXmlAttribYesNo (const char *xml, const char *attrib_name, bool *dest)
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

static bool GetXmlAttribOnOff (const char *xml, const char *attrib_name, bool *dest)
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

static bool GetXmlAttribStr (const char *xml, const char *attrib_name, char *dest)
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

	strtolwrcase (buf);
	return true;
}

static void strtolwrcase (char *s)
{
	for (; *s; s++)
		*s = tolower (*s);
}


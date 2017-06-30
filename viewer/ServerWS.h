#ifndef _SWS_H_
#define _SWS_H_

class ServerWS;

#include "WSviewer.h"
#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>   // So we got the profile for 10 seconds
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NIL NULL       // A name for the void pointer
#define REMOTE_SERVER_PORT 6000
#define MAX_MSG 1200

void *tServerWS(void *sws);

class ServerWS
{
	public:
		ServerWS(Connection * c);
		void connectAsViewer();
		void update();
		void stopServer();
		void startServer();
			
		Position pos[3];
		Connection *con;
		
		int sd;
		struct sockaddr_in cliAddr, remoteServAddr;
		int rc;
		struct hostent *h;

		bool toStart;
		bool toStop;
};

#endif

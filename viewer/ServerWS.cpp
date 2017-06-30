#include "ServerWS.h"

void* tServerWS(void* w)
{
    ServerWS* sws = (ServerWS*)w;

    sws->connectAsViewer();
    while (1) {
        sws->update();
    }

    return NULL;
}

ServerWS::ServerWS(Connection* c)
{
    con = c;
    toStart = false;
    toStop = false;
}

void ServerWS::stopServer()
{
    int cliLen = sizeof(cliAddr);

    rc = sendto(sd, "<Stop/>\n", strlen("<Stop/>\n") + 1, 0,
        (struct sockaddr*)&(cliAddr),
        cliLen);
}

void ServerWS::startServer()
{
    int cliLen = sizeof(cliAddr);

    rc = sendto(sd, "<Start/>\n", strlen("<Start/>\n") + 1, 0,
        (struct sockaddr*)&(cliAddr),
        cliLen);
}

void ServerWS::update()
{
    char msg[1000];
    int cliLen = (int)sizeof(cliAddr);
    rc = recvfrom(sd, msg, MAX_MSG, 0,
        (struct sockaddr*)&(cliAddr), (socklen_t*)&(cliLen));

    if (toStart) {
        toStart = false;
        startServer();
    } else if (toStop) {
        toStop = false;
        stopServer();
    }
}

void ServerWS::connectAsViewer()
{
    /* get server IP address (no check if input is IP address or DNS name */
    h = gethostbyname(con->getHost());
    if (h == NULL) {
        printf("ServerWS :: %s :: ", con->getHost());
        exit(1);
    }
    remoteServAddr.sin_family = h->h_addrtype;
    memcpy((char*)&remoteServAddr.sin_addr.s_addr,
        h->h_addr_list[0], h->h_length);
    remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

    /* socket creation */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        printf("ServerWS :: Cannot open socket \n");
        exit(1);
    }

    /* bind any port */
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAddr.sin_port = htons(6025);

    rc = bind(sd, (struct sockaddr*)&cliAddr, sizeof(cliAddr));
    if (rc < 0) {
        printf("ServerWS :: cannot bind port\n");
        exit(1);
    }

    rc = sendto(sd, "<View/>", strlen("<View/>"), 0,
        (struct sockaddr*)&remoteServAddr,
        sizeof(remoteServAddr));

    int cliLen = sizeof(cliAddr);

    char msg[1000];
    rc = recvfrom(sd, msg, MAX_MSG, 0,
        (struct sockaddr*)&(cliAddr), (socklen_t*)&(cliLen));
}

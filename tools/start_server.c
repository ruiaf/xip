#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */ 

#define REMOTE_SERVER_PORT 6000
#define MAX_MSG 500

int main(void)
{
  int sd, rc;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;
  char msg[MAX_MSG];
  

  /* get server IP address (no check if input is IP address or DNS name */
  h = gethostbyname("localhost");
  if(h==NULL) {
    printf("unknown host");
    exit(1);
  }
  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
	 h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("cannot open socket \n");
    exit(1);
  }
  
  /* bind any port */
/*  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(6025);
  
  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("cannot bind port\n");
    exit(1);
  }*/
  
  rc = sendto(sd, "<View/>", strlen("<View/>"), 0, 
	      (struct sockaddr *) &remoteServAddr, 
	      sizeof(remoteServAddr));

  int cliLen = sizeof(cliAddr);
  rc = recvfrom(sd, msg, MAX_MSG,0,
		(struct sockaddr *) &cliAddr, &cliLen);  
  
  rc = sendto(sd, "<Start/>\n", strlen("<Start/>\n")+1, 0, 
	      (struct sockaddr *) &cliAddr, 
	      cliLen);
  return 0;
}

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

class Connection;

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "LibComm.h"
#include "defines.h"

using namespace std;

class Connection
{
 public:
  Connection(char *h, char *n, int p);
  
  void connect();
  const char *getHost() { return host; }

 private:
  char host[100];
  char name[20];
  int rob_id;
};

#endif

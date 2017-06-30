#ifndef _SENSOR_R_H_
#define _SENSOR_R_H_

class SensorRequest;

#include "defines.h"
#include "Position.h"
#include <memory.h>

class SensorRequest
{
 public:
  
  SensorRequest()
    {
		memset(ir,0,sizeof(ir));
		compass=false;
		ground=false;
		memset(beacon,0,sizeof(beacon));
    }

  bool ir[4];
  bool compass;
  bool ground;
  bool beacon[10];

 private:
};

#endif

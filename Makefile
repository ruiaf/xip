CC = g++-4.1
CLIBS = -lpthread -lSDL -lSDL_gfx
CFLAGS = -Wall -ansi -O4 -ffast-math -g

OFILES = LibComm.o DiscretePosition.o
OFILES +=  MathTools.o MotorController.o
OFILES +=  Meditator.o viewer/WSviewer.o
OFILES +=  WSmaze.o WSbeacon.o WSmotors.o
OFILES +=  WSsensors.o WSposition.o
OFILES +=  Connection.o Action.o
OFILES +=  WorldState.o Log.o Structures.o
OFILES +=  Path.o WSparameters.o Plan.o
OFILES +=  Mouse.o Position.o WSbeaconsearch.o
OFILES +=  viewer/ServerWS.o

HFILES = Action.h Connection.h Log.h Meditator.h MotorController.h Path.h Plan.h Structures.h WSbeacon.h WSmaze.h WSmotors.h WSparameters.h SensorRequest.h WSposition.h WSsensors.h viewer/WSviewer.h WorldState.h defines.h MathTools.h LibComm.h Mouse.h WSbeaconsearch.h DiscretePosition.h viewer/ServerWS.h

xip:	$(OFILES) $(HFILES) Makefile
	$(CC) -o $@ $(CFLAGS) $(OFILES) $(CLIBS)
	rm -f *~
	rm -f *#
	rm -f core*

.cpp.o: $< $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

profile:
	make "CFLAGS = -Wall -ansi -O4 -ffast-math -pg"

clean:
	rm -f *.o
	rm -f *~
	rm -f viewer/*.o
	rm -f viewer/*~

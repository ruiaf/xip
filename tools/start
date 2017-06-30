#/bin/bash

CIBERTOOLSDIR="/home/raf/ciberrato/ciberToolsShared1.1.1_mdk10.0"
LABDIR="../Labs"

(cd $CIBERTOOLSDIR/Simulator ; simulator -gps -lab ../Labs/$1Lab.xml -grid ../Labs/$1Grid.xml) &
sleep 1

(cd $CIBERTOOLSDIR/Viewer ; Viewer -autoconnect)
killall simulator
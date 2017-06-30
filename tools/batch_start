#!/bin/bash

#1º argumento o nome do labirinto

# se alguem quiser melhorar isto para ler os varios labirintos e
# executar um aleatoriamente e comecar numa posicao aleatoria

XIPDIR=/home/mp3/ciberrato/xip
CIBERTOOLSDIR=/home/mp3/ciberrato/ciberToolsShared1.1.1_mdk10.0
LABDIR="../Labs"

N_EXEC=0

while [ $N_EXEC -le 100 ]
  do
  N_EXEC=$((N_EXEC+1))
  echo $N_EXEC
  
  (cd $CIBERTOOLSDIR/Simulator ; ./simulator -gps -lab ../Labs/$1Lab.xml -grid ../Labs/$1Grid.xml > /dev/null) &
  
  (sleep 15 && ./start_server) &
  
  (cd $XIPDIR ; ((./xip -log -pos 2) >> logfiles/batch_start.log) && killall simulator %> /dev/null )

done

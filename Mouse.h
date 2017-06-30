/*
Copyright (c) 2003-2005
Rui Ferreira, Ricardo Barreira, Nuno Cerqueira, Rui Martins
University of Porto
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the University of Porto nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MOUSE_H_
#define _MOUSE_H_

class Mouse;

#include <getopt.h>

#include "WorldState.h"
#include "Connection.h"
#include "Log.h"
#include "MotorController.h"
#include "Meditator.h"
#include "SensorRequest.h"
#include "viewer/WSviewer.h"

using namespace std;

/**
 * \brief Classe que representa o XIP
 * 
 * Classe central do c�digo, � aqui que este � iniciado.
 * Cont�m o ciclo de processamento.
 */

class Mouse
{
 public:
  
  /**
   * \brief Fun��o onde � iniciado o codigo
   * 
   * Esta � a fun�ao onde come�a a execu��o do agente.
   * Aqui � criado um objecto da classe Mouse e � executado
   * o m�todo de comportamental.
   */
  int main(int argc, char *argv[]);
  
  /**
   * \brief Constructor da classe Mouse.
   * 
   * Aqui � inicializado o agente de acordo com as op��es enviadas na linha
   * de comando
   */
  Mouse(int argc, char *argv[]);
  
  /**
   * \brief Ciclo central de processamento do rato.
   * 
   * Ciclo onde o agente executa o seu processamento.
   * � actualizado o estado interno.
   * � calculada a melhor acção.
   * � executada a acção calculada.
   */
  void behave();
 private:

  /**
   * \brief Parse das op��es de linha de comando.
   * 
   * Aqui s�o analisadas as op��es de linha de comando
   */
  void parse_opt(int argc,char *argv[]);
  
  /**
   * \brief Estado do mundo do agente.
   * 
   * Estado do mundo do agente, organizado por camadas.
   */
  WorldState *ws;
  
  /**
   * \brief Liga��o com o servidor.
   * 
   * Liga��o com o servidor do ciberrato.
   */
  Connection *con;

  /**
   * \brief Log.
   * 
   * Log das ac��es.
   */
  Log *log;
  
  /**
   * \brief Camada de acesso aos motores.
   * 
   * Camada que faz a abstrac��oo da comunica��o com os motores.
   */  
  MotorController * motor;

  /**
   * \brief Camada de decis�o do agente.
   * 
   * Camada que toma as decis�es
   */
  Meditator * med;

  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  char host[30]; 
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  char name[30];
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  char pwsname[30];
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  int pos;
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  bool dolog;
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  bool pws;
 
  /**
   * \brief Op��o de linha de comandos.
   * 
   * Op��o de linha de comandos
   */
  bool wsv;

     ws_and_con_and_log wscon;

};

#endif

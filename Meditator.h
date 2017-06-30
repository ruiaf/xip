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

#ifndef _MEDITATOR_H_
#define _MEDITATOR_H_

class Meditador;

#include "Action.h"
#include "MathTools.h"
#include "WorldState.h"
#include "Structures.h"
#include "Path.h"
#include "Plan.h"
#include "MotorController.h"
#include "Position.h"
#include "SensorRequest.h"

using namespace std;

/** \brief Estado de comportamento em que se encontra o agente
 * 
 * Representa o estado em que se encontra o agente.
 * Pode estar a procurar o farol, a ir para o farol, a sair do farol,
 * ou a voltar para o início.
 */
enum BehaveState {SEARCH_BEACON,GOTO_BEACON,GOTO_RETURN_BEACON,GOTO_START};

#define BBEFORELAST 0
#define BEFORELAST 1
#define LAST 2

/** \brief Classe onde são tomadas as decisões estratégicas do xip
 * 
 * Nesta classe são tomadas as decisões estratégicas.
 * É baseada numa maquina de estados.
 */
class Meditator
{
	public:
		/** \brief Aqui é inicializada a classe
		 * 
		 * Aqui é inicializada a classe
		 */
		Meditator(WorldState *w, MotorController * m);

		/** \brief Determina a acção mais apropriada ao estado do mundo
		 * 
		 * Dependendo do objectivo, estado do mundo determina a melhor acção
		 * a tomar em dado momento.
		 */
		Action * bestAction(void);
		SensorRequest CalculateSensorRequest(void);



	private:
		/** \brief Determina a acção mais apropriada caso esteja à procura do farol */
		Action * actionSearchBeacon();

		/** \brief Determina a acção mais apropriada caso se esteja a dirigir para o farol */
		Action * actionGoToBeacon();

		/** \brief Determina a acção mais apropriada caso se esteja a ir para o beacon de return */
		Action * actionGoToReturnBeacon();

		/** \brief Determina a acção mais apropriada caso se esteja a ir para a posição inicial */
		Action * actionGoToStart();

		/** \brief Determina a acção mais apropriada caso tenha colidido */
		Action * dealWithCollision();  

		/** \brief Retorna verdadeiro caso tenha conseguido encontrar o caminho para o final (A*)
		 *         nos últimos ciclos
		 */
		bool reachedEndinLastCycles() { return notReachedCount < 5; }

		void decideBeaconToReturn();


		/** \brief Último caminho planeado para chegar ao objectivo */
		Path CurrentPath;

		/** \brief Estado do comportamento */
		enum BehaveState behaveState;

		/** \brief Estado do mundo */
		WorldState *ws;

		/** \brief Camada de abstracção do motor */
		MotorController *motor;

		/** \brief gerador de planos */
		Plan *planner;

		/** \brief Serve para caso de o ciclo de acender o led de return
		 *         tenha sido perdido na rede, o estado seja detectado correctamente
		 */
		bool wantedToReturn;

		/** \brief Cancelar o comportamento de tratamento de colisões */
		bool toAbort;

		/** \brief Indica se no retorno o xip vai apenas por caminhos conhecidos
		  ou se vai a explorar caminho
		  */
		bool goingThroughKnownPlaces;

		/** \brief Número de ciclos em que não consegui chegar ao destino no A* */
		int notReachedCount;

		/** \brief Conheço bem o caminho entre o farol e o início */
		bool knowWellWayBetweenObjectives;

		/** \brief Diz se está a ser eficiente */
		bool beingEfficient;
		bool beingVeryEfficient;

		int estimatedCyclesEndKnownPlaces;
		int estimatedCyclesEndUnKnownPlaces;
		int foundTime;
		int lastReturningTime;
		int numberOfCyclesToBeacon;

		WSbeacon * returnBeacon;
};

#endif

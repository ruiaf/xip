#ifndef _Plan_H_
#define _Plan_H_

#define MAX_PATH_SIZE 10000

class Plan;

#include "Action.h"
#include "defines.h"
#include "Structures.h"
#include "WorldState.h"
#include "MotorController.h"
#include "Position.h"
#include "DiscretePosition.h"

#define JUMP 2

using namespace std;

/** \brief Classe onde é possível calcular um novo plano de acção
 *
 * De acordo com o interesse é possível calcular diferentes planos
 * de acção. Seja explorar caminho ou jogar pelo seguro passando
 * apenas por sitios conhecidos
 */
class Plan
{
 public:
  /** \brief Inicialização da classe */
  Plan(WorldState *p,MotorController *m);
  
  /** \brief Retorna a 1ª acção para executar um caminho entre a
   *         posição actual e um distino
   */
  Action * wayTo(const Position & pos);
  Action * wayTo(const Position &originPos, const Position & pos);

  /** \brief Retorna a 1ª acção para executar um caminho entre a
   *         posição actual e um distino passando apenas por
   *         lugares conhecidos
   */
  Action * wayThroughKnownPlacesTo(const Position & pos);
  Action * wayThroughKnownPlacesTo(const Position &originPos, const Position & pos);
  
  /** \brief Retorna a 1ª acção para executar um caminho entre a
   *         posição actual e um distino valorizando a passagem por
   *         lugares desconhecidos
   */
  Action * wayExploringTo(const Position & pos);
  Action * wayExploringTo(const Position &originPos, const Position & pos);
  
  /** \brief Retorna uma lower bound do número de ciclos até ao objectivo */
  int CyclesUntilObjective();
  
  /** \brief Indica se conseguiu encontrar caminho para o destino */
  bool ReachedEnd() { return CouldReach; }
  
  /** \brief Caminho para o objectivo */
  vector<DiscretePosition> routeToDestination;
  
  /** \brief Caminho optimizado para o objectivo */
  vector<DiscretePosition> optimizedRouteToDestination;
 
  bool isAstarPosExp(const DiscretePosition & pos);

  
 private:

  /** \brief Retorna a posição da matriz correspondente */
  Path * getPath(const DiscretePosition & pos) { return &(travelCost[pos.getX()][pos.getY()]); }
  
  /** \brief Inicializa a matriz de travelCost */
  void initTravelCost();
  
  /** \brief Limpa a matriz de travelCost */
  void clearTravelCost(const DiscretePosition &origin,
		       const DiscretePosition &dest);

  /** \brief Encontra caminho pela matriz */
  void findRoute(DiscretePosition origin, DiscretePosition destination); 

  /** \brief Optimiza um caminho determinado */
  void optimizeRoute();

  /** \brief Indica o ponto seguinte do caminho optimizado */
  int getNextOptimizedPoint();

  
  /** \brief Verifica se é possível optimizar um caminho entre inicio e ind */
  bool tryWayToPoint(int ind);
  
  /** \brief Estado do mundo */
  WorldState *ws;
  
  /** \brief Camada de acesso aos motores */
  MotorController *motor;
  
  /** \brief Indica se conseguiu encontrar caminho para o destino */
  bool CouldReach;
  
  /** \brief Tabela que indica o custo (e outra informação da pesquisa) de chegar a dado local */
  Path travelCost[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];

  /** \brief informacao para limpar o travelcost antes de cada pesquisa*/
  Path cleanTravelCost[2*LAB_X*MAZE_DEF][2*LAB_Y*MAZE_DEF];
  
  /** \brief Índice do último ponto optimizado */
  int optimizedIndex;

  friend class WSV;
};

#endif

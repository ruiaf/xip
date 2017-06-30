#include "Plan.h"

Plan::Plan(WorldState * w, MotorController * m)
{
  ws=w;
  motor=m;
  CouldReach = true;
  initTravelCost();
}

Action * Plan::wayTo(const Position & pos) { 
  return wayTo(ws->getPosition()->getPos(),pos); }

Action * Plan::wayThroughKnownPlacesTo(const Position & pos) {
   return wayThroughKnownPlacesTo(ws->getPosition()->getPos(),pos); }

Action * Plan::wayExploringTo(const Position & pos) { 
  return wayExploringTo(ws->getPosition()->getPos(),pos); }


int Plan::CyclesUntilObjective()
{
	double cycles=0.0;
	Position ant,cur;

	if (optimizedRouteToDestination.size())
	{
		ant = optimizedRouteToDestination[0].getPosition();

		for (unsigned i=1; i < optimizedRouteToDestination.size() && i < 1000 ; i++)
		{
			cur = optimizedRouteToDestination[i].getPosition();

			cycles += hypot(ant.getX()-cur.getX(),ant.getY()-cur.getY());
			ant=cur;
		}
		cycles = (cycles/0.15);
	}
	return (int) cycles;
}


Action * Plan::wayTo(const Position &originPos, const Position &destinationPos)
{ 
	CouldReach = true; // Por defeito marca como tendo chegado ao final

	DiscretePosition origin(originPos);
	DiscretePosition destination(destinationPos);

	clearTravelCost(origin,destination);

	Heap * pq = new Heap(); // pqueue para o A*
	Path * node = getPath(origin);
	pq->insert(node);
	node->setExpansioned();
	node = pq->deleteMin();
	if (node->isFinal()) goto skipWayTo; // big hack! corrigir isto!

	//agora a contar com a direcção inicial para a hesitação de planos...
	{
		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);

					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							!sucessor->isExpansioned() &&
							((cost_inc = sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos)),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
						double turn_overhead = (fabs(MathTools::calc_angle(
								node->getPosition().getX(),
								node->getPosition().getY(),
								sucessor->getPosition().getX(),
								sucessor->getPosition().getY())) < 91)/(5);
						sucessor->update(node,cost_inc+turn_overhead); 
						pq->insert(sucessor);
					}
				}
			}  
	}

	while(true)
	{   
		do
		{
			if (pq->isEmpty())
			{
				printf("Failed to find a way until objective! :(\n");
				CouldReach = false;
				return new Action(0.05,-0.05);
			}
			node = pq->deleteMin();

		}
		while (!ws->getMaze()->validToGoWithMouse(node->getPosition()) ||
				node->isExpansioned());

		node->setExpansioned();

		if (node->isFinal()) break; // consegui chegar ao destino! :D

		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);

					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							!sucessor->isExpansioned() &&
							((cost_inc = sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos)),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
						sucessor->update(node,cost_inc); 
						pq->insert(sucessor);
					}
				}
			}
	}

skipWayTo:
	
	delete pq; // apagar a pqueue

	findRoute(origin,node->getPosition());
	optimizeRoute();

	if (optimizedRouteToDestination.size()>1) // se ainda não estivermos no destino..
	{
		if (ws->getMaze()->goodToGoWithMouse(optimizedRouteToDestination[1]))
			return motor->goSlowToXY(optimizedRouteToDestination[1].getPosition());
		else return motor->goToXY(optimizedRouteToDestination[1].getPosition());
	}

	return new Action(0.0,0.0);
}

Action * Plan::wayThroughKnownPlacesTo(const Position &originPos, const Position &destinationPos)
{ 
	CouldReach = true; // Por defeito marca como tendo chegado ao final

	DiscretePosition origin(originPos);
	DiscretePosition destination(destinationPos);

	clearTravelCost(origin,destination);

	Heap * pq = new Heap(); // pqueue para o A*
	Path * node = getPath(origin);
	
	pq->insert(node);
	node->setExpansioned();
	node = pq->deleteMin();
	if (node->isFinal()) goto skipWayTKPTo; // big hack! corrigir isto!

	//agora a contar com a direcção inicial para a hesitação de planos...
	{
		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);

					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							!sucessor->isExpansioned() &&
							((cost_inc = sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos)),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
							double turn_overhead = (fabs(MathTools::calc_angle(
								node->getPosition().getX(),
								node->getPosition().getY(),
								sucessor->getPosition().getX(),
								sucessor->getPosition().getY())) < 91)/(5);
						sucessor->update(node,cost_inc+turn_overhead); 
						pq->insert(sucessor);
					}
				}
			}  
	}

	
	
	while(true)
	{   
		do
		{
			if (pq->isEmpty())
			{
				printf("Failed to find a way until objective! :(\n");
				CouldReach = false;
				return new Action(0.05,-0.05);
			}
			node = pq->deleteMin();

		}
		while (!ws->getMaze()->validToGoWithMouse(node->getPosition()) ||
				node->isExpansioned());

		node->setExpansioned();

		if (node->isFinal()) break; // consegui chegar ao destino! :D

		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);
					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							ws->getMaze()->isKnown(newPos) &&
							!sucessor->isExpansioned() &&
							((cost_inc = sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos)),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
						sucessor->update(node,cost_inc);
						pq->insert(sucessor);
					}
				}
			}
	}

skipWayTKPTo:
	delete pq; // apagar a pqueue

	findRoute(origin,node->getPosition());
	optimizeRoute();

	if (optimizedRouteToDestination.size()>1) // se ainda não estivermos no destino..
	{
		if (ws->getMaze()->goodToGoWithMouse(optimizedRouteToDestination[1]))
			return motor->goSlowToXY(optimizedRouteToDestination[1].getPosition());
		else return motor->goToXY(optimizedRouteToDestination[1].getPosition());
	}

	return new Action(0.0,0.0);
}

Action * Plan::wayExploringTo(const Position &originPos, const Position &destinationPos)
{ 
	CouldReach = true; // Por defeito marca como tendo chegado ao final

	DiscretePosition origin(originPos);
	DiscretePosition destination(destinationPos);

	clearTravelCost(origin,destination);

	Heap * pq = new Heap(); // pqueue para o A*
	Path * node = getPath(origin);

		pq->insert(node);
	node->setExpansioned();
	node = pq->deleteMin();
	if (node->isFinal()) goto skipWayExTo; 
	//agora a contar com a direcção inicial para a hesitação de planos...
	{
		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);

					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							!sucessor->isExpansioned() &&
							((cost_inc = sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos)),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
							double turn_overhead = (fabs(MathTools::calc_angle(
								node->getPosition().getX(),
								node->getPosition().getY(),
								sucessor->getPosition().getX(),
								sucessor->getPosition().getY())) < 91)/(5);
						sucessor->update(node,cost_inc+turn_overhead); 
						pq->insert(sucessor);
					}
				}
			}  
	}

	while(true)
	{   
		do
		{
			if (pq->isEmpty())
			{
				printf("Failed to find a way until objective! :(\n");
				CouldReach = false;
				return new Action(0.05,-0.05);
			}
			node = pq->deleteMin();

		}
		while (!ws->getMaze()->validToGoWithMouse(node->getPosition()) ||
				node->isExpansioned());

		node->setExpansioned();

		if (node->isFinal()) break; // consegui chegar ao destino! :D

		Path * sucessor;
		double cost_inc;

		// isto está confuso por causa das optimizações... (teve q ser)
		for (int i=-JUMP; i<=JUMP;i+=JUMP)
			for (int j=-JUMP; j<=JUMP;j+=JUMP)
			{
				if (!(i==0 && j==0)) {
					DiscretePosition newPos(node->getPosition().getX() + i,
							node->getPosition().getY() + j);
					sucessor = getPath(newPos);
					if (ws->getMaze()->CanGoWithMouse(newPos) &&
							!sucessor->isExpansioned() &&
							(cost_inc = (sqrt((double)(i*i+j*j)) + ws->getMaze()->goodToGoWithMouse(newPos))*(ws->getMaze()->isKnown(newPos)?5:1),
							 !sucessor->beenVisited() || sucessor->getValue() > node->getValue() + cost_inc))
					{
						sucessor->update(node,cost_inc); 
						pq->insert(sucessor);
					}
				}
			}
	}

skipWayExTo:
	delete pq; // apagar a pqueue

	findRoute(origin,node->getPosition());
	optimizeRoute();


	if (optimizedRouteToDestination.size()>1) // se ainda não estivermos no destino..
	{
		if (ws->getMaze()->goodToGoWithMouse(optimizedRouteToDestination[1]))
			return motor->goSlowToXY(optimizedRouteToDestination[1].getPosition());
		else return motor->goToXY(optimizedRouteToDestination[1].getPosition());
	}

	return new Action(0.0,0.0);
}


void Plan::findRoute(DiscretePosition origin, DiscretePosition destination)
{
	routeToDestination.clear();
	routeToDestination.push_back(destination);

	Path * node = getPath(destination);

	while(node!=NULL)
	{
		routeToDestination.push_back(node->getPosition());
		//    printf("%.3lf, ",node->getCost());
		node = node->getBefore();
	}

	// faz reverse da route

	DiscretePosition tmp;
	int i=0,j=routeToDestination.size()-1;

	while(i<j)
	{
		tmp=routeToDestination[i];
		routeToDestination[i]=routeToDestination[j];
		routeToDestination[j]=tmp;
		i++; j--;
	}
}

void Plan::initTravelCost()
{
	for (int i=0;i<2*LAB_X*MAZE_DEF;i++)
		for (int j=0;j<2*LAB_Y*MAZE_DEF;j++)
		{
			DiscretePosition cur(i,j);
			travelCost[i][j].init(cur);
		}
	memcpy(cleanTravelCost,travelCost,sizeof(cleanTravelCost));
}

void Plan::clearTravelCost(const DiscretePosition &origin, const DiscretePosition &dest)
{

	/*
	for (int i=0;i<2*LAB_X*MAZE_DEF;i+=1)
		for (int j=0;j<2*LAB_Y*MAZE_DEF;j+=1)
		{
			travelCost[i][j].clear(dest);
		}
	*/

	memcpy(travelCost,cleanTravelCost,sizeof(cleanTravelCost));
	travelCost[0][0].clear(dest);
}

void Plan::optimizeRoute()
{
	int pt;

	optimizedIndex=0;
	optimizedRouteToDestination.clear();

	optimizedRouteToDestination.push_back(routeToDestination.front());

	while ((pt = getNextOptimizedPoint()) >= 0)
		optimizedRouteToDestination.push_back(routeToDestination[pt]);

	optimizedRouteToDestination.push_back(routeToDestination.back());
}

int Plan::getNextOptimizedPoint()
{
	int route_size = routeToDestination.size()-1;
	optimizedIndex++;

	while (optimizedIndex < route_size &&
			tryWayToPoint(optimizedIndex)) optimizedIndex++;
	if (optimizedIndex>=route_size)
		return -1;
	return optimizedIndex;
}

bool Plan::isAstarPosExp(const DiscretePosition & pos)
{
	return getPath(pos)->isExpansioned();
}


#define SWAP(A,B) {t=A; A=B; B=t;}

bool Plan:: tryWayToPoint(int ind)
{
	int x,y,t;
	int x0 = optimizedRouteToDestination.back().getX();
	int y0 = optimizedRouteToDestination.back().getY();
	int x1 = routeToDestination[ind].getX();
	int y1 = routeToDestination[ind].getY();
	
	bool steep = abs(y1-y0)>abs(x1-x0);

	if (steep) {
		SWAP(x0,y0);
		SWAP(x1,y1);
	}
	if (x0 > x1) {
		SWAP(x0,x1);
		SWAP(y0,y1);
	}
	
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);

	int error=0;
	int ystep;
	y=y0;

	if (y0<y1) 
		ystep = 1;
	else
		ystep = -1;

	for (x=x0; x<x1;x++) {
		if (steep) {
			if (ws->getMaze()->goodToOptimizeWithMouse(y,x) > 0.01)
					return false;
		}
		else {
			if (ws->getMaze()->goodToOptimizeWithMouse(x,y) > 0.01)
					return false;
		}
		error+=deltay;
		if (2*error >= deltax) {
			y+=ystep;
			error-=deltax;
		}
	}
	
	return true;
}

/*


bool Plan:: tryWayToPoint_old(int ind)
{
	double x,y;
	double x1 = optimizedRouteToDestination.back().getX();
	double y1 = optimizedRouteToDestination.back().getY();
	double x2 = routeToDestination[ind].getX();
	double y2 = routeToDestination[ind].getY();
	
	double deltax = x2 - x1;                   // The difference in the x's
	double deltay = y2 - y1;                   // The difference in the y's
	double m,b,sinal, t;
	bool troca;


	if (deltay<deltax)
	{
		troca = false;
	}
	else
	{ 
		troca = true;
		SWAP(x1,y1)
			SWAP(x2,y2)
			SWAP(deltax,deltay)
	}
	sinal = (deltax >= 0) ? 1 : -1;
	m = deltay / deltax;
	b=y1-m*x1;
	if (fabs(deltax)<0.01)
	{
		sinal = (deltay >= 0) ? 1 : -1;
		for (y = 1; y <= fabs(deltay); y++)
		{
			x = x1;                 // Calculate the y-value
			if (troca)
			{
				if (ws->getMaze()->goodToOptimizeWithMouse( (int)(x),int(y*sinal + y1)) > 0.01)
					return false;
			}
			else if (ws->getMaze()->goodToOptimizeWithMouse( (int)(y*sinal + y1),int(x)) > 0.01)
				return false;
		}
	}
	else
	{
		for (x = 1; x <= fabs(deltax); x++)
		{
			y = m*(x*sinal+x1)+b;                        // Calculate the y-value
			if (troca) {
				if (ws->getMaze()->goodToOptimizeWithMouse(int(round(y)), (int)(sinal*x + x1)) > 0.01)
					return false;
			}
			else if (ws->getMaze()->goodToOptimizeWithMouse((int)(sinal*x + x1),int(round(y))) > 0.01)
				return false;
		}
	}

	return true;
}
*/
#undef SWAP

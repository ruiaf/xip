#include "Action.h"

Action::Action(double l,double r)
{
	actiontype=MOTOR;
	l = max((double)-0.150,l);
	l = min((double)0.150,l);
	r = max((double)-0.150,r);
	r = min((double)0.150,r);

	lPower=floor(l*1000+0.5)/1000;
	rPower=floor(r*1000+0.5)/1000;
	beaconToVisit=-1;
	
}

Action::Action(enum atype a, int bv=-1)
{
	actiontype=a;
	lPower=0.0;
	rPower=0.0;
	beaconToVisit=bv;
}

Action::Action(enum atype a,double l, double r)
{
	actiontype=a;
	l = max((double)-0.15,l);
	l = min((double)0.15,l);
	r = max((double)-0.15,r);
	r = min((double)0.15,r);

	lPower=floor(l*1000+0.5)/1000;
	rPower=floor(r*1000+0.5)/1000;
	beaconToVisit=-1;
}

void Action::setType(enum atype a,int bv)
{
	if(actiontype!=RETURN)
		actiontype = a;
	beaconToVisit=bv;
}

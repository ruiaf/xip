#include "MotorController.h"

#define MIN_ROT_VAL 2
#define MAX_ROT_VAL 60

#define NEAR_BOT_DIST 0.3
#define TIME_MOUSE_AHEAD 100

// FUZZY STUFF
#define MIN_CONSID_LEFT (0.00)
#define CONSID_LEFT (0.04)
#define GOING_LEFT(X) (((X) > CONSID_LEFT) ? (1) : (((X) < MIN_CONSID_LEFT) ? (0) : (((X)-MIN_CONSID_LEFT) / (CONSID_LEFT - MIN_CONSID_LEFT))))
#define GOING_RIGHT(X) (GOING_LEFT(-(X)))

#define NEAR_WALL_DIST (0.20)
#define MIN_CONSID_NEAR_WALL_DIST (0.4)
#define WALL_CLOSE(X) (((X) < NEAR_WALL_DIST) ? (1) : (((X) > MIN_CONSID_NEAR_WALL_DIST) ? (0) : (((X)-MIN_CONSID_NEAR_WALL_DIST) / (NEAR_WALL_DIST - MIN_CONSID_NEAR_WALL_DIST))))

MotorController::MotorController(WorldState* w, Log* l)
{
    ws = w;
    log = l;
    wasLastActionCorrected = false;
    listChangedActions.empty();
}

request_t MotorController::transformRequest(SensorRequest sr)
{
    request_t r;
    for (int i = 0; i < 4; i++)
        r.IRSensorRequest[i] = sr.ir[i];
    r.GroundRequest = sr.ground;
    r.CompassRequest = sr.compass;
    for (int i = 0; i < ws->getParameters()->getNBeacons(); i++)
        r.BeaconRequest[i] = sr.beacon[i];
    return r;
}

void MotorController::requestSensors(SensorRequest r)
{
    SetRequest(transformRequest(r));
    DriveMotors(ws->getMotors()->getInertiaLeft(), ws->getMotors()->getInertiaRight());
}

void MotorController::drive(Action* act, SensorRequest r)
{
    SetRequest(transformRequest(r));
    if (act->toDrive()) {
        ws->setActionSent(DriveMotors(act->getlPower(), act->getrPower()));
        ws->getMotors()->action_done(act);
    } else if (act->toReturn()) {
        ws->setActionSent(SetReturningLed2(act->getlPower(), act->getrPower()));
        ws->getMotors()->action_done(act);
    } else if (act->toEnd()) {
        ws->setActionSent(Finish2(act->getlPower(), act->getrPower()));
        ws->getMotors()->action_done(act);
        puts("I'm back in grid :D! iuupppi!");
    } else if (act->toVisit()) {
        ws->setActionSent(SetVisitingLed2(act->getlPower(), act->getrPower()));
        ws->getMotors()->action_done(act);
        puts("It's visited!");
    }

    if (ws->ActionSent()) {
        ws->getPosition()->PredictPosition(act);
        updateChangedActions();
        char debug[256];
        sprintf(debug, "ACTION - (%lf,%lf)", act->getlPower(), act->getrPower());
        log->writeDebug(debug);
    }
    if (!ws->ActionSent() && act->toVisit())
        ws->getBeacon(act->getBeaconToVisit())->setUnvisited();

    hist_action[ws->getTime()][0] = act->getlPower();
    hist_action[ws->getTime()][0] = act->getrPower();
}

//isto está feio
Action* MotorController::goToXY(double x_final, double y_final)
{
    double pLeft, pRight, pRightLeft_diff, pRightLeft_sum;
    double iLeft, iRight;
    double heading, north;
    double trueHeading, trueMovement;
    double maxRotation, maxMovement;
    bool max_rot, min_rot;
    WSmotors* motors = ws->getMotors();
    WSposition* position = ws->getPosition();
    WSsensors* sensors = ws->getSensors();

    wasLastActionCorrected = false;
    heading = MathTools::calc_angle(position->getX(), position->getY(), x_final, y_final);

    north = position->getDirDeg();

    heading = MathTools::normalize_180_to_180(heading - north);
    trueHeading = heading = MathTools::deg2rad(heading);

    iLeft = motors->getInertiaLeft();
    iRight = motors->getInertiaRight();

    max_rot = (fabs(heading) > MathTools::deg2rad(MAX_ROT_VAL));
    min_rot = (!max_rot) && (fabs(heading) < MathTools::deg2rad(MIN_ROT_VAL));

    if (!min_rot) {
        if (heading >= 0.0) // turn left
            maxRotation = motors->getMaxPowerLeftTurn(iLeft, iRight, &pLeft, &pRight);
        else // turn right
            maxRotation = motors->getMaxPowerRightTurn(iLeft, iRight, &pLeft, &pRight);

        if (trueHeading > maxRotation)
            trueHeading = maxRotation;
        else if (trueHeading < -maxRotation)
            trueHeading = -maxRotation;
    } else
        trueHeading = 0;

    if (!max_rot) {
        trueMovement = MathTools::distance(position->getX(), position->getY(), x_final, y_final);
        maxMovement = motors->getMaxPowerFront(iLeft, iRight, &pLeft, &pRight);
        if (trueMovement > maxMovement)
            trueMovement = maxMovement;
    } else
        trueMovement = 0;

    // FUZZY LOGIC COLLISION DETECTION
    if (!max_rot && fabs(trueMovement) > 0.01) {
        double gl = GOING_LEFT(trueHeading), wcl = WALL_CLOSE(sensors->getLeftDist());
        double factor_normalized_l = gl * wcl * wcl;
        double gr = GOING_RIGHT(trueHeading), wcr = WALL_CLOSE(sensors->getRightDist());
        double factor_normalized_r = gr * wcr * wcr;
        double wccl = WALL_CLOSE(sensors->getCenterLeftDist());
        double factor_normalized_cl = wccl;
        double wccr = WALL_CLOSE(sensors->getCenterRightDist());
        double factor_normalized_cr = wccr;

        double truehead_old = trueHeading;
        double truemove_old = trueMovement;

        /////////////////// front obstacle

        trueMovement = -0.15 * factor_normalized_cl * factor_normalized_cr + trueMovement * (1 - factor_normalized_cl * factor_normalized_cr);
        trueMovement = -0.03 * factor_normalized_cr + trueMovement * (1 - factor_normalized_cr);
        trueMovement = -0.03 * factor_normalized_cl + trueMovement * (1 - factor_normalized_cl);
        if (factor_normalized_cl > 0.8 || factor_normalized_cr > 0.8)
            wasLastActionCorrected = true;

        ///////////////////// side obstacle
        trueHeading = (((-(2.0 - wcr) * trueHeading * factor_normalized_l + trueHeading * (1 - factor_normalized_l)) + (-(2.0 - wcl) * trueHeading * factor_normalized_r + trueHeading * (1 - factor_normalized_r)))) / 2;

        char debug[255];
        sprintf(debug, "COLISION - action(h=%.4lf,m=%.4lf->h=%.4lf,m=%.4lf) gl(%.2lf) wl(%.2lf) gr(%.2lf) wr(%.2lf) wc(%.4lf,%.4lf)", truehead_old, truemove_old, trueHeading, trueMovement, gl, wcl, gr, wcr, wccl, wccr);
        log->writeDebug(debug);
    }

    pRightLeft_diff = 2 * trueHeading - iRight + iLeft;
    pRightLeft_sum = 4 * trueMovement - iRight - iLeft;

    pLeft = (pRightLeft_sum - pRightLeft_diff) / 2.0;
    pRight = pRightLeft_sum + pRightLeft_diff;

    //	char debug[255];
    //	sprintf(debug,"Desired power - (%.4lf,%.4lf)",pLeft,pRight);
    //	log->writeDebug(debug);

    return new Action(pLeft, pRight);
}

Action* MotorController::goSlowToXY(double x_final, double y_final)
{
    return goToXY(x_final, y_final);
}

// no dock faz march atras caso seja necessário
//isto está feio
Action* MotorController::dockXY(double x_final, double y_final)
{
    double pLeft, pRight, pRightLeft_diff, pRightLeft_sum;
    double iLeft, iRight;
    double heading, north;
    double trueHeading, trueMovement;
    double maxRotation, maxMovement;
    bool max_rot, min_rot, rearMove = false;
    WSmotors* motors = ws->getMotors();
    WSposition* position = ws->getPosition();
    WSsensors* sensors = ws->getSensors();

    wasLastActionCorrected = false;
    heading = MathTools::calc_angle(position->getX(), position->getY(), x_final, y_final);

    north = position->getDirDeg();

    heading = MathTools::normalize_180_to_180(heading - north);

    if (heading > 90) {
        rearMove = true;
        heading = MathTools::normalize_180_to_180(heading - 180);
    }

    trueHeading = heading = MathTools::deg2rad(heading);

    iLeft = motors->getInertiaLeft();
    iRight = motors->getInertiaRight();

    min_rot = (fabs(heading) < MathTools::deg2rad(MIN_ROT_VAL));
    max_rot = false;

    if (!min_rot) {
        if (heading >= 0.0) // turn left
            maxRotation = motors->getMaxPowerLeftTurn(iLeft, iRight, &pLeft, &pRight);
        else // turn right
            maxRotation = motors->getMaxPowerRightTurn(iLeft, iRight, &pLeft, &pRight);

        if (trueHeading > maxRotation)
            trueHeading = maxRotation;
        else if (trueHeading < -maxRotation)
            trueHeading = -maxRotation;
    } else
        trueHeading = 0;

    if (!max_rot) {
        trueMovement = MathTools::distance(position->getX(), position->getY(), x_final, y_final);
        maxMovement = motors->getMaxPowerFront(iLeft, iRight, &pLeft, &pRight);
        if (trueMovement > maxMovement)
            trueMovement = maxMovement;
    } else
        trueMovement = 0;

    if (!max_rot && fabs(trueMovement) > 0.00001) {
        if (sensors->getLeftDist() < NEAR_WALL_DIST && trueHeading > 0) {
            trueHeading = -trueHeading;
            //printf ("Avoid dock l *** %lf\n", trueHeading);
        }
        if (sensors->getRightDist() < NEAR_WALL_DIST && trueHeading < 0) {
            trueHeading = -trueHeading;
            //printf ("Avoid dock r *** %lf\n", trueHeading);
        }

        if (trueHeading < 0) { // direita
            if (sensors->getCenterRightDist() < NEAR_BOT_DIST) {
                if (sensors->getCenterLeftDist() < NEAR_BOT_DIST) {
                    trueMovement = 0;
                    //puts ("Cheira-me a rato!");
                    //printf ("Avoid dock stopr *** %lf\n", trueHeading);
                    wasLastActionCorrected = true;
                } else {
                    trueMovement = 0;
                    //printf ("Avoid dock cr *** %lf\n", trueHeading);
                    wasLastActionCorrected = true;
                }
            } else if (sensors->getCenterLeftDist() < NEAR_BOT_DIST) {
                trueMovement = 0;
                //printf ("Avoid dock cnr *** %lf\n", trueHeading);
                wasLastActionCorrected = true;
            }
        } else { // esquerda
            if (sensors->getCenterLeftDist() < NEAR_BOT_DIST) {
                if (sensors->getCenterRightDist() < NEAR_BOT_DIST) {
                    trueMovement = 0;
                    //puts ("Cheira-me a rato!");
                    //printf ("Avoid dock stopl *** %lf\n", trueHeading);
                    wasLastActionCorrected = true;
                } else {
                    trueMovement = 0;
                    //printf ("Avoid dock cl *** %lf\n", trueHeading);
                    wasLastActionCorrected = true;
                }
            } else if (sensors->getCenterRightDist() < NEAR_BOT_DIST) {
                trueMovement = 0;
                //printf ("Avoid dock cnl *** %lf\n", trueHeading);
                wasLastActionCorrected = true;
            }
        }
    }

    if (rearMove)
        trueMovement = -trueMovement;

    pRightLeft_diff = 2 * trueHeading - iRight + iLeft;
    pRightLeft_sum = 4 * trueMovement - iRight - iLeft;

    pLeft = (pRightLeft_sum - pRightLeft_diff) / 2.0;
    pRight = pRightLeft_sum - pLeft;
    return new Action(pLeft, pRight);
}

bool MotorController::isMouseStoppedAhead()
{
    return listChangedActions.size() > 15;
}

void MotorController::updateChangedActions()
{
    if (wasLastActionCorrected)
        listChangedActions.push_back(ChangedAction(ws->getTime(),
            ws->getPosition()->getX(),
            ws->getPosition()->getY()));
    while (!listChangedActions.empty()) {
        ChangedAction c = listChangedActions.front();

        if (c.time + TIME_MOUSE_AHEAD < ws->getTime() || MathTools::distance(c.x, c.y, ws->getPosition()->getX(), ws->getPosition()->getY()) > 0.5) {
            listChangedActions.pop_front();
        } else
            break;
    }
}

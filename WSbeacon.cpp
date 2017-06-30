#include "WSbeacon.h"

#include <string.h>

using namespace std;

WSbeacon::WSbeacon(WorldState* w, int bindex)
{
    ws = w;
    measure.beaconVisible = false;
    beacon_index = bindex;
    beenSetVisited = false;

    memset(beacon_prob, 0, sizeof(beacon_prob));
    memset(sensor_prob, -1, sizeof(sensor_prob));
    memset(samples, 0, sizeof(samples));
    init_prob();
    createSensorMatrix();
    know_point = false;
}

// TODO: ACRESCENTAR OS VALORES DOS SENSORES DE FAROL DOS OUTROS RATOS
//		 O QUE É MELHOR??? COMUNICAR O SENSOR OU A POSIÇÃO CALCULADA NO LABIRINTO?
void WSbeacon::update(bool isBeaconReady)
{
    if (!ws->getSensors()->inGround(beacon_index))
        clearMouse();

    if (isBeaconReady) {
        measure = GetBeaconSensor(beacon_index);
        if (measure.beaconVisible) {

            char debug[256];
            sprintf(debug, "BEACON DIR %lf", measure.beaconDir);
            ws->log->writeDebug(debug);

            if (!ws->getSensors()->inGround(beacon_index))
                applySensor(ws->getPosition()->getLagDirDeg() + measure.beaconDir);
            else if (ws->getSensors()->hasGround())
                clearArroundBeacon();
        }
    }
    findBest();
}

double WSbeacon::getX()
{

    if (ws->getSensors()->inGround(beacon_index))
        return ws->getPosition()->getX();
    return best_point.getPosition().getX();
}

double WSbeacon::getY()
{

    if (ws->getSensors()->inGround(beacon_index))
        return ws->getPosition()->getY();
    return best_point.getPosition().getY();
}

char WSbeacon::getVal(double x, double y)
{
    DiscretePosition pos(Position(x, y));
    return beacon_prob[pos.getX()][pos.getY()];
}

Position WSbeacon::getPosition()
{
    Position pos = best_point.getPosition();
    ws->getMaze()->setToBounds(pos);
    return pos;
}

void WSbeacon::markCurrentPositionAsUnreachable()
{
    /// TODO:: WTF??? WHY IS THIS EMPTY?
}

void WSbeacon::init_prob(void)
{
    int val;
    double x, aux;

    prob_acc[0] = 0;
    double error = ws->getParameters()->getCompassNoise();

    for (val = 1; val < 200; val++) {
        x = (double)(val - 100);
        aux = exp(-(x * x) / (2 * error * error));
        prob_acc[val] = prob_acc[val - 1] + (aux / (error * sqrt((double)2 * PI)));
    }
}

double WSbeacon::distri_normal_acc(double dir, double dirmax)
{

    double p1, p2;

    if (dir < -95)
        p1 = 0;
    else if (dir >= 95)
        p1 = 1;
    else
        p1 = prob_acc[(int)round(dir) + 100];

    if (dirmax < -95)
        p2 = 0;
    else if (dirmax >= 95)
        p2 = 1;
    else
        p2 = prob_acc[(int)round(dirmax) + 100];

    return p2 - p1;
}

void WSbeacon::createSensorMatrix()
{
    int alpha, beta, i, j;
    double ang, angdif;

    int midx = LAB_X * MAZE_DEF;
    int midy = LAB_Y * MAZE_DEF;

    for (int dir = 0; dir <= 360; dir++) {
        alpha = dir - 90;
        beta = dir + 90;

        for (i = 0; i < 2 * LAB_X * MAZE_DEF; i++)
            for (j = 0; j < 2 * LAB_Y * MAZE_DEF; j++) {
                ang = MathTools::calc_angle(midx, midy, i, j);
                angdif = MathTools::angle_diff(dir, ang);
                //double normdir = MathTools::normalize_180_to_180(dir);
                //double dif = min(fabs(ang-dir),fabs(dir-ang));
                //double dist = sqrt((double) (i-midx)*(i-midx)+(j-midy)*(j-midy));
                if (MathTools::angle_between(ang, alpha, beta)) {
                    sensor_prob[dir][i][j] = (char)(distri_normal_acc(angdif - 15, angdif + 15) * 100);
                } else {
                    sensor_prob[dir][i][j] = 0;
                }
            }
    }
}

void WSbeacon::applySensor(double dir)
{
    int i, j;

    dir = MathTools::normalize_0_to_360((int)round(dir));
    DiscretePosition cur(ws->getPosition()->getLagPos());
    int mydir = (int)round(dir);

    for (i = 0; i < 2 * LAB_X * MAZE_DEF; i++)
        for (j = 0; j < 2 * LAB_Y * MAZE_DEF; j++)
            setBeaconMatPos(cur.getX() + i - LAB_X * MAZE_DEF,
                cur.getY() + j - LAB_Y * MAZE_DEF,
                sensor_prob[mydir][i][j]);
}

void WSbeacon::clearArroundBeacon()
{
    int i, j;

    DiscretePosition cur(ws->getPosition()->getPos());

    for (i = 0; i < 2 * LAB_X * MAZE_DEF; i++)
        for (j = 0; j < 2 * LAB_Y * MAZE_DEF; j++)
            if (ws->getPosition()->getPos().distance(
                    DiscretePosition(cur.getX() + i - LAB_X * MAZE_DEF,
                        cur.getY() + j - LAB_Y * MAZE_DEF)
                        .getPosition())
                > 1.5)
                setBeaconMatPos(cur.getX() + i - LAB_X * MAZE_DEF,
                    cur.getY() + j - LAB_Y * MAZE_DEF,
                    0);
}

void WSbeacon::setBeaconMatPos(int xmazepos, int ymazepos, char val)
{
    if (!ws->getMaze()->inMatrixBounds(xmazepos, ymazepos))
        return;
    //int cp = beacon_prob[xmazepos][ymazepos];
    //double peso=samples[xmazepos][ymazepos]/(samples[xmazepos][ymazepos]+1);
    if (samples[xmazepos][ymazepos] == 0)
        beacon_prob[xmazepos][ymazepos] = val;
    else
        beacon_prob[xmazepos][ymazepos] = min((beacon_prob[xmazepos][ymazepos] * val) / 80, 100);
    samples[xmazepos][ymazepos]++;
}

void WSbeacon::findBest()
{
    double best_point_value_tmp = 0;
    double best_point_dist = 999999999;
    DiscretePosition mypos = DiscretePosition(ws->getPosition()->getPos());
    // TODO: não pode ser de 0 ao LAB_X! tem de ser do min maze pos ao max maze pos actual
    for (int i = (int)(RAIO_FAROL * MAZE_DEF); i < 2 * LAB_X * MAZE_DEF - RAIO_FAROL * MAZE_DEF; i++)
        for (int j = int(RAIO_FAROL * MAZE_DEF); j < 2 * LAB_Y * MAZE_DEF - RAIO_FAROL * MAZE_DEF; j++) {
            double dist = abs(mypos.getX() - i) + abs(mypos.getY() - j);
            if ((beacon_prob[i][j] > best_point_value_tmp || (beacon_prob[i][j] == best_point_value_tmp && dist < best_point_dist)) && ws->getMaze()->goodToGoWithMouse(i, j) < 3) {
                best_point_value = beacon_prob[i][j];
                best_point_value_tmp = beacon_prob[i][j];
                best_point_dist = dist;
                best_point.setX(i);
                best_point.setY(j);
            }
        }
}

void WSbeacon::clearMouse()
{
    DiscretePosition pos(ws->getPosition()->getPos());

    int radius = (int)((RAIO_FAROL - 0.8) * MAZE_DEF);
    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
            if (i * i + j * j < radius * radius) {
                samples[pos.getX() + i][pos.getY() + j] += 1;
                beacon_prob[pos.getX() + i][pos.getY() + j] = 0;
            }
    if (!(beacon_prob[best_point.getX()][best_point.getY()]))
        return;
}

#include "WSmaze.h"

WSmaze::WSmaze(WorldState* w)
{
    ws = w;
    memset(maze, -1, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memset(aux2, -1, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memset(samples, 0, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF * sizeof(int));
    memset(Smallmaze, -1, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memset(Smallaux2, -1, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memset(Smallsamples, 0, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF * sizeof(int));

    init_prob();
    havePerfect = false;

    printf("A calcular tabelas");
    fflush(stdout);
    createRobotMatrix();
    createSensor();
    createGoodMouseMatrix();
    createGoMouseMatrix();
    createKnownMatrix();
    createValidGoMatrix();
    printf("   Feito\n");
    fflush(stdout);
}

void WSmaze::update()
{
    if (havePerfect)
        return;

    // TODO: PENSO QUE ISTO É CONSTANTE. NAO VALE A PENA ACTUALIZAR A CADA CICLO
    MATminXpossible = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF);
    MATmaxXpossible = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF);
    MATminYpossible = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF);
    MATmaxYpossible = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF);
    minXpossible = ws->getPosition()->getMinXpossible();
    maxXpossible = ws->getPosition()->getMaxXpossible();
    minYpossible = ws->getPosition()->getMinYpossible();
    maxYpossible = ws->getPosition()->getMaxYpossible();

#ifdef DO_ERODE_DILATE
    memcpy(mazeErodedAndDilated, maze, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memcpy(maze, aux2, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memcpy(SmallmazeErodedAndDilated, Smallmaze, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memcpy(Smallmaze, Smallaux2, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
#endif

#ifdef DO_SMALL_WS
    decreaseSmallMazeSamples();
#endif

    clearGoodToGoMouseMatrix();
    clearIsKnownMatrix();
    clearCanGoWithMouseMatrix();

    if (ws->getSensors()->inCollision())
        applyWallRobotMatrix(ws->getPosition()->getThisCyclePredictedX(),
            ws->getPosition()->getThisCyclePredictedY());

    applyRobotMatrix(ws->getPosition()->getX(),
        ws->getPosition()->getY());

    applyAllSensors();

#ifdef DO_ERODE_DILATE
    doErodesAndDilates();
#endif

#ifdef DO_SMALL_WS
    //  if (GetReturningLed())
    adjustByMazesDifferences();
#endif
}

void WSmaze::doErodesAndDilates()
{
    // para ficar com a versão antes do processamento
    memcpy(aux2, maze, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    memcpy(Smallaux2, Smallmaze, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);

    // processamento de imagem
    // como isto é lento apenas processar num pequeno alcance do rato
    // de resto utilizar a informação do ciclo anterior...

    memcpy(aux, maze, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);

    erode(aux, maze);
    dilate(maze, aux);

    dilate(aux, maze);
    dilate(maze, aux);

    erode(aux, maze);
    erode(maze, aux);

    memcpy(aux, mazeErodedAndDilated, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + 0.5 + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS - 0.5 + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + 0.5 + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS - 0.5 + LAB_Y) * MAZE_DEF)));

    for (int i = minx; i < maxx; i++)
        for (int j = miny; j < maxy; j++)
            aux[i][j] = maze[i][j];

    memcpy(maze, aux, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);

#ifdef DO_SMALL_WS
    // erode small
    // falta optimizar isto igual ao ws normal...
    erode(Smallaux, Smallmaze);
    dilate(Smallmaze, Smallaux);

    dilate(Smallaux, Smallmaze);
    dilate(Smallmaze, Smallaux);

    erode(Smallaux, Smallmaze);
    erode(Smallmaze, Smallaux);
#endif
}

// TODO: APLICAR OS SENSORES DOS OUTROS RATOS!!!
void WSmaze::applyAllSensors(void)
{
    //
    // se n tiver todos os sensores, n aplica nada
    //
    if (!ws->getSensors()->hasIRSensors())
        return;

    // passagens estreitas, feio :(
    if (ws->getSensors()->getLeftSensor() > 0.5 && ws->getSensors()->getCenterLeftSensor() < 2 || ws->getSensors()->getLeftSensor() < 2 && ws->getSensors()->getCenterLeftSensor() > 0.5) {
        if (ws->getSensors()->getLeftSensor() > ws->getSensors()->getCenterLeftSensor()) {
            applySensor(ws->getSensors()->getLeftSensor(), ws->getSensors()->getLeftDist(), LEFTPOS);
            applyCleanSensor(ws->getSensors()->getCenterLeftSensor(), ws->getSensors()->getCenterLeftDist(), CENTERLEFTPOS);
        } else {
            applyCleanSensor(ws->getSensors()->getLeftSensor(), ws->getSensors()->getLeftDist(), LEFTPOS);
            applySensor(ws->getSensors()->getCenterLeftSensor(), ws->getSensors()->getCenterLeftDist(), CENTERLEFTPOS);
        }
    } else {
        applySensor(ws->getSensors()->getLeftSensor(), ws->getSensors()->getLeftDist(), LEFTPOS);
        applySensor(ws->getSensors()->getCenterLeftSensor(), ws->getSensors()->getCenterLeftDist(), CENTERLEFTPOS);
    }

    if (ws->getSensors()->getCenterRightSensor() > 0.5 && ws->getSensors()->getRightSensor() < 2 || ws->getSensors()->getCenterRightSensor() < 2 && ws->getSensors()->getRightSensor() > 0.5) {
        if (ws->getSensors()->getCenterRightSensor() > ws->getSensors()->getRightSensor()) {
            applySensor(ws->getSensors()->getCenterRightSensor(), ws->getSensors()->getCenterRightDist(), CENTERRIGHTPOS);
            applyCleanSensor(ws->getSensors()->getRightSensor(), ws->getSensors()->getRightDist(), RIGHTPOS);
        } else {
            applyCleanSensor(ws->getSensors()->getCenterRightSensor(), ws->getSensors()->getCenterRightDist(), CENTERRIGHTPOS);
            applySensor(ws->getSensors()->getRightSensor(), ws->getSensors()->getRightDist(), RIGHTPOS);
        }
    } else {
        applySensor(ws->getSensors()->getCenterRightSensor(), ws->getSensors()->getCenterRightDist(), CENTERRIGHTPOS);
        applySensor(ws->getSensors()->getRightSensor(), ws->getSensors()->getRightDist(), RIGHTPOS);
    }

    // se os dois sensores do meio forem muito altos, então pode-se considerar que tem parede entre eles

    if (ws->getSensors()->getCenterLeftSensor() > 50 && ws->getSensors()->getCenterRightSensor() > 50)
        applySensor((ws->getSensors()->getCenterLeftSensor() + ws->getSensors()->getCenterRightSensor()) / 2,
            (ws->getSensors()->getCenterLeftDist() + ws->getSensors()->getCenterRightDist()) / 2,
            0.0);
}

void WSmaze::applySensor(double sensorVal, double sensorDist, double angle)
{
    if (sensorVal > MIN_SENSOR_VALID)
        applySensorMatrix((int)(ws->getPosition()->getDirDeg() + angle),
            sensorDist,
            sensorVal,
            ws->getPosition()->getX() + 0.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)),
            ws->getPosition()->getY() + 0.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)), 0.9);
    else
        applyCleanSensorMatrix((int)(ws->getPosition()->getDirDeg() + angle),
            sensorDist,
            sensorVal,
            ws->getPosition()->getX() + 0.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)),
            ws->getPosition()->getY() + 0.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)), 0.9);
}

void WSmaze::applyCleanSensor(double sensorVal, double sensorDist, double angle)
{
    applyCleanSensorMatrix((int)(ws->getPosition()->getDirDeg() + angle),
        sensorDist,
        sensorVal,
        ws->getPosition()->getX() + 0.5 * cos(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)),
        ws->getPosition()->getY() + 0.5 * sin(ws->getPosition()->getDirRad() + MathTools::deg2rad(angle)), 0.9);
}

bool WSmaze::validToGoWithMouse(int x, int y)
{
    for (int i = 0; i < valid_go_matrix_size; i++)
        if (!inMatrixBounds(x + valid_go_matrix[i][0], y + valid_go_matrix[i][1]))
            return false;

    return true;
}

void WSmaze::createValidGoMatrix(void)
{
    valid_go_matrix_size = 0;
    for (int i = -VALID_GO_MARGIN; i <= VALID_GO_MARGIN; i++)
        for (int j = -VALID_GO_MARGIN; j <= VALID_GO_MARGIN; j++)
            if ((i * i + j * j) <= 25) {
                valid_go_matrix[valid_go_matrix_size][0] = i;
                valid_go_matrix[valid_go_matrix_size++][1] = j;
            }
}

bool WSmaze::isKnown(int x, int y)
{
    if (isKnownMatrix[x][y] < 0) {
        int known = 0;
        int x2, y2;

        for (int i = 0; i < known_matrix_size; i++) {
            x2 = x + known_matrix[i][0];
            y2 = y + known_matrix[i][1];
            if (inMatrixBounds(x2, y2) && samples[x2][y2] > 0)
                known++;
        }
        isKnownMatrix[x][y] = (known >= MIN_TO_BE_KNOWN);
    }
    return isKnownMatrix[x][y];
}

void WSmaze::createKnownMatrix(void)
{
    known_matrix_size = 0;
    for (int i = -KNOWN_MARGIN; i <= KNOWN_MARGIN; i++)
        for (int j = -KNOWN_MARGIN; j <= KNOWN_MARGIN; j++)
            if ((i * i + j * j) <= 25) {
                known_matrix[known_matrix_size][0] = i;
                known_matrix[known_matrix_size++][1] = j;
            }
}

bool WSmaze::CanGoWithMouse(int x, int y)
{
    if (isCanGoWithMouseMatrix[x][y] < 0) {
        int n_parede_forte = 0, n_parede_fraca = 0;
        int x2, y2;

        for (int i = 0; i < go_mouse_matrix_size; i++) {
            x2 = x + go_mouse_matrix[i][0];
            y2 = y + go_mouse_matrix[i][1];
            if (inMatrixBounds(x2, y2))
                if (maze[x2][y2] >= PROB_CONSID_WALL * 2)
                    n_parede_forte++;
                else if (maze[x2][y2] >= PROB_CONSID_WALL)
                    n_parede_fraca++;
        }
        isCanGoWithMouseMatrix[x][y] = (n_parede_forte + n_parede_fraca / 2 < 20);
    }
    return isCanGoWithMouseMatrix[x][y];
}

void WSmaze::createGoMouseMatrix(void)
{
    go_mouse_matrix_size = 0;
    for (int i = -GO_MOUSE_MARGIN; i <= GO_MOUSE_MARGIN; i++)
        for (int j = -GO_MOUSE_MARGIN; j <= GO_MOUSE_MARGIN; j++)
            if ((i * i + j * j) <= 15) {
                go_mouse_matrix[go_mouse_matrix_size][0] = i;
                go_mouse_matrix[go_mouse_matrix_size++][1] = j;
            }
}

double WSmaze::nearWall(int x, int y)
{
    for (int i = -10; i <= 10; i++)
        for (int j = -10; j <= 10; j++)
            if (inMatrixBounds(x + i, y + j) && maze[x + i][y + j] >= PROB_CONSID_WALL)
                return true;
    return false;
}

double WSmaze::goodToGoWithMouse(int x, int y)
{
    int nvisits = 0;
    if (isGoodToGoWithMouseMatrix[x][y] < 0) {
        int i;
        int x2, y2;

        for (i = 0; i < good_mouse_matrix_size; i++) {
            x2 = x + good_mouse_matrix[i][0];
            y2 = y + good_mouse_matrix[i][1];

            if (!inMatrixBounds(x2, y2))
                continue;

            if (maze[x2][y2] > PROB_CONSID_WALL) {
                return (isGoodToGoWithMouseMatrix[x][y] = (double)good_mouse_matrix[i][2]);
            }
            isGoodToGoWithMouseMatrix[x][y] = 0.0;
        }
    }
    return isGoodToGoWithMouseMatrix[x][y];
}

double WSmaze::goodToOptimizeWithMouse(int x, int y)
{
    if (isGoodToGoWithMouseMatrix[x][y] < 0) {
        int i;
        int x2, y2;

        for (i = 0; i < good_mouse_matrix_size; i++) {
            x2 = x + good_mouse_matrix[i][0];
            y2 = y + good_mouse_matrix[i][1];

            if (!inMatrixBounds(x2, y2))
                continue;

            if (maze[x2][y2] > PROB_CONSID_WALL) {
                return (isGoodToGoWithMouseMatrix[x][y] = (double)good_mouse_matrix[i][2]);
            }
        }
        isGoodToGoWithMouseMatrix[x][y] = 0.0;
    }
    return isGoodToGoWithMouseMatrix[x][y];
}

char WSmaze::getMazePos(double x, double y)
{
    if (!inBounds(x, y))
        return -1;
    return maze[(int)((x + LAB_X) * MAZE_DEF)][(int)((y + LAB_Y) * MAZE_DEF)];
}

char WSmaze::getSmallMazePos(double x, double y)
{
    if (!inBounds(x, y))
        return -1;
    return Smallmaze[(int)((x + LAB_X) * MAZE_DEF)][(int)((y + LAB_Y) * MAZE_DEF)];
}

void WSmaze::setMazePos(double x, double y, char val, double certeza)
{
    if (!inBounds(x, y))
        return;

    int xmazepos = (int)round(((x + LAB_X) * MAZE_DEF));
    int ymazepos = (int)round(((y + LAB_Y) * MAZE_DEF));

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    maze[xmazepos][ymazepos] = (maze[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    samples[xmazepos][ymazepos] += peso;
}

void WSmaze::setMazeMatPos(int xmazepos, int ymazepos, char val, double certeza)
{
    if (!inMatrixBounds(xmazepos, ymazepos))
        return;

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    maze[xmazepos][ymazepos] = (maze[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    samples[xmazepos][ymazepos] += peso;
}

void WSmaze::setMazeMatPosAfterProcess(int xmazepos, int ymazepos, char val, double certeza)
{
    if (!inMatrixBounds(xmazepos, ymazepos))
        return;

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    aux2[xmazepos][ymazepos] = (aux2[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    samples[xmazepos][ymazepos] += peso;
}

void WSmaze::createRobotMatrix(void)
{
    int i, j;

    for (i = 0; i < MAZE_DEF; i++)
        for (j = 0; j < MAZE_DEF; j++)
            if (sqrt((double)((i - MAZE_DEF / 2) * (i - MAZE_DEF / 2) + (j - MAZE_DEF / 2) * (j - MAZE_DEF / 2))) > ((double)MAZE_DEF / 2))
                robotMatrix[i][j] = -1;
            else
                robotMatrix[i][j] = 0;
}

void WSmaze::createSensor(void)
{
    int alpha, beta, ang;
    int i, j;
    double dist, dist_max_parede, sensor_val_max, cur_sensorval, sensordif, sensor_max_dif;

    // limitado pela definição da matriz
    for (int dir = 0; dir < 360; dir++) {
        double sensor_val;
        alpha = dir - 30;
        beta = dir + 30;

        for (int sensordist = 0; sensordist <= MAZE_DEF; sensordist++) {
            if (sensordist == 0)
                sensor_val = 100;
            else
                sensor_val = 1 / (((double)sensordist) / MAZE_DEF);
            if (sensor_val > 100)
                sensor_val = 100;

            for (i = 0; i < 3 * MAZE_DEF; i++)
                for (j = 0; j < 3 * MAZE_DEF; j++) {
                    ang = (int)MathTools::calc_angle(3 * MAZE_DEF / 2, 3 * MAZE_DEF / 2, i, j);
                    dist = sqrt((double)(i - 3 * MAZE_DEF / 2) * (i - 3 * MAZE_DEF / 2) + (j - 3 * MAZE_DEF / 2) * (j - 3 * MAZE_DEF / 2));

                    if (MathTools::angle_between(ang, alpha, beta) && dist <= sensordist + 0.5 * MAZE_DEF) {
                        dist_max_parede = dist - 0.5 * MAZE_DEF;

                        if (dist_max_parede <= 0)
                            sensor_val_max = 100;
                        else
                            sensor_val_max = ((double)1) / (dist_max_parede / MAZE_DEF);
                        if (sensor_val_max > 100)
                            sensor_val_max = 100;

                        if (dist == 0)
                            cur_sensorval = 100;
                        else
                            cur_sensorval = 1 / (dist / MAZE_DEF);
                        if (cur_sensorval > 100)
                            cur_sensorval = 100;

                        sensordif = sensor_val - cur_sensorval;
                        sensor_max_dif = sensor_val - sensor_val_max - 1;

                        sensor[dir][sensordist][i][j] = (char)(distri_normal_acc(sensordif, sensor_max_dif) * 100);
                    } else
                        sensor[dir][sensordist][i][j] = -1;
                }
        }
    }

    // limitado pela definição do sensor

    for (int dir = 0; dir < 360; dir++) {
        double sensordist;
        alpha = dir - 30;
        beta = dir + 30;

        for (int sensor_val = 0; sensor_val < 10; sensor_val++) {
            if (sensor_val == 0)
                sensordist = 20;
            else
                sensordist = ((double)1) / (((double)sensor_val) / 10);

            sensordist *= MAZE_DEF;

            if (sensor_val < 3)
                sensor_long[dir][sensor_val].size = (int)100;
            else
                sensor_long[dir][sensor_val].size = (int)sensordist * 2 + 10;

            for (i = 0; i < sensor_long[dir][sensor_val].size; i++)
                for (j = 0; j < sensor_long[dir][sensor_val].size; j++) {
                    ang = (int)MathTools::calc_angle(sensor_long[dir][sensor_val].size / 2, sensor_long[dir][sensor_val].size / 2, i, j);
                    dist = sqrt((double)(i - sensor_long[dir][sensor_val].size / 2) * (i - sensor_long[dir][sensor_val].size / 2) + (j - sensor_long[dir][sensor_val].size / 2) * (j - sensor_long[dir][sensor_val].size / 2));

                    if (MathTools::angle_between(ang, alpha, beta) && dist <= sensordist + 0.5 * MAZE_DEF) {
                        dist_max_parede = dist - 0.5 * MAZE_DEF;

                        if (dist_max_parede <= 0)
                            sensor_val_max = 100;
                        else
                            sensor_val_max = ((double)1) / (dist_max_parede / MAZE_DEF);
                        if (sensor_val_max > 100)
                            sensor_val_max = 100;

                        if (dist == 0)
                            cur_sensorval = 100;
                        else
                            cur_sensorval = 1 / (dist / MAZE_DEF);
                        if (cur_sensorval > 100)
                            cur_sensorval = 100;

                        sensordif = ((double)sensor_val) / 10 - cur_sensorval;
                        sensor_max_dif = ((double)sensor_val) / 10 - sensor_val_max - 1;

                        sensor_long[dir][sensor_val].mat[i][j] = (char)(distri_normal_acc(sensordif, sensor_max_dif) * 100);
                    } else
                        sensor_long[dir][sensor_val].mat[i][j] = -1;
                }
        }
    }
}

void WSmaze::createGoodMouseMatrix(void)
{
    int raio;
    int i, j;

    good_mouse_matrix_size = 0;
    for (i = -GOOD_MOUSE_MARGIN; i <= GOOD_MOUSE_MARGIN; i++)
        for (j = -GOOD_MOUSE_MARGIN; j <= GOOD_MOUSE_MARGIN; j++) {
            good_mouse_matrix[good_mouse_matrix_size][0] = i;
            good_mouse_matrix[good_mouse_matrix_size][1] = j;

            raio = (i * i + j * j);
            if (raio <= 5)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 100;
            else if (raio <= 10)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 30;
            else if (raio <= 20)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 25;
            else if (raio <= 25)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 8;
            else if (raio <= 30)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 8;
            else if (raio <= 33)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 7;
            else if (raio <= 35)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 6;
            else if (raio <= 36)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 5;
            else if (raio <= 37)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 4;
            else if (raio <= 40)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 2;
            else if (raio <= 60)
                good_mouse_matrix[good_mouse_matrix_size++][2] = 1;
        }

    qsort(good_mouse_matrix, good_mouse_matrix_size, sizeof(int) * 3, good_mouse_matrix_cmp);
}

#define p1 ((const int*)a)
#define p2 ((const int*)b)
int good_mouse_matrix_cmp(const void* a, const void* b)
{
    return (p1[0] * p1[0] + p1[1] * p1[1]) - (p2[0] * p2[0] + p2[1] * p2[1]);
}
#undef p1
#undef p2

void WSmaze::applyRobotMatrix(double X, double Y)
{
    int i, j;

    int xpos = (int)round((X + LAB_X) * MAZE_DEF - MAZE_DEF / 2);
    int ypos = (int)round((Y + LAB_Y) * MAZE_DEF - MAZE_DEF / 2);

    for (i = 0; i < MAZE_DEF; i++)
        for (j = 0; j < MAZE_DEF; j++)
            if (robotMatrix[i][j] >= 0) {
                setMazeMatPos(xpos + i, ypos + j, robotMatrix[i][j], 1.0);
#ifdef DO_SMALL_WS
                setSmallMazeMatPos(xpos + i, ypos + j, robotMatrix[i][j], 1.0);
#endif
            }
}

void WSmaze::applyWallRobotMatrix(double X, double Y)
{
    int i, j;

    int xpos = (int)round((X + LAB_X) * MAZE_DEF);
    int ypos = (int)round((Y + LAB_Y) * MAZE_DEF);

    for (i = -10; i < 10; i++)
        for (j = -10; j < 10; j++)
            if ((i * i + j * j) <= 30) {
                setMazeMatPos(xpos + i, ypos + j, 100, 1.0);
#ifdef DO_SMALL_WS
                setSmallMazeMatPos(xpos + i, ypos + j, 100, 1.0);
#endif
            }
}

void WSmaze::applyWallRobotMatrixAfterProcess(double X, double Y)
{
    int i, j;

    int xpos = (int)round((X + LAB_X) * MAZE_DEF);
    int ypos = (int)round((Y + LAB_Y) * MAZE_DEF);

    for (i = -10; i < 10; i++)
        for (j = -10; j < 10; j++)
            if ((i * i + j * j) <= 30) {
                setMazeMatPosAfterProcess(xpos + i, ypos + j, 100, 1.0);
#ifdef DO_SMALL_WS
                setSmallMazeMatPosAfterProcess(xpos + i, ypos + j, 100, 1.0);
#endif
            }
}

void WSmaze::applyCleanSensorMatrix(int ang, double sensordist, double sensor_val, double X, double Y, double conf)
{
    int i, j;

    ang = (int)round(MathTools::normalize_0_to_360(ang));

    if (sensor_val >= 1) {
        int sdist = (int)round(sensordist * 10);

        int xpos = (int)round((X + LAB_X) * MAZE_DEF - 3 * MAZE_DEF / 2);
        int ypos = (int)round((Y + LAB_Y) * MAZE_DEF - 3 * MAZE_DEF / 2);

        for (i = 0; i < 3 * MAZE_DEF; i++)
            for (j = 0; j < 3 * MAZE_DEF; j++)
                if (sensor[ang][sdist][i][j] >= 0 && sensor[ang][sdist][i][j] < 40) {
                    setMazeMatPos(xpos + i, ypos + j, sensor[ang][sdist][i][j], conf);
#ifdef DO_SMALL_WS
                    setSmallMazeMatPos(xpos + i, ypos + j, sensor[ang][sdist][i][j], conf);
#endif
                }
    } else {
        int sval = (int)round(sensor_val * 10);

        int xpos = (int)round((X + LAB_X) * MAZE_DEF - sensor_long[ang][sval].size / 2);
        int ypos = (int)round((Y + LAB_Y) * MAZE_DEF - sensor_long[ang][sval].size / 2);

        for (i = 0; i < sensor_long[ang][sval].size; i++)
            for (j = 0; j < sensor_long[ang][sval].size; j++)
                if (sensor_long[ang][sval].mat[i][j] >= 0 && sensor_long[ang][sval].mat[i][j] < 2) {
                    setMazeMatPos(xpos + i, ypos + j, (char)(sensor_long[ang][sval].mat[i][j]), conf);
#ifdef DO_SMALL_WS
                    setSmallMazeMatPos(xpos + i, ypos + j, (char)(sensor_long[ang][sval].mat[i][j]), conf);
#endif
                }
    }
}

void WSmaze::applySensorMatrix(int ang, double sensordist, double sensor_val, double X, double Y, double conf)
{
    int i, j;

    ang = (int)round(MathTools::normalize_0_to_360(ang));

    if (sensor_val >= 1) {
        int sdist = (int)round(sensordist * 10);

        int xpos = (int)round((X + LAB_X) * MAZE_DEF - 3 * MAZE_DEF / 2);
        int ypos = (int)round((Y + LAB_Y) * MAZE_DEF - 3 * MAZE_DEF / 2);

        for (i = 0; i < 3 * MAZE_DEF; i++)
            for (j = 0; j < 3 * MAZE_DEF; j++)
                if (sensor[ang][sdist][i][j] >= 0) {
                    setMazeMatPos(xpos + i, ypos + j, sensor[ang][sdist][i][j], conf);
#ifdef DO_SMALL_WS
                    setSmallMazeMatPos(xpos + i, ypos + j, sensor[ang][sdist][i][j], conf);
#endif
                }
    } else {
        int sval = (int)round(sensor_val * 10);

        int xpos = (int)round((X + LAB_X) * MAZE_DEF - sensor_long[ang][sval].size / 2);
        int ypos = (int)round((Y + LAB_Y) * MAZE_DEF - sensor_long[ang][sval].size / 2);

        for (i = 0; i < sensor_long[ang][sval].size; i++)
            for (j = 0; j < sensor_long[ang][sval].size; j++)
                if (sensor_long[ang][sval].mat[i][j] >= 0) {
                    setMazeMatPos(xpos + i, ypos + j, (char)(sensor_long[ang][sval].mat[i][j]), conf);
#ifdef DO_SMALL_WS
                    setSmallMazeMatPos(xpos + i, ypos + j, (char)(sensor_long[ang][sval].mat[i][j]), conf);
#endif
                }
    }
}

/* funções de probabilidade */

void WSmaze::init_prob()
{
    int val;
    double x, aux;

    prob_acc[0] = 0;

    for (val = 1; val < 200; val++) {
        x = (double)(val - 100);
        aux = exp(-(x * x) / (2 * DESVIO_SENSOR * DESVIO_SENSOR * 10000));
        prob_acc[val] = prob_acc[val - 1] + (aux / (DESVIO_SENSOR * 100 * sqrt((double)2 * PI)));
    }
}

double WSmaze::distri_normal_acc(double dif, double dif_max)
{
    double p1, p2;

    if (dif < -1)
        p1 = 0;
    else if (dif >= 1)
        p1 = 1;
    else
        p1 = prob_acc[(int)(dif * 100) + 100];

    if (dif_max < -1)
        p2 = 0;
    else if (dif_max >= 1)
        p2 = 1;
    else
        p2 = prob_acc[(int)(dif_max * 100) + 100];

    return p1 - p2;
}

void WSmaze::clearGoodToGoMouseMatrix()
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS + LAB_Y) * MAZE_DEF)));

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++)
            isGoodToGoWithMouseMatrix[i][j] = -1;
}

void WSmaze::clearIsKnownMatrix()
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS + LAB_Y) * MAZE_DEF)));

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++)
            isKnownMatrix[i][j] = -1;
}

void WSmaze::clearCanGoWithMouseMatrix()
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS + LAB_Y) * MAZE_DEF)));

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++)
            isCanGoWithMouseMatrix[i][j] = -1;
}

// operações morfologicas

void WSmaze::erode(char newmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y], char oldmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y])
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS + LAB_Y) * MAZE_DEF)));

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++) {
            if (oldmatrix[i][j] < 0)
                newmatrix[i][j] = -1;
            else if ((oldmatrix[i - 1][j - 1] > PROB_CONSID_WALL || oldmatrix[i - 1][j - 1] < 0) && (oldmatrix[i - 1][j] > PROB_CONSID_WALL || oldmatrix[i - 1][j] < 0) && (oldmatrix[i - 1][j + 1] > PROB_CONSID_WALL || oldmatrix[i - 1][j + 1] < 0) && (oldmatrix[i][j - 1] > PROB_CONSID_WALL || oldmatrix[i][j - 1] < 0) && (oldmatrix[i][j] > PROB_CONSID_WALL || oldmatrix[i][j] < 0) && (oldmatrix[i][j + 1] > PROB_CONSID_WALL || oldmatrix[i][j + 1] < 0) && (oldmatrix[i + 1][j - 1] > PROB_CONSID_WALL || oldmatrix[i + 1][j - 1] < 0) && (oldmatrix[i + 1][j] > PROB_CONSID_WALL || oldmatrix[i + 1][j] < 0) && (oldmatrix[i + 1][j + 1] > PROB_CONSID_WALL || oldmatrix[i + 1][j + 1] < 0))
                newmatrix[i][j] = 100;
            else
                newmatrix[i][j] = 0;
        }
}

void WSmaze::erodeMazeToUnknown()
{
    memcpy(maze, aux2, LAB_X * 2 * MAZE_DEF * LAB_Y * 2 * MAZE_DEF);
    erodeToUnknown(aux2, maze);
}

void WSmaze::erodeToUnknown(char newmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y], char oldmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y])
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++) {
            if (oldmatrix[i][j] < 0)
                newmatrix[i][j] = -1;
            else if ((oldmatrix[i - 1][j - 1] > PROB_CONSID_WALL) && (oldmatrix[i - 1][j] > PROB_CONSID_WALL) && (oldmatrix[i - 1][j + 1] > PROB_CONSID_WALL) && (oldmatrix[i][j - 1] > PROB_CONSID_WALL) && (oldmatrix[i][j] > PROB_CONSID_WALL) && (oldmatrix[i][j + 1] > PROB_CONSID_WALL) && (oldmatrix[i + 1][j - 1] > PROB_CONSID_WALL) && (oldmatrix[i + 1][j] > PROB_CONSID_WALL) && (oldmatrix[i + 1][j + 1] > PROB_CONSID_WALL))
                newmatrix[i][j] = oldmatrix[i][j];
            else if (oldmatrix[i][j] > PROB_CONSID_WALL)
                newmatrix[i][j] = -1;
            else
                newmatrix[i][j] = oldmatrix[i][j];
        }
}

void WSmaze::dilate(char newmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y], char oldmatrix[2 * MAZE_DEF * LAB_X][2 * MAZE_DEF * LAB_Y])
{
    int i, j;

    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF) + 1;
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF) - 1;
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF) + 1;
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF) - 1;

    minx = (int)round(MathTools::max(minx, ((ws->getPosition()->getX() - ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    maxx = (int)round(MathTools::min(maxx, ((ws->getPosition()->getX() + ACTION_RADIUS + LAB_X) * MAZE_DEF)));
    miny = (int)round(MathTools::max(miny, ((ws->getPosition()->getY() - ACTION_RADIUS + LAB_Y) * MAZE_DEF)));
    maxy = (int)round(MathTools::min(maxy, ((ws->getPosition()->getY() + ACTION_RADIUS + LAB_Y) * MAZE_DEF)));

    for (i = minx; i < maxx; i++)
        for (j = miny; j < maxy; j++) {
            if (oldmatrix[i][j] < 0)
                newmatrix[i][j] = -1;
            else if (oldmatrix[i - 1][j - 1] > PROB_CONSID_WALL || oldmatrix[i - 1][j] > PROB_CONSID_WALL || oldmatrix[i - 1][j + 1] > PROB_CONSID_WALL || oldmatrix[i][j - 1] > PROB_CONSID_WALL || oldmatrix[i][j] > PROB_CONSID_WALL || oldmatrix[i][j + 1] > PROB_CONSID_WALL || oldmatrix[i + 1][j - 1] > PROB_CONSID_WALL || oldmatrix[i + 1][j] > PROB_CONSID_WALL || oldmatrix[i + 1][j + 1] > PROB_CONSID_WALL)
                newmatrix[i][j] = 100;
            else
                newmatrix[i][j] = 0;
        }
}

/********************************************************/
/* SMALL WORLD STATE                                    */
/********************************************************/

void WSmaze::setSmallMazePos(double x, double y, char val, double certeza)
{
#ifdef DO_SMALL_WS
    if (!inBounds(x, y))
        return;

    int xmazepos = (int)round(((x + LAB_X) * MAZE_DEF));
    int ymazepos = (int)round(((y + LAB_Y) * MAZE_DEF));

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    Smallmaze[xmazepos][ymazepos] = (Smallmaze[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    Smallsamples[xmazepos][ymazepos] += peso;
#endif
}

void WSmaze::setSmallMazeMatPos(int xmazepos, int ymazepos, char val, double certeza)
{
#ifdef DO_SMALL_WS
    if (!inSmallMazeMatrixBounds(xmazepos, ymazepos))
        return;

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    Smallmaze[xmazepos][ymazepos] = (Smallmaze[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    Smallsamples[xmazepos][ymazepos] += peso;
#endif
}

void WSmaze::setSmallMazeMatPosAfterProcess(int xmazepos, int ymazepos, char val, double certeza)
{
#ifdef DO_SMALL_WS
    if (!inSmallMazeMatrixBounds(xmazepos, ymazepos))
        return;

    int peso;

    if (certeza > 0.95)
        peso = 50;
    else if (certeza > 0.6)
        if (val <= 8 && val >= 0)
            peso = 5;
        else
            peso = 2;
    else if (val == 0)
        peso = 5;
    else if (val > 0 && val < 2)
        peso = 2;
    else
        peso = 1;

    Smallmazeaux2[xmazepos][ymazepos] = (Smallmazeaux2[xmazepos][ymazepos] * samples[xmazepos][ymazepos] + val * peso) / (samples[xmazepos][ymazepos] + peso);
    Smallsamplesaux2[xmazepos][ymazepos] += peso;
#endif
}

bool WSmaze::inSmallMazeMatrixBounds(int x, int y)
{
    return inMatrixBounds(x, y);
}

bool WSmaze::inSmallMazeBounds(double x, double y)
{
    return inBounds(x, y);
}

double WSmaze::compareWithSmallMaze(char s[2 * LAB_X * MAZE_DEF][2 * LAB_Y * MAZE_DEF],
    int ssamples[2 * LAB_X * MAZE_DEF][2 * LAB_Y * MAZE_DEF],
    int dx, int dy)
{
    double total = 0.0;
    int npontos = 0;
    int i, j;

    for (i = -(SMALL_LAB_X - 1) * MAZE_DEF; i < (SMALL_LAB_X - 1) * MAZE_DEF; i++)
        for (j = -(SMALL_LAB_Y - 1) * MAZE_DEF; j < (SMALL_LAB_Y - 1) * MAZE_DEF; j++) {
            int labi, labj;
            int slabi, slabj;
            labi = (int)((ws->getPosition()->getX() + LAB_X) * MAZE_DEF) + i;
            labj = (int)((ws->getPosition()->getY() + LAB_Y) * MAZE_DEF) + j;
            slabi = labi - dx;
            slabj = labj - dy;

            if (ssamples[slabi][slabj] < 5 || samples[labi][labj] < 70)
                continue;

            if (s[slabi][slabj] < 0 || maze[labi][labj] < 0)
                continue;
            else if ((s[slabi][slabj] < PROB_CONSID_WALL && maze[labi][labj] < PROB_CONSID_WALL))
                total++;
            else if (s[slabi][slabj] >= PROB_CONSID_WALL && maze[labi][labj] >= PROB_CONSID_WALL)
                total++;
            else
                total -= 2;
            npontos++;
        }

    //printf("pontos analisados: %d\n",npontos);
    return total / npontos;
}

void WSmaze::decreaseSmallMazeSamples()
{
    int minx = (int)((ws->getPosition()->getMinXpossible() + LAB_X) * MAZE_DEF);
    int maxx = (int)((ws->getPosition()->getMaxXpossible() + LAB_X) * MAZE_DEF);
    int miny = (int)((ws->getPosition()->getMinYpossible() + LAB_Y) * MAZE_DEF);
    int maxy = (int)((ws->getPosition()->getMaxYpossible() + LAB_Y) * MAZE_DEF);

    int myposx = (int)round((ws->getPosition()->getX() + LAB_X) * MAZE_DEF);
    int myposy = (int)round((ws->getPosition()->getY() + LAB_Y) * MAZE_DEF);

    for (int i = minx; i < maxx; i++)
        for (int j = miny; j < maxy; j++) {
            if ((i < (myposx - SMALL_LAB_X * MAZE_DEF) || i > (myposx + SMALL_LAB_X * MAZE_DEF)) || (j < (myposy - SMALL_LAB_Y * MAZE_DEF) || j > (myposy + SMALL_LAB_Y * MAZE_DEF))) {
                Smallsamples[i][j] = 0;
                Smallmaze[i][j] = -1;
            }
        }
}

void WSmaze::adjustByMazesDifferences()
{
    double esq, dir, cima, baixo, val;
    double movx = 0.0, movy = 0.0;

    val = compareWithSmallMaze(Smallmaze, Smallsamples, 0, 0);

    esq = compareWithSmallMaze(Smallmaze, Smallsamples, -1, 0);
    dir = compareWithSmallMaze(Smallmaze, Smallsamples, 1, 0);
    cima = compareWithSmallMaze(Smallmaze, Smallsamples, 0, 1);
    baixo = compareWithSmallMaze(Smallmaze, Smallsamples, 0, -1);

    /*printf("TIME: %d val: %lf -- esq: %lf, dir: %lf, cima: %lf, baixo: %lf\n",
	  ws->getTime(),val,esq,dir,cima,baixo);*/

    if (esq < val && dir - val > POS_MOVE_MARGIN) {
        movx = -0.1 / 4;
    } else if (dir < val && esq - val > POS_MOVE_MARGIN) {
        movx = 0.1 / 4;
    }

    if (cima < val && baixo - val > POS_MOVE_MARGIN) {
        movy = 0.1 / 4;
    } else if (baixo < val && cima - val > POS_MOVE_MARGIN) {
        movy = -0.1 / 4;
    }

    if (fabs(movx) > 0.001 || fabs(movy) > 0.001) {
        ws->getPosition()->adjustByMaze(movx, movy);
        printf("Time: %d x:%lf y:%lf\n", ws->getTime(), movx, movy);
    }
}

void WSmaze::PaintMouseStoppedAhead()
{
    puts("A pintar rato parado");
    applyWallRobotMatrixAfterProcess(ws->getPosition()->getX() + 1 * cos(ws->getPosition()->getDirRad()),
        ws->getPosition()->getY() + 1 * sin(ws->getPosition()->getDirRad()));
    return;
}

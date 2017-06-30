#ifndef _STRUCT_H_
#define _STRUCT_H_

#define MAX_HEAP_SIZE 2048

#include "Path.h"

class Heap;

class Heap {
public:
    Heap();
    ~Heap();

    bool isEmpty(void);
    void makeEmpty(void);

    void insert(Path* p)
    {
        array[0] = p;

        if (theSize == MAX_HEAP_SIZE) {
            printf("A heap está cheia! PANICO!!!\n"); // felizmente nunca deu este erro ;)
            fflush(stdout);
            return;
        }

        int hole;

        if (p->getpqPlace() < 0)
            hole = ++theSize;
        else
            hole = p->getpqPlace();

        for (; hole > 0 && *p < *array[hole / 2]; hole /= 2) {
            array[hole] = array[hole / 2];
            array[hole]->setpqPlace(hole);
        }
        array[hole] = p;
        p->setpqPlace(hole);
    }

    Path* deleteMin(void)
    {
        if (isEmpty())
            return NULL;

        Path* toReturn = array[1];
        array[1] = array[theSize--];
        array[1]->setpqPlace(1);
        percolateDown(1);
        return toReturn;
    }

    void percolateDown(int hole)
    {
        int child;

        Path* tmp = array[hole];

        for (; hole * 2 <= theSize; hole = child) {
            child = hole * 2;
            if (child != theSize && *array[child + 1] < *array[child])
                child++;
            if (*array[child] < *tmp) {
                array[hole] = array[child];
                array[hole]->setpqPlace(hole);
            } else
                break;
        }
        array[hole] = tmp;
        array[hole]->setpqPlace(hole);
    }

private:
    int theSize;
    Path* array[MAX_HEAP_SIZE];
};

#endif

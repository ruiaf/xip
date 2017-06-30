#include "Structures.h"

Heap::Heap()
{
  theSize=0;
}

Heap::~Heap()
{
  makeEmpty();
}

void Heap::makeEmpty(void)
{
  theSize=0;
}

bool Heap::isEmpty(void)
{
  return theSize<1;
}


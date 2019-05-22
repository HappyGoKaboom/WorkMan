//
// Created by unknown on 04/01/19.
//

#ifndef TEST_H
#define TEST_H

#include "data.h"
#include "Memory.tcc"
#include "Worker.h"
#include "WorkMan.h"
#include "WorkManTypes.h"

class Worker;

typedef RWQueue<Data> Lock;

void setup();

void processA(Data* read);
void processB(Data* read, Data* write);
void processC(Data* write);


#endif //TEST

//
// Created by unknown on 17/12/18.
//

#ifndef WORK_MAN_H
#define WORK_MAN_H

#include <vector>
#include "Worker.h"
#include "WorkManTypes.h"

class Worker;

/* =======================================================================================================/
 *  WorkMan Class - Workload Manager
 *      This deals with each workload, starting, stopping, running and creating
 * ====================================================================================================*/
class WorkMan {
public:
    std::vector<Worker> workers; // array of workers
    unsigned int numWorkers = 0;
    unsigned int threadPool = 0; // how many available threads do we have
    unsigned int threadCount = 0; // current thread count

    Worker& create(Worker&& config);
    void modify(int worker, Worker& config);
    void get(int worker);
    void remove(int worker = -1, bool wait = true);
    void start(int worker = -1, bool wait = true);
    void stop(int worker = -1, bool wait = true);
    void resume(int worker = -1, bool wait = true);
    void terminate(int worker = -1);
};




#endif //WORK_MAN_H

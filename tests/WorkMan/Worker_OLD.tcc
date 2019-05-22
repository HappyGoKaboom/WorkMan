//
// Created by unknown on 04/01/19.
//

#ifndef WORKER_H
#define WORKER_H

#include <vector>
#include <thread>
#include <functional>
#include <chrono>

enum DISTRIBUTION {
    STATIC_MODEL,
    DYNAMIC_MODEL
};

enum LOCKCONFIG {
    READ,
    WRITE,
    READWRITE
};

template<typename Lock, typename LockData>
class Worker {
public:
    Worker(
            DISTRIBUTION model,
            unsigned int batch,
            unsigned int maxThreads,
            bool autoLocking,
            std::vector<std::vector<unsigned int>> threads,
            std::vector<unsigned long> process,
            std::vector<void *> processes,
            std::vector<LOCKCONFIG> lockConfig)
            :
            model(model),
            batch(batch),
            autoLocking(autoLocking),
            maxThreads(maxThreads),
            threads(std::move(threads)),
            process(std::move(process)),
            processes(std::move(processes)),
            lockConfig(std::move(lockConfig)) {
        std::cout << "CONST$RUCT" << std::endl;
        update();
    };

    // copy constructor
    Worker(const Worker &other) = delete;

    // copy assignment
    Worker &operator=(const Worker &other) = delete;

    // move constructor
    Worker(Worker &&other) noexcept {
        threads = std::move(other.threads);
        threadPool = std::move(other.threadPool);
        process = std::move(other.process);
        processes = std::move(other.processes);
        cyclers = std::move(other.cyclers);
        lockConfig = std::move(other.lockConfig);
        batch = std::move(other.batch);
        maxThreads = std::move(other.maxThreads);
        index = std::move(other.index);
        model = std::move(other.model);
        autoLocking = std::move(other.autoLocking);
        workThreads = std::move(other.workThreads);
        assignedThreads = std::move(other.assignedThreads);
        threadProcessLookup = std::move(other.threadProcessLookup);

        // must update all children worker reference to this
        for (unsigned int n = 0; n < this->workThreads.size(); ++n) {
            this->workThreads[n].worker = this;
        }
    }

    // move assignment
    Worker &operator=(Worker &&other) noexcept {
        threads = std::move(other.threads);
        threadPool = std::move(other.threadPool);
        process = std::move(other.process);
        processes = std::move(other.processes);
        cyclers = std::move(other.cyclers);
        lockConfig = std::move(other.lockConfig);
        batch = std::move(other.batch);
        maxThreads = std::move(other.maxThreads);
        index = std::move(other.index);
        model = std::move(other.model);
        autoLocking = std::move(other.autoLocking);
        workThreads = std::move(other.workThreads);
        assignedThreads = std::move(other.assignedThreads);
        threadProcessLookup = std::move(other->threadProcessLookup);

        // must update all children worker reference to this
        for (unsigned int n = 0; n < this->workThreads.size(); ++n) {
            this->workThreads[n].worker = this;
        }

        return *this;
    }

    // move pointer constructor
    explicit Worker(Worker *other) {
        threads = std::move(other->threads);
        threadPool = std::move(other->threadPool);
        process = std::move(other->process);
        processes = std::move(other->processes);
        cyclers = std::move(other->cyclers);
        lockConfig = std::move(other->lockConfig);
        batch = std::move(other->batch);
        maxThreads = std::move(other->maxThreads);
        index = std::move(other->index);
        model = std::move(other->model);
        autoLocking = std::move(other->autoLocking);
        workThreads = std::move(other->workThreads);
        assignedThreads = std::move(other->assignedThreads);
        threadProcessLookup = std::move(other->threadProcessLookup);

        // must update all children worker reference to this
        for (unsigned int n = 0; n < this->workThreads.size(); ++n) {
            this->workThreads[n].worker = this;
        }
    }

    struct WorkProcess {
        WorkProcess() {};

        WorkProcess(WorkProcess &&other) noexcept {
            work = std::move(other.work);
            index = std::move(other.index);
            completed = std::move(other.completed);
            total = std::move(other.total);
        };

        WorkProcess &operator=(WorkProcess &&other) noexcept {
            work = std::move(other.work);
            index = std::move(other.index);
            completed = std::move(other.completed);
            total = std::move(other.total);
        };

        WorkProcess(const WorkProcess &other) noexcept = delete;

        WorkProcess &operator=(const WorkProcess &other) = delete;

        std::vector<Lock *> work;
        unsigned int index = 0; // last item of work
        unsigned int completed = 0;
        unsigned int total = 0;
    };

    struct WorkThread {
        WorkThread(Worker<Lock, Data> *parent) : worker(parent) {
        };

        WorkThread(WorkThread &&other) noexcept {
            id = std::move(other.id);
            processes = std::move(other.processes);
            process = std::move(other.process);
            currentItem = std::move(other.currentItem);
            worker = std::move(other.worker);
            hasWork = std::move(other.hasWork);
        };

        WorkThread &operator=(WorkThread &&other) noexcept {
            id = std::move(other.id);
            processes = std::move(other.processes);
            process = std::move(other.process);
            currentItem = std::move(other.currentItem);
            worker = std::move(other.worker);
            hasWork = std::move(other.hasWork);
        };

        WorkThread(const WorkThread &other) noexcept = delete;

        WorkThread &operator=(const WorkThread &other) = delete;

        std::vector<WorkProcess> processes; // process and item list for that process
        std::vector<int> assigned; // assigned processes
        unsigned int id = 0; // current process id ?
        unsigned int process = 0; // current process id ?
        unsigned int currentItem = 0;
        Worker<Lock, Data> *worker;

        Lock *next(); // next workload item
        void done(); // completed workload
        bool hasWork = false;
    };

    // Worker Functions
    void start(); // start the threads
    void pause(); // pauses the threads
    void stop(); // stop the threads
    void terminate(); // terminate the threads
    void workhandlerLocking(WorkThread &thread);

    void workhandler(WorkThread &thread);

    void add(Lock *item, unsigned int process = 0);

    void update();

    // Thread Functions
    void create(unsigned int threadID);

    void detach();

    void join();

    void swap();

    void getPriority();

    void setPriority();

    /*
     *      model(model),
            batch(batch),
            autoLocking(autoLocking),
            maxThreads(maxThreads),
            threads(std::move(threads)),
            process(std::move(process)),
            processes(std::move(processes)),
            lockConfig(std::move(lockConfig))
     */

    /* Thread Configuration
     *      threads [Thread] [Processes] = Process ID
     *
     *      {PROCESS::A, PROCESS::B},   // T0
     *      {PROCESS::A, PROCESS::B},   // T1
     *      {PROCESS::C},               // T2
     *      {PROCESS::C}                // T3
     *
     */
    std::vector<std::vector<unsigned int>> threads;

    /*  WorkThreads
     *      workTreads [ Assigned Thread ] = WorkThread
     */
    std::vector<WorkThread> workThreads;

    /*  threadPool
     *      threadPool [Available Threads] = thread ID (ie std::thread)
     */
    std::vector<std::thread> threadPool;

    /*  process
     *      process [ order ] = PROCESS
     *
     *      PROCESS::A, PROCESS::B, PROCESS::C
     */
    std::vector<unsigned long> process; // process order (functions)

    /*  processes
     *      processes [Process] = function pointer
     */
    std::vector<void *> processes; // the process functions themselves

    /*  cyclers
     *      cyclers [Thread ID] = cycler
     */
    std::vector<unsigned short int> cyclers; // list of each threads assigned workload cyclers

    /*  assignedThreads
     *      assignedThreads [ProcessID] [Cycler] = Assigned Thread.
     */
    std::vector<std::vector<unsigned short int>> assignedThreads; // list of threads assigned to each process

    /*  threadProcessLookup
     *      threadProcessLookup [Thread ID] [Process] = WorkerThread Process the wirk is assigned to
     */
    std::vector<std::vector<unsigned short int>> threadProcessLookup; // by the process on thread what is the index within the thread.processes

    std::vector<LOCKCONFIG> lockConfig; // lock config
    int batch = 1;
    unsigned int maxThreads = 0;
    unsigned int index = 0;
    DISTRIBUTION model;
    bool autoLocking = false;
};

/* -------------------------------------------------------------------------------------------------------/
 *      Struct methods for the work items
 * ----------------------------------------------------------------------------------------------------*/


/* -------------------------------------------------------------------------------------------------------/
 *      Worker Functions
 * ----------------------------------------------------------------------------------------------------*/
template<typename Lock, typename LockData>
void Worker<Lock, LockData>::start() {

    std::cout << "Worker Start" << std::endl;

    // check threads have not being started

    // cycler assignments

    // start / create threads according to thread config
    for (unsigned int thread = 0; thread < maxThreads; ++thread) {
        create(thread); // will need to pass the
    }
}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::pause() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::stop() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::terminate() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::add(Lock *item, unsigned int process) {
    // get threads for the first process
    // check if cycler out of range
    std::cout << "--------" << std::endl;
//    std::cout << "Cycler: " << cyclers[process] << std::endl;

    if (cyclers[process] >= assignedThreads[process].size()) {
        cyclers[process] = 0; // reset
//        std::cout << "Cycler Corrected: " << cyclers[process] << std::endl;
    }
    // add item to threads process list
    WorkThread &thread = workThreads[assignedThreads[process][cyclers[process]]];

    thread.processes[this->threadProcessLookup[thread.id][process]].work.push_back(item);

    std::cout << "Added to thread " << assignedThreads[process][cyclers[process]]
              << "  for process " << process
              << "    at index " << thread.processes[this->threadProcessLookup[thread.id][process]].work.size()-1
              << std::endl;

    ++cyclers[process];
    thread.hasWork = true;
}


template<typename Lock, typename LockData>
void Worker<Lock, LockData>::update() {
    // Called upon construction
    assignedThreads.reserve(process.size());
    cyclers.reserve(process.size());
    workThreads.reserve(threads.size());
    threadProcessLookup.reserve(process.size());

    for (unsigned short proc = 0; proc < process.size(); ++proc) {
        cyclers.push_back(0);
        assignedThreads.push_back(std::vector<unsigned short int>(0));
    }

    // threads process assignments
    for (unsigned short thread = 0; thread < threads.size(); ++thread) {
        threadProcessLookup.push_back(std::vector<unsigned short int>(0));
        workThreads.push_back(std::move(WorkThread(this)));

        workThreads[thread].processes.reserve(threads[thread].size());

        for (unsigned short proc = 0; proc < threads[thread].size(); ++proc) {
            workThreads[thread].processes.push_back(std::move(WorkProcess()));
            threadProcessLookup[thread].push_back(workThreads[thread].processes.size() - 1);
            assignedThreads[threads[thread][proc]].push_back(thread);
            workThreads[thread].assigned.push_back(threads[thread][proc]);

            std::cout << "Process: " << threads[thread][proc] << "   For thread: " << thread << std::endl;
        }
    }

    for (unsigned int l = 0; l < assignedThreads.size(); ++l) {
        std::cout << "Assigned Process: " << l << " = ";

        for (unsigned short int p = 0; p < assignedThreads[l].size(); ++p) {
            std::cout << assignedThreads[l][p] << ", ";
        }

        std::cout << std::endl;
    }

    for (unsigned short int l = 0; l < cyclers.size(); ++l) {
        std::cout << "Cycler: " << l << " = " << cyclers[l] << std::endl;
    }

    std::cout << "SIZE OF CYCLERS: " << cyclers.size() << std::endl;
}

template<typename Lock, typename LockData>
Lock *Worker<Lock, LockData>::WorkThread::next() {
    // For the specific process cycle through all available items
    // TODO always look up the correct assigned slot via threadProcessLookup
    // Will need to clean up semantics and crap
    // Issue is when an item is moved to another thread its process is 2 but assigned to 0 so this->process is 0 when it should be 2
    // THIS PROCESS needs to the the correct assigned process
    // NEEDS TO FIND FROM worker What process it is working on to correct this->process

    if (this->currentItem > this->processes[this->process].work.size() - 1) {
        this->currentItem = 0; // back to start
    }

    if (this->process != this->assigned[0]) {
//        this->process = this->assigned[0];
    }

    Lock *work = this->processes[this->process].work[this->currentItem];

    std::cout << "      Next: T=" << this->id << " p=" << this->process << "   item: " << this->currentItem
              << "      ProcSize: " << this->processes.size() << "     Work: "
              << this->processes[this->process].work.size()
              << std::endl;

    ++this->currentItem;
    return work;
}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::WorkThread::done() {
    // Get next process
    // if no next process
    // check if any process has work
    // if so change process to the process that has work
    // else has work = false and we are now done with existing work load
    unsigned int nextProcess = this->process + 1;
    bool foundWork = false;

    std::cout << "      Done: T=" << this->id << " p=" << this->process << "  citem: " << this->currentItem - 1
              << "      Work: "
              << this->processes[this->process].work.size() << std::endl;

    if (nextProcess < this->processes.size()) {
        // there is a next processes on this thread get return to worker
        this->processes[nextProcess].work.emplace_back(
                std::move(this->processes[this->process].work[this->process])
        );

        this->processes[this->process].work.erase(
                this->processes[this->process].work.begin() + this->currentItem - 1
        );

        // check batch limit
        if (this->processes[nextProcess].work.size() >= this->worker->batch) {
            // batch is ready proceed to next process
            this->process = nextProcess;
        }
    } else {
        // no next process just erase the item, check if there is another thread having a process for the work

        if (nextProcess < this->worker->assignedThreads.size()) {
            // another process on another thread
            std::cout << "Moving work to another thread: " << nextProcess << "   citem: " << this->currentItem - 1
                      << std::endl;


            this->worker->add(
                    std::move(this->processes[this->process].work[this->currentItem - 1]),
                    nextProcess
            );
        }

        // delete if not
        this->processes[this->process].work.erase(
                this->processes[this->process].work.begin() + this->currentItem - 1
        );

        // check if no more work is available, check each process to see if there is any work outstanding
        for (unsigned int n = 0; n < this->processes.size(); ++n) {
            if (this->processes[n].work.size() > 0) {
                // found work
                this->process = n;
                foundWork = true;
                break;
            }
        }

        if (!foundWork) {
            // no more work for this thread, check upper if there are any other threads for the next process
            std::cout << "      Done: T=" << this->id << " p=" << this->process << "   NO WORK! " << std::endl;
            this->hasWork = false;
        }
    }
}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::workhandlerLocking(WorkThread &thread) {
    using namespace std::chrono_literals;

    while (true) { // process each item until no work CHANGE TO IF STATEMENT
        if (thread.hasWork) {
            Lock *work = thread.next(); // pointer to next item or nullptr if no valid item

            if (work) {
                LockData *write;

                switch (this->lockConfig[thread.process]) {
                    case LOCKCONFIG::READ: {
                        const LockData *read1 = work->lockRead();
                        //                        this->processes[this->process](read);
                        if (read1) {
                            work->unlockRead(read1);
                        }
                    }
                        break;
                    case LOCKCONFIG::WRITE: {
                        write = work->lockWrite();

                        if (write) {
                            work->unlockWrite(write);
                        }
                    }
                        break;
                    case LOCKCONFIG::READWRITE: {
                        const LockData *read2 = work->lockRead();
                        write = work->lockWrite();

                        if (read2) {
                            work->unlockRead(read2);
                        }
                        if (write) {
                            work->unlockWrite(write);
                        }

                    }
                        break;
                }

                thread.done();
            } else {
                std::cout << "FUBAR lock is left intact / or NULLPTR" << std::endl;
            }
        } else {
            // no work so wait
            std::cout << "Sleeping: T=" << thread.id << std::endl;
            std::this_thread::sleep_for(5s);
            std::cout << "Awake: T=" << thread.id << std::endl;
            // TODO change to while loop for has work and kill thread after x amount of no work
            //  or kill thread according to main thread (let thread end)
            if (!thread.hasWork) {
                return; // TODO REMOVE
            }
        }

        std::this_thread::sleep_for(1s);
    }
}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::workhandler(WorkThread &thread) {
    while (true) { // process each item until no work CHANGE TO IF STATEMENT
        if (thread.hasWork) {
            Lock *work = thread.next(); // pointer to next item or nullptr if no valid item
            std::cout << "Has work: " << thread.id << std::endl;

//            this->processes[this->process](work); // manual locking
        } else {
            // no work so wait
            std::cout << "Sleeping: " << thread.id << std::endl;
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
            std::cout << "Awake: " << thread.id << std::endl;
            // TODO change to while loop for has work and kill thread after x amount of no work
            //  or kill thread according to main thread (let thread end)
            return; // TODO REMOVE
        }
    }
}


/* -------------------------------------------------------------------------------------------------------/
 *      Thread Functions -- std::thread @TODO and DEFINES
 * ----------------------------------------------------------------------------------------------------*/
template<typename Lock, typename LockData>
void Worker<Lock, LockData>::create(unsigned int threadID) {
//    std::cout << "Creating Thread: #" << threadID << std::endl;
    // create thread, instance and threadID
    WorkThread &thread = this->workThreads[threadID];
    thread.id = threadID;

    threadPool.push_back(
            std::thread(
                    [&] {
                        // add work thread to thread list
                        // then call with it
                        if (this->autoLocking) {
                            this->workhandlerLocking(thread);
                        } else {
                            this->workhandler(thread);
                        }
                    }
            )
    );

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::detach() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::join() {
    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }
}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::swap() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::getPriority() {

}

template<typename Lock, typename LockData>
void Worker<Lock, LockData>::setPriority() {

}




/* -------------------------------------------------------------------------------------------------------/
 *      Thread Functions -- SDL2 threads @TODO and DEFINES
 * ----------------------------------------------------------------------------------------------------*/

#endif //GLTESTING_WORKER_H

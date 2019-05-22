#ifndef WORKER_H
#define WORKER_H

#include <vector>
#include <thread>
#include <functional>
#include <chrono>

#include "Stack.h"
#include "Thread.h"
#include "WorkMan.h"

#include "WorkManTypes.h"

class WorkMan;

/* =======================================================================================================/
 *  Worker Class:
 *      This is the parent to the stack class, this holds each workload configuration
 * ====================================================================================================*/

class Worker {
public:
    /* -------------------------------------------------------------------------------------------------------/
     *  Constructors
     * ----------------------------------------------------------------------------------------------------*/
    // Default Constructor
    Worker() {

    };

    /* -------------------------------------------------------------------------------------------------------/
     *  Constructor:
     *
            WORK::TEST_WORKLOAD_1,
            DISTRIBUTION::DYNAMIC_MODEL, // Global distribution model Static (Cannot be Condensed) Dynamic (Can be Condensed)
            5,      // Priority used for condensing and expanding
            true,   // Auto Locking
            {   // process IDs
                    PROCESS::A, PROCESS::B, PROCESS::C
            },
            {   // process functions (order corresponds to the order in the Process IDs)
                    (void*)processA, // PROCESS::A
                    (void*)processB, // PROCESS::B
                    (void*)processC  // PROCESS::C
            },
            {   // threads Config
                    {PROCESS::A, PROCESS::B},   // T0
                    {PROCESS::A, PROCESS::B},   // T1
                    {PROCESS::C},               // T2
                    {PROCESS::C}                // T3
            },
            {   // lock config
                    LOCK_CONFIG::READ,           // PROCESS::A
                    LOCK_CONFIG::READWRITE,      // PROCESS::B
                    LOCK_CONFIG::WRITE           // PROCESS::C
            },
            {   // Batch Settings
                    Batch(BATCH_TYPE::SIZE, 1000), // 1000 items for PROCESS::A
                    Batch(BATCH_TYPE::SIZE, 1000), // 1000 items for PROCESS::B
                    Batch(BATCH_TYPE::TIMED, 200, BATCH_TIMING::MS, true) // 200 ns for PROCESS::C using collected model (timer used for all threads)
            }

     * ----------------------------------------------------------------------------------------------------*/
    // Constructor
    Worker(
            unsigned int name,
            DISTRIBUTION distributionModel,
            int priority,
            bool autoLocking,
            std::vector<unsigned int> processIDs,
            std::vector<void *> processFunctions,
            std::vector<std::vector<unsigned int>> threadConfig,
            std::vector<LOCK_CONFIG> lockConfig,
            std::vector<Batch> batchConfig
    ) :
            name(name),
            distributionModel(distributionModel),
            priority(priority),
            autoLocking(autoLocking),
            processIDs(std::move(processIDs)),
            processFunctions(std::move(processFunctions)),
            threadConfig(std::move(threadConfig)),
            lockConfig(std::move(lockConfig)),
            batchConfig(std::move(batchConfig))
            {
        // update worker from config
//        update();
    };

    // copy constructor
    Worker(const Worker &next) = delete;

    // copy assignment
    Worker &operator=(const Worker &next) = delete;

    // move constructor
    Worker(Worker &&next) noexcept {
        next.name = name;
        next.distributionModel = distributionModel;
        next.priority = priority;
        next.autoLocking = autoLocking;
        next.processIDs = std::move(processIDs);
        next.processFunctions = std::move(processFunctions);
        next.threadConfig = std::move(threadConfig);
        next.lockConfig = std::move(lockConfig);
        next.batchConfig = std::move(batchConfig);
    };

    // move assignment
    Worker &operator=(Worker &&next) noexcept {
        next.name = name;
        next.distributionModel = distributionModel;
        next.priority = priority;
        next.autoLocking = autoLocking;
        next.processIDs = std::move(processIDs);
        next.processFunctions = std::move(processFunctions);
        next.threadConfig = std::move(threadConfig);
        next.lockConfig = std::move(lockConfig);
        next.batchConfig = std::move(batchConfig);

        return *this;
    };

    // move pointer constructor
    explicit Worker(Worker *next) {
        next->name = name;
        next->distributionModel = distributionModel;
        next->priority = priority;
        next->autoLocking = autoLocking;
        next->processIDs = std::move(processIDs);
        next->processFunctions = std::move(processFunctions);
        next->threadConfig = std::move(threadConfig);
        next->lockConfig = std::move(lockConfig);
        next->batchConfig = std::move(batchConfig);
    };

    /* -------------------------------------------------------------------------------------------------------/
     *  Types
     * ----------------------------------------------------------------------------------------------------*/
    struct Cycler {
        Cycler(
                unsigned int min, unsigned int max, unsigned int current
        ) :
                min(min),
                max(max),
                current(current) {}

        unsigned int min;
        unsigned int max;
        unsigned int current;
    };

    /* -------------------------------------------------------------------------------------------------------/
     *  Properties
     * ----------------------------------------------------------------------------------------------------*/
    // Config Input
    unsigned int name;
    unsigned int priority;
    bool autoLocking;
    DISTRIBUTION distributionModel;
    std::vector<unsigned int> processIDs;
    std::vector<void *> processFunctions;
    std::vector<std::vector<unsigned int>> threadConfig;
    std::vector<LOCK_CONFIG> lockConfig;
    std::vector<Batch> batchConfig;

    // Internal
private:
    std::vector<int> mThread; // threadIDs - list of each thread id
    std::vector<Stack> mStacks; // handlers - each handler and its work load / processes and where it is at each index
    std::vector<unsigned int> mLookup; // lookup - by process = which index in handlers for that process
    std::vector<Cycler> mCycler; // cycler - by process = list of { min, max, current } used for work delegation

    /* -------------------------------------------------------------------------------------------------------/
     *  Functions
     * ----------------------------------------------------------------------------------------------------*/

public:
    void start();
    void pause(); // pause will simply pause at the after the current work item is done
    void stop(); // stop will stop the work entirely (leaving x amount un processed)
    void kill(); // kill the thread and its handler
    void add(void* item); // add a work item (optionally to a specific process)
    void add(void* item, unsigned int process); // add a work item (optionally to a specific process)
    unsigned int condense(unsigned int amount); // condense workers threads down to free up threads
    unsigned int expand(unsigned int amount); // allow current worker to expand as threads are available
    void setParent(WorkMan &parent);

private:
    void update(); // updates the worker according to spec changes (or initial spec)
    WorkMan *mWorkMan;
};



#endif
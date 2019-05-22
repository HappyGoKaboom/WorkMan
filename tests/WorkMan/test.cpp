//
// Created by unknown on 04/01/19.
//

#include "test.h"
//#include "Handler.tcc"

#include <vector>
#include <any>
#include <iostream>
#include <thread>

// Process IDs
enum PROCESS {
    A,
    B,
    C
};

enum WORK { // Workload IDs
    TEST_WORKLOAD_1,
    TEST_WORKLOAD_2,
};

void setup() {
    // Worker thread configuration example
    Map<Lock> map(1000);
    static WorkMan workman;

    std::cout << "CPUS/Threads: " << std::thread::hardware_concurrency() << std::endl;

    // generate workloads
    for (unsigned int n = 0; n < 3; ++n) {
        map.add(Lock{{n, n}});
    }

    Worker MyWorkLoad {
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
    };

    Worker &myWorker = workman.create(std::move(MyWorkLoad));

//     add items to the worker
    for (unsigned int n = 0; n < map.count(); ++n) {
        myWorker.add(map[n]);
    }
//
//    myWorker.start(); // start the threads
//    myWorker.join(); // start the threads
}

void processA(Data* read) {
    std::cout << "Process A Running...." << std::endl;
}

void processB(Data* read, Data* write) {
    std::cout << "Process B Running...." << std::endl;
}

void processC(Data* write) {
    std::cout << "Process C Running...." << std::endl;

    // move to the next workload
}

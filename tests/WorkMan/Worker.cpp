#include "Worker.h"

/* -------------------------------------------------------------------------------------------------------/
 *  Start
 * ----------------------------------------------------------------------------------------------------*/

void Worker::start() {

}

/* -------------------------------------------------------------------------------------------------------/
 *  Pause
 * ----------------------------------------------------------------------------------------------------*/

void Worker::pause() {
}

/* -------------------------------------------------------------------------------------------------------/
 *  Stop
 * ----------------------------------------------------------------------------------------------------*/

void Worker::stop() {

}

/* -------------------------------------------------------------------------------------------------------/
 *  Kill
 * ----------------------------------------------------------------------------------------------------*/

void Worker::kill() {

}

/* -------------------------------------------------------------------------------------------------------/
 *  Add
 * ----------------------------------------------------------------------------------------------------*/

void Worker::add(void* item) {
    // adds to the first process delegated by cycler to each thread within that process

}

void Worker::add(void* item, unsigned int process) {
    // adds to a specific process delegated by cycler to each thread within that process

}

/* -------------------------------------------------------------------------------------------------------/
 *  condense
 * ----------------------------------------------------------------------------------------------------*/

unsigned int Worker::condense(unsigned int amount) {
    return 0;
}

/* -------------------------------------------------------------------------------------------------------/
 *  expand
 * ----------------------------------------------------------------------------------------------------*/

unsigned int Worker::expand(unsigned int amount) {
    return 0;
}

/* -------------------------------------------------------------------------------------------------------/
 *  update -- udpates the current configuration in play
 * ----------------------------------------------------------------------------------------------------*/

void Worker::update() {
/*
 * WORK::TEST_WORKLOAD_1,
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
 */



}


void Worker::setParent(WorkMan &parent) {
//    mParent = &parent;
}


#ifndef HANDLER_H
#define HANDLER_H

#include <vector>
#include "WorkManTypes.h"

/* =======================================================================================================/
 *  Handler Class - Thread Handler
 *      This is the child of the stack, each handler runs on a thread and holds the processes for its
 *      workload.
 *
 *      Items that come in from Workers cycler are placed in here
 * ====================================================================================================*/
class Handler {
    /* -------------------------------------------------------------------------------------------------------/
     * Constructors
     * ----------------------------------------------------------------------------------------------------*/
    // Default Constructor
    Handler() {

    };

    // Constructor
//    Handler(int a) : a(a) {
//
//    };

    // copy constructor
    Handler(const Handler &other) = delete;

    // copy assignment
    Handler &operator=(const Handler &other) = delete;

    // move constructor
    Handler(Handler &&other) noexcept {

    };

    // move assignment
    Handler &operator=(Handler &&other) noexcept {
        return *this;
    };

    // move pointer constructor
    explicit Handler(Handler *other) {

    };

    /* -------------------------------------------------------------------------------------------------------/
     *  Types
     * ----------------------------------------------------------------------------------------------------*/


    /* -------------------------------------------------------------------------------------------------------/
     *  Properties
     * ----------------------------------------------------------------------------------------------------*/
    std::vector<void*> workload; // Process = Asssigned Workloads are pointers to the lock holding the data
    std::vector<void*> processFunctions; // Process = process functions to send work to.

    unsigned int currentProcess;
    Batch batch;
    LOCK_CONFIG lockConfig;
    bool autoLocking;

    /* -------------------------------------------------------------------------------------------------------/
     *  Functions
     * ----------------------------------------------------------------------------------------------------*/
    void* next(); // get next available item returns a pointer to your item you must static_cast<Lock<Data>>

    void done(void* item); // completed move to next process
    void done(void* item, unsigned int process); // move to a specific process
    void done(void* item, void* Worker); // move to another worker
    void done(void* item, void* Worker, unsigned int process); // move to another worker and specific process

    void handlerLocking(YOURTYPE aType); // deals with auto locking
    void handlerNoLocking(YOURTYPE aType); // deals without locking
    void handler(); // generic handler to select your function without any specific handling
};


#endif
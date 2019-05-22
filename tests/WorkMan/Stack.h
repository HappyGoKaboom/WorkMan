//
// Created by unknown on 17/05/19.
//

#ifndef GLTESTING_STACK_TCC
#define GLTESTING_STACK_TCC

#include <vector>
#include "Handler.tcc"

/* =======================================================================================================/
 *  Stack Class
 *      This holds each worker and its work load, this is responsible for switching stacks when
 *      running in stacked mode, if there is more than 1 stack this means the Worker has stacked
 *      its workload onto another thread
 * ====================================================================================================*/

class Stack {
    // Default Constructor
    Stack() {

    };

    // Constructor
//    Stack(int a) : a(a) {
//
//    };

    // copy constructor
    Stack(const Stack &other) = delete;

    // copy assignment
    Stack &operator=(const Stack &other) = delete;

    // move constructor
    Stack(Stack &&other) noexcept {

    };

    // move assignment
    Stack &operator=(Stack &&other) noexcept {
        return *this;
    };

    // move pointer constructor
    explicit Stack(Stack *other) {

    };

    /* -------------------------------------------------------------------------------------------------------/
     *  Types
     * ----------------------------------------------------------------------------------------------------*/


    /* -------------------------------------------------------------------------------------------------------/
     *  Properties
     * ----------------------------------------------------------------------------------------------------*/
    std::vector<Handler> stacks;
    bool stacked = false;

    /* -------------------------------------------------------------------------------------------------------/
     *  Functions
     * ----------------------------------------------------------------------------------------------------*/
    void addHandler(); // adds a handler with configs
    void next();
    void add(Handler Handler);
    void remove(Handler Handler);
};

#endif //GLTESTING_STACK_TCC

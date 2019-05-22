//
// Created by unknown on 22/05/19.
//

#ifndef GLTESTING_WORKMANTYPES_H
#define GLTESTING_WORKMANTYPES_H

enum DISTRIBUTION {
    STATIC_MODEL,
    DYNAMIC_MODEL
};

enum LOCK_CONFIG {
    READ,
    WRITE,
    READWRITE
};

enum BATCH_TYPE {
    TIMED,
    SIZE
};

enum BATCH_TIMING {
    NS, // Nano second
    US, // Micro second
    MS, // Milli second
    S, // Second
    M, // Minute
    H // Hour
};

struct Batch {
    Batch (BATCH_TYPE type, unsigned int amount) : type(type), amount(amount) {
    };

    Batch (BATCH_TYPE type, unsigned int amount, BATCH_TIMING timing, bool collected) : type(type), amount(amount), timing(timing), collected(collected) {
    };

    BATCH_TYPE type;
    unsigned int amount;
    BATCH_TIMING timing;
    bool collected;
};

#endif //GLTESTING_WORKMANTYPES_H

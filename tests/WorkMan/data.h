//
// Created by unknown on 04/01/19.
//

#ifndef WORKER_DATA_H
#define WORKER_DATA_H

#include <memory>

class Data {
public:
    Data(long a, long b) : ma(std::move(a)), mb(std::move(b)) {
        // CTOR
//        std::cout << "CTOR" << std::endl;
//        std::cout << "    ma: " << ma << std::endl;
//        std::cout << "    mb: " << mb << std::endl;
    }

    Data() {
//        std::cout << "CTOR default" << std::endl;
    }

    ~Data() {
        // DTOR
//        std::cout << "DTOR" << std::endl;
    }

    Data(const Data &other) {
//        std::cout << "COPY" << std::endl;
        this->ma = other.ma;
        this->mb = other.mb;
    };

    Data(Data &&other) noexcept : ma(std::move(other.ma)), mb(std::move(other.mb)) {
        // move
//        std::cout << "MOVE" << std::endl;
    }

    Data &operator=(const Data &other) {
        // copy assignment
//        std::cout << "COP ASSIGNMENT" << std::endl;
        this->ma = other.ma;
        this->mb = other.mb;
        return *this;
    }

    Data &operator=(Data &&other) noexcept {
        // move assignment
//        std::cout << "MOVE ASSIGNMENT" << std::endl;
        this->ma = std::move(other.ma);
        this->mb = std::move(other.mb);
        return *this;
    }

    long ma;
    long mb;
};


#endif //GLTESTING_DATA_H

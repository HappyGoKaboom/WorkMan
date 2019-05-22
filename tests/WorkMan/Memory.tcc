//
// Created by unknown on 03/09/18.
//

#ifndef INC_3DVOXEL_MEMORY_H
#define INC_3DVOXEL_MEMORY_H

#include <algorithm>
#include <vector>
#include <cstddef>
#include <iostream>


template<typename T>
class Map;
/* =======================================================================================================/
 *      Locking Mechanisms
 * ====================================================================================================*/

/* -------------------------------------------------------------------------------------------------------/
 *      Read / Write Exclusive Lock
 *          This allows multiple readers but only one writer
 * ----------------------------------------------------------------------------------------------------*/
//        template<typename T, typename U> class Map;

template<typename T>
class RWExclusive {


public:
    RWExclusive(T item) : mData(item) {
//                std::cout << "CTOR InitList" << std::endl;
        mData = mData;
    };

    RWExclusive() {
//                 std::cout << "CTOR" << std::endl;
    };

    // destructor
    ~RWExclusive() noexcept {
//                 std::cout << "DTOR" << std::endl;
        // user handles destruction of there own data / item
    };

    const T *lockRead();

    void unlockRead();

    T *lockWrite();

    void unlockWrite();

private:
    int mReadLocked = 0;
    int mWriteLocked = 0;
    T mData;
    unsigned long mapIndex;
};

template<typename T>
const T *RWExclusive<T>::lockRead() {
    if (!mWriteLocked) {
//                std::cout << "Read Lock" << std::endl;
        ++mReadLocked;
        return &mData;
    }
    else {
//                std::cout << "Cannot obtain Read Lock" << std::endl;
        return nullptr;
    }
}

template<typename T>
void RWExclusive<T>::unlockRead() {
    if (!mWriteLocked) {
//                std::cout << "Read UnLock" << std::endl;
        --mReadLocked;
    }
}

template<typename T>
T *RWExclusive<T>::lockWrite() {
    if (!mReadLocked && !mWriteLocked) {
//                std::cout << "Write Lock" << std::endl;
        ++mWriteLocked;
        return &mData;
    }
    else {
//                std::cout << "Cannot obtain Write Lock" << std::endl;
        return nullptr;
    }
}

template<typename T>
void RWExclusive<T>::unlockWrite() {
    if (mWriteLocked > 0) {
//                std::cout << "Write UnLock" << std::endl;
        --mWriteLocked;
    }
}

/* -------------------------------------------------------------------------------------------------------/
 *      Read / Write Queue Lock
 *          This allows simultaneous multiple readers and writers
 *          single read data
 *          copy data into the write data
 *          update on unlock (immediate or delayed if readlocks are in place)
 * ----------------------------------------------------------------------------------------------------*/
template<typename T>
class RWQueue {
    template<typename U> friend
    class Map;

public:
    explicit RWQueue(T &&item) : mReadData(std::move(item)), mWriteData(mReadData) {
//                std::cout << "RWQueue CTOR InitList" << std::endl;
    };

    // destructor
    ~RWQueue() noexcept {
//                 std::cout << "RWQueue DTOR" << std::endl;
        // user handles destruction of there own data / item
    };

    // copy constructor
    RWQueue(const RWQueue &other) = delete;

    // copy assignment
    RWQueue &operator=(const RWQueue &other) = delete;

    // move constructor
    RWQueue(RWQueue &&other) noexcept {
//                std::cout << "RWQueue MOVE &&" << std::endl;
        mReadData = std::move(other.mReadData);
        mWriteData = std::move(other.mWriteData);
        mReadLocked = std::move(other.mReadLocked);
        mWriteLocked = std::move(other.mWriteLocked);
        mapIndex = std::move(other.mapIndex);
    }

    // move assignment
    RWQueue &operator=(RWQueue &&other) noexcept {
//                std::cout << "RWQueue MOVE=" << std::endl;
        mReadData = std::move(other.mReadData);
        mWriteData = std::move(other.mWriteData);
        mReadLocked = std::move(other.mReadLocked);
        mWriteLocked = std::move(other.mWriteLocked);
        mapIndex = std::move(other.mapIndex);
        return *this;
    }

    // move pointer constructor
    explicit RWQueue(RWQueue *other) {
//                std::cout << " RWQueue MOVE *" << std::endl;
        mReadData = std::move(other->mReadData);
        mWriteData = std::move(other->mWriteData);
        mReadLocked = std::move(other->mReadLocked);
        mWriteLocked = std::move(other->mWriteLocked);
        mapIndex = std::move(other->mapIndex);
    }

    const T *lockRead();

    void unlockRead(const T *&ptr);

    T *lockWrite();

    void unlockWrite(T *&ptr); // this updates the read copy
    const int getReaders();

    const int getWriters();

    const unsigned long getIndex() {
        return mapIndex;
    }

private:
    int mReadLocked = 0;
    int mWriteLocked = 0;
    bool mReadDeny = false; // deny whilst update is being performed
    bool mUpdatePending = false; // is there an update pending
    T mReadData;
    T mWriteData;
    unsigned long mapIndex = 0;
};

template<typename T>
const T *RWQueue<T>::lockRead() {
    if (!mReadDeny) {
        ++mReadLocked;
        return &mReadData;
    }
    else {
        return nullptr;
    }
}

template<typename T>
void RWQueue<T>::unlockRead(const T *&ptr) {
    ptr = nullptr; // destroy pointer so we cannot use the pointer after unlock is called

    if (mReadLocked) {
        --mReadLocked;
    }

    if (mUpdatePending && mReadLocked == 0 && mUpdatePending == 0) {
        // perform update if no locks in place
        mUpdatePending = true; // no reads whilst updating the read version
        mReadData = mWriteData;
        mUpdatePending = false; // reads can now be done
    }
}

template<typename T>
T *RWQueue<T>::lockWrite() {
    ++mWriteLocked;
    return &mWriteData;
}

template<typename T>
void RWQueue<T>::unlockWrite(T *&ptr) {
    if (mWriteLocked) {
        ptr = nullptr; // destroy pointer so we cannot use the pointer after unlock is called
        --mWriteLocked;

        if (!mReadLocked && !mWriteLocked) {
            // do immediate update as no read or write locks in place
            mUpdatePending = true; // no reads whilst updating the read version
            mReadData = mWriteData;
            mUpdatePending = false; // reads can now be done
        }
        else {
            // await read unlock
            mUpdatePending = true;
        }
    }
}

template<typename T>
const int RWQueue<T>::getReaders() {
    return mReadLocked; // get count
}

template<typename T>
const int RWQueue<T>::getWriters() {
    return mWriteLocked; // get count
}

/* =======================================================================================================/
 *      Map - handles free lists
 * ====================================================================================================*/
template<typename T>
class Map {
public:
    Map(size_t reserve) {
        mMap.reserve(reserve);
    };

    T *operator[](unsigned long index) {
        // range check !!!
        return &mMap[index];
    }

    T &add(T &&item);
    unsigned long count();

    void free(T &item);

    unsigned long getFragmentCount();

    void purge();

private:
    std::vector<unsigned long> mFragments;
    size_t mContiguous = 0;
    std::vector<T> mMap;
};

template<typename T>
T &Map<T>::add(T &&item) {
    if (!mFragments.empty()) {
        // use last fragment index
        unsigned long index = mFragments[mFragments.size() - 1];
        item.mapIndex = index;

        mMap[index] = std::move(item);
        mFragments.pop_back();

        return (T &) mMap[index];
    }
    else {
        // contiguous
        unsigned long index = mMap.size();
        ++mContiguous;
        item.mapIndex = index; // set index in the lock

        mMap.emplace_back(std::move(item));

        return (T &) mMap[index];
    }
}

template<class T>
void Map<T>::free(T &item) {
    std::cout << item.mapIndex << ":" << mContiguous << std::endl;

    if (item.mapIndex < mContiguous) {
        // add to fragments
        mFragments.emplace_back(item.mapIndex);
    }
    else {
        // is equal to mContiguous ie last item
        --mContiguous;
    }
}

template<class T>
unsigned long Map<T>::getFragmentCount() {
    return mFragments.size();
}

template<class T>
void Map<T>::purge() {
    // Removes all free fragments
    if (!mFragments.empty()) {
        std::sort(mFragments.begin(), mFragments.end());

        // erase in reverse order to prevent deallocations screwing up indexes
        for (size_t n = 0; n < mFragments.size(); ++n) {
            mMap.erase(mMap.begin() + mFragments[mFragments.size() - 1 - n]);
        }
    }

    // correct indexes
    for (unsigned long n = 0; n < mMap.size(); ++n) {
        mMap[n].mapIndex = n;
    }

    mFragments.clear();
    mContiguous -= mFragments.size();
}

template<typename T>
unsigned long Map<T>::count() {
    return mMap.size();
}

#endif //INC_3DVOXEL_MEMORY_H


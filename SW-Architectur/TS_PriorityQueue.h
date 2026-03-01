#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T, typename Compare>
class ThreadSafePriorityQueue {
    std::priority_queue<T, std::vector<T>, Compare> queue;
    std::mutex mtx;
    std::condition_variable cv;
public:
    explicit ThreadSafePriorityQueue(Compare cmp = Compare{})
        : queue(std::move(cmp)) {}

    void push(const T& v) {
        std::lock_guard lk(mtx);
        queue.push(v);
        
        cv.notify_one();
    }
    T wait_and_pop() {
        std::unique_lock lk(mtx);
        cv.wait(lk, [this]{ return !queue.empty(); });
        T val = queue.top();
        queue.pop();
        return val;
    }
    bool empty() const {
        std::lock_guard lk(mtx);
        return queue.empty();
    }
};
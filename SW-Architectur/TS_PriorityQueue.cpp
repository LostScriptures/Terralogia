#include <queue>
#include <mutex>
#include <condition_variable>

#include "TS_PriorityQueue.h"

template<typename T, typename Compare>
class ThreadSafePriorityQueue {
private:
    std::priority_queue<T, std::vector<T>, Compare> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    explicit ThreadSafePriorityQueue(Compare cmp = Compare{})
        : queue(std::move(cmp)) // or queue(cmp)
    {}
    
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
        cv.notify_one();
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });

        T value = queue.top();
        queue.pop();
        return value;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};
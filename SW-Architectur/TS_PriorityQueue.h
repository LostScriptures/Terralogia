#pragma once
template<typename T, typename Compare>
class ThreadSafePriorityQueue {
private:
    std::priority_queue<T, std::vector<T>, Compare> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    explicit ThreadSafePriorityQueue(Compare cmp = Compare{})
        : queue(std::move(cmp))
    {}
    
    void push(const T& value);
    T wait_and_pop();
    bool empty();
};
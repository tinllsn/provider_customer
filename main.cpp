#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> buffer;
const unsigned int MAX_BUFFER_SIZE = 10;
std::mutex mtx;
std::condition_variable cv_empty;
std::condition_variable cv_full;

void provider() {
    int item = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_full.wait(lock, [] { return buffer.size() < MAX_BUFFER_SIZE; });

        buffer.push(item);
        std::cout << "Produced: " << item << std::endl;
        item++;

        lock.unlock();
        cv_empty.notify_one();

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }
}

void customer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_empty.wait(lock, [] { return !buffer.empty(); });

        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << item << std::endl;

        lock.unlock();
        cv_full.notify_one();

        std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Simulate work
    }
}

int main() {
    std::thread producer_thread(provider);
    std::thread consumer_thread(customer);

    producer_thread.join();
    consumer_thread.join();

    return 0;
}

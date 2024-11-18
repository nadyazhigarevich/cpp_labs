#include "pch.h"

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console_logger");

class CashRegister {
private:
    int id;
    std::queue<int> queue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> active;

public:
    CashRegister(int id) : id(id), active(true) {}

    void serveCustomer(int customerId) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            logger->info("Cash Register {} is serving Customer {}", id, customerId);
        }

        int servingTime = rand() % 3 + 1;
        std::this_thread::sleep_for(std::chrono::seconds(servingTime));

        {
            std::lock_guard<std::mutex> lock(mutex);
            logger->info("Customer {} is done at Cash Register {}", customerId, id);
        }
    }

    void addCustomer(int customerId) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(customerId);
        }
        cv.notify_one();
    }

    void processQueue() {
        while (active) {
            int customerId;
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [this]() { return !queue.empty() || !active; });
                if (!active && queue.empty()) return;
                if (!queue.empty()) {
                    customerId = queue.front();
                    queue.pop();
                }
                else {
                    continue;
                }
            }
            serveCustomer(customerId);
        }
    }

    void stop() {
        active = false;
        cv.notify_all();
    }

    int getId() const {
        return id;
    }

    bool isActive() const {
        return active;
    }

    void setActive(bool state) {
        active = state;
    }
};

class Restaurant {
private:
    std::vector<std::unique_ptr<CashRegister>> registers;
    std::vector<std::thread> threads;

public:
    Restaurant(int numRegisters) {
        for (int i = 0; i < numRegisters; ++i) {
            registers.emplace_back(std::make_unique<CashRegister>(i + 1));
            threads.emplace_back(&CashRegister::processQueue, registers.back().get());
        }
    }

    void addCustomer(int customerId, std::uniform_int_distribution<int>& distribution, std::default_random_engine& generator) {
        int registerIndex = distribution(generator);
        registers[registerIndex]->addCustomer(customerId);
    }

    void stopRegisters() {
        for (auto& reg : registers) {
            reg->stop();
        }
    }

    void joinThreads() {
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

int main() {
    const int numCustomers = 20;
    const int numRegisters = 5;
    Restaurant restaurant(numRegisters);

    std::vector<std::thread> customerThreads;
    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, numRegisters - 1);

    for (int i = 0; i < numCustomers; ++i) {
        customerThreads.emplace_back(&Restaurant::addCustomer, &restaurant, i + 1, std::ref(distribution), std::ref(generator));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    for (auto& thread : customerThreads) {
        thread.join();
    }

    restaurant.stopRegisters();
    restaurant.joinThreads();

    return 0;
}
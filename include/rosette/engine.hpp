#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace rosette {

class Scheduler;
class Port;
class Mempool;

class Engine {
public:
    Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool init(int argc, char** argv);
    void start();
    void stop();

    bool is_running() const { return running_; }

private:
    bool running_{false};
    std::unique_ptr<Scheduler> scheduler_;
    std::vector<std::unique_ptr<Port>> ports_;
    std::vector<std::unique_ptr<Mempool>> mempools_;
};

} // namespace rosette

#pragma once
#include <thread>

#include <types.h>
namespace Plusnx::Cpu {
    class CoreContainer {
    public:
        CoreContainer() = default;
        void RunThread(const std::stop_token& stop);

        void Initialize();

        void Destroy();
    private:
        std::optional<std::jthread> thread;
        u64 coreId{};
    };
}
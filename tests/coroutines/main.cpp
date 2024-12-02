#include <cpu/coroutine.h>
#include <types.h>

int main() {
    Plusnx::Cpu::Coroutine::Initialize();

    Plusnx::Cpu::Create(0, [] {
        std::print("Hello ");
        Plusnx::Cpu::Coroutine::Yield();
        std::print("Gabriel ");
    });

    constexpr auto SmallerStackSize{4 * 1024};
    Plusnx::Cpu::Create(SmallerStackSize, [] {
        std::print("World\n");
        Plusnx::Cpu::Coroutine::Yield();
        std::print("Correia\n");
    });

    Plusnx::Cpu::Go();

    std::print("Control returned to the main function\n");

    Plusnx::Cpu::Coroutine::Destroy();
}
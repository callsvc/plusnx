#pragma once

#include <types.h>
namespace Plusnx::Cpu {
    u64 GetThreadStackSize();
    u64 GetRngValue();

    constexpr auto CommonStackSize{4 * 1024 * 1024};

    enum class ReusableState {
        Free,
        Running,
        Stopped,
    };

    using CallBack = std::function<void()>;

    // https://c9x.me/articles/gthreads/code0.html
    class Coroutine {
    public:
        Coroutine() = default;
        Coroutine(u64 block, CallBack&& fn);

        [[noreturn]] static void Start();
        [[noreturn]] static void Finish();

        static bool Yield();

        std::vector<u8> stack;
        ReusableState state;
        u64 rng{};

        struct {
#if defined(__x86_64__)
            u64 rsp;
            u64 r15, r14, r13, r12;
            u64 rbx;
            u64 rbp;
#endif
        } thread;

        CallBack function;

        void Activate();
        void Deactivate();
    };

    class CoroutinePool {
    public:
        static void Swap(Coroutine& prev, Coroutine& next);
        Coroutine& Create(u64 stackSize, CallBack&& cb);

        void PrintStates();

        void Go();
        std::map<u64, Coroutine> grths;
    };

    extern CoroutinePool ulths;
    extern thread_local u64 local_gid;
}
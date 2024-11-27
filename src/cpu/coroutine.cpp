#include <fstream>
#include <ranges>

#include <pthread.h>

#include <sys_fs/fsys/regular_file.h>
#include <cpu/coroutine.h>

namespace Plusnx::Cpu {
    std::unique_ptr<CoroutinePool> storage;
    thread_local u64 local_gid;

#if defined(__x86_64__)
    // https://www.felixcloutier.com/x86/stmxcsr
    // https://www.felixcloutier.com/x86/fldcw

    __attribute__((naked)) void CoroutinePool::SaveRestoreAndJump(void* prev, void* next) {
        __asm("mov %rsp, 0x0(%rdi)");
        __asm("mov %r15, 0x8(%rdi)");
        __asm("mov %r14, 0x10(%rdi)");
        __asm("mov %r13, 0x18(%rdi)");
        __asm("mov %r12, 0x20(%rdi)");
        __asm("mov %rbx, 0x28(%rsp)");
        __asm("mov %rbp, 0x30(%rdi)");
        __asm("stmxcsr 0x38(%rdi)");
        __asm("fstcw 0x3C(%rdi)");

        __asm("mov 0x0(%rsi), %rsp");
        __asm("mov 0x8(%rsi), %r15");
        __asm("mov 0x10(%rsi), %r14");
        __asm("mov 0x18(%rsi), %r13");
        __asm("mov 0x20(%rsi), %r12");
        __asm("mov 0x28(%rsi), %rbx");
        __asm("mov 0x30(%rsi), %rbp");
        __asm("ldmxcsr 0x38(%rsi)");
        __asm("fldcw 0x3C(%rsi)");
        __asm("ret");
    }
#endif

    u64 GetThreadStackSize() {
        u64 pthSize{};

        pthread_attr_t attr;
        pthread_getattr_np(pthread_self(), &attr);
        pthread_attr_getstacksize(&attr, &pthSize);
        pthread_attr_destroy(&attr);

        return pthSize;
    }
    u64 GetRngValue() {
        SysFs::FSys::RegularFile random("/dev/urandom");
        if (random)
            return random.Read<u64>();
        return {};
    }

    void Coroutine::Initialize() {
        if (!storage)
            storage = std::make_unique<CoroutinePool>();
#if AFFILIATION_MODE
        storage->parent = std::this_thread::get_id();
#endif
    }
    void Coroutine::Destroy() {
#if AFFILIATION_MODE
        assert(storage->parent == std::this_thread::get_id());
#endif
        for (const auto& gthread: storage->grths | std::views::values) {
            if (gthread.rng && gthread.state == ReusableState::Running)
                throw runtime_plusnx_except("Coroutine {} is still running", gthread.rng);
        }

        {
            auto alive{std::move(storage)};
        }
    }

    Coroutine::Coroutine(u64 block, CallBack&& fn) {
        if (!block)
            block = GetThreadStackSize();
        function = std::move(fn);
        if (stack.size() < block)
            stack.resize(block);

        rng = GetRngValue();

        struct StackBegin {
            uintptr_t start;
            uintptr_t control;
        };

        const auto knowable{stack.begin() + stack.size()};
        const auto trapstack{
            StackBegin{
                .start = reinterpret_cast<uintptr_t>(Start),
                .control = reinterpret_cast<uintptr_t>(Finish),
            }
        };
        static_assert(sizeof(trapstack) == 16);
        std::memcpy(&*knowable - 16, &trapstack, sizeof(trapstack));

        thread = {
            .rsp = reinterpret_cast<u64>(&*knowable - 16)
        };

        state = ReusableState::Stopped;
    }

    void Coroutine::Activate() {
        local_gid = rng;
        state = ReusableState::Running;
    }
    void Coroutine::Deactivate() {
        local_gid = {};
        if (state == ReusableState::Running)
            state = ReusableState::Stopped;
    }

    void Coroutine::Start() {
        if (const auto callback = storage->grths[local_gid].function)
            callback();
        Finish();
    }
    void Coroutine::Finish() {
#if 0
        std::print("The coroutine has completed its task\n");
#endif
        storage->grths[local_gid].state = ReusableState::Free;
        storage->grths[local_gid].function = {};
        storage->grths[local_gid].thread = {};

        for (; ;) {
            Yield();
        }
    }

    bool Coroutine::Yield() {
        std::optional<u64> next;
        std::optional<u64> current;
        auto& threads{storage->grths};

#if 0
        storage->PrintStates();
#endif

        for (const auto& [gid, routine] : threads | std::views::reverse) {
            if (next && current)
                break;
            if (gid == local_gid)
                if (current.emplace(routine.rng); current)
                    continue;

            if (routine.state != ReusableState::Stopped)
                continue;
            next.emplace(routine.rng);
        }

        storage->Swap(threads[*current], threads[*next]);

        return true;
    }

    void CoroutinePool::Swap(Coroutine& prev, Coroutine& next) {
        prev.Deactivate();
        next.Activate();

        SaveRestoreAndJump(&prev.thread, &next.thread);
    }
    Coroutine& CoroutinePool::Create(const u64 stackSize, CallBack&& cb) {
        for (auto& threads: grths | std::views::values) {
            if (threads.state != ReusableState::Free)
                continue;

            threads = Coroutine(stackSize, std::move(cb));
            return threads;
        }

        Coroutine green(stackSize, std::move(cb));
        const auto rng{green.rng};
        grths.emplace(rng, std::move(green));
        return grths[rng];
    }
    void CoroutinePool::PrintStates() {
        for (const auto& [rng, grctx] : grths) {
            std::print("Signature: {}\n", rng);
            std::print("State: {}\n", std::to_underlying(grctx.state));
            std::print("Stack size: {}\n", grctx.stack.size());

#if defined(__x86_64__)
            std::print("%RSP: {}\n", grctx.thread.rsp);
            std::print("%RBP: {}\n", grctx.thread.rbp);
#endif
        }
    }

    void CoroutinePool::Go() {
        if (grths.empty())
            return;

        Coroutine context{};
        context.state = ReusableState::Stopped;
        for (const auto& green: std::ranges::views::values(grths)) {
            if (green.state == ReusableState::Free)
                continue;

            grths.emplace(0, std::move(context));
            // We cannot use the green object directly, as we are swapping the stack
            Swap(grths[0], grths[green.rng]);
            break;
        }
    }

    Coroutine& Create(const u64 stackSize, CallBack&& cb) {
        return storage->Create(stackSize, std::move(cb));
    }
    void Go() {
        storage->Go();
    }

}

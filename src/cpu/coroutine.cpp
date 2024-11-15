#include <fstream>
#include <ranges>
#include <thread>

#include <pthread.h>

#include <sys_fs/fsys/regular_file.h>
#include <cpu/coroutine.h>
extern void YieldContext(void* previous, void* next);

namespace Plusnx::Cpu {
    CoroutinePool ulths;
    thread_local u64 local_gid;

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

    Coroutine::Coroutine(u64 block, CallBack &&fn) {
        if (!block)
            block = GetThreadStackSize();
        function = std::move(fn);
        if (stack.size() < block)
            stack.resize(block);

        // ReSharper disable once CppDFAConstantConditions
        if (!rng)
            rng = GetRngValue();

        *reinterpret_cast<u64 *>(&stack[stack.size() - 0x8]) = reinterpret_cast<u64>(Finish);
        *reinterpret_cast<u64 *>(&stack[stack.size() - 0x10]) = reinterpret_cast<u64>(Start);

        std::memset(&thread, 0, sizeof(thread));
        thread.rsp = reinterpret_cast<u64>(&stack[stack.size() - 0x10]);

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
        if (const auto callback = ulths.grths[local_gid].function)
            callback();
        Finish();
    }
    void Coroutine::Finish() {
#if 0
        std::print("The coroutine has completed its task\n");
#endif
        ulths.grths[local_gid].state = ReusableState::Free;
        ulths.grths[local_gid].function = {};
        ulths.grths[local_gid].thread = {};

        for (; ;) {
            Yield();
        }
    }

    bool Coroutine::Yield() {
        std::optional<u64> next;
        std::optional<u64> current;
        auto& threads{ulths.grths};

#if 0
        ulths.PrintStates();
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

        ulths.Swap(threads[*current], threads[*next]);

        return true;
    }

    void CoroutinePool::Swap(Coroutine& prev, Coroutine& next) {
        prev.Deactivate();
        next.Activate();

        YieldContext(&prev.thread, &next.thread);
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
            std::print("State: {}\n", static_cast<u32>(grctx.state));
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
        for (const auto & green : std::ranges::views::values(grths)) {
            if (green.state == ReusableState::Free)
                continue;

            grths.emplace(0, std::move(context));
            // We cannot use the green object directly, as we are swapping the stack
            Swap(grths[0], grths[green.rng]);
            break;
        }
    }

}

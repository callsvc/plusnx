#pragma once

#include <nxk/base/kauto_object.h>
#include <nxk/kernel.h>
namespace Plusnx::Nxk::Types {
    class KThread final : public Base::KSynchronizationObject, public std::enable_shared_from_this<KThread> {
    public:
        explicit KThread(Kernel& kernel) : KSynchronizationObject(kernel, Base::KAutoType::KThread) {}
        void Initialize(u8* _entry, u8* tls, u8* topstack);

        void Run() const;
        void Kill();

        u8* entry{nullptr};
        u8* stack{nullptr};
        u8* threadTls{nullptr};

        u64 threadId{};
        bool running{};
        bool waiting{};
    private:
        std::array<char, 16> oldName{};
    };
}

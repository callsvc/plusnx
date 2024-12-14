#pragma once

#include <nxk/base/kauto_object.h>
#include <nxk/kernel.h>
namespace Plusnx::Nxk::Types {
    class KThread final : public Base::KSynchronizationObject {
    public:
        explicit KThread(Kernel& kernel) : KSynchronizationObject(kernel, Base::KAutoType::KThread) {}
        void Initialize(u8* entry, u8* tls, u8* topstack);

        void Start();
        void Stop() const;

        u8* entry{nullptr};
        u8* stack{nullptr};
        u64 threadId{};

    private:
        std::array<char, 16> oldName{""};
    };
}

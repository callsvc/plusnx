#include <nxk/types/kprocess.h>
#include <nxk/address_space.h>

#include <boost/align/align_up.hpp>

#if !defined(NATIVE_CODE_EXECUTION)
#define NATIVE_CODE_EXECUTION 0
#endif

namespace Plusnx::Nxk {
    u32 GetHostBitsCount(AddressSpaceType type) {
#if NATIVE_CODE_EXECUTION
        if (type == AddressSpaceType::AddressSpace64Bit)
            return 38;
#else
        if (type == AddressSpaceType::AddressSpace64Bit)
            return 39;
#endif
        if (type == AddressSpaceType::AddressSpace64BitOld)
            return 36;

        return 32;
    }

    void CreateUserAddressSpace(const std::shared_ptr<Types::KProcess>& process, const std::unique_ptr<MemoryNx>& buffer, const std::array<RegionProperties*, 0x5>& regions) {
        const auto& creation{process->creation};

        if (creation->addressType != AddressSpaceType::AddressSpace64Bit)
            return;

        const auto codeSize{creation->codeNumPages * SwitchPageSize};
        const auto base{buffer->guest};

        // CODE
        *regions[0] = {base->data(), boost::alignment::align_up(codeSize, RegionAlignment)};
        // ALIAS
        *regions[1] = {regions[0]->end().base(), VirtualMemorySpace39::AliasSize};
        // HEAP
        *regions[2] = {regions[1]->end().base(), VirtualMemorySpace39::HeapSize};
        // STACK
        *regions[3] = {regions[2]->end().base(), VirtualMemorySpace39::StackSize};
        // TLS/IO
        *regions[4] = {regions[3]->end().base(), VirtualMemorySpace39::TlsIoSize};
    }
}

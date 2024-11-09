#include <sys/mman.h>
#include <armored/code_blocks.h>
namespace Plusnx::Armored {
    CodeBlocks::CodeBlocks(const std::shared_ptr<EmitterInterface>& arm, const u64 count) : emitter(arm) {
        if (count)
            Expand(count / sysconf(_SC_PAGE_SIZE));

        Protect(0, 0, CodeBlockProtectionStatus::Disable);
        const u64 instructions{count / emitter->details->GetInstructionSize()};
        emitter->ChangeBlockScheme(this);

        for (u64 robot{}; instructions % 8 == 0 && robot < instructions; robot += 8) {
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
            emitter->EmitNop();
        }
        Protect(0, 0, CodeBlockProtectionStatus::Disable);
    }

    CodeBlocks::~CodeBlocks() {
        emitter->ChangeBlockScheme();
        munmap(segment, size);
    }

    void CodeBlocks::Protect(const u64 starts, u64 ends, const CodeBlockProtectionStatus action) const {
        auto flags{PROT_READ | PROT_WRITE | PROT_EXEC};
        if (action == CodeBlockProtectionStatus::Enable)
            flags &= ~PROT_EXEC;

        if (!ends)
            ends = size;

        mprotect(segment, ends - starts, flags);
    }

    void CodeBlocks::Expand(const u64 holes) {
        const auto total{holes * sysconf(_SC_PAGE_SIZE)};
        if (size > total)
            return;
        constexpr auto flags{PROT_NONE};
        if (!segment)
            segment = mmap(nullptr, total, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        else
            segment = mremap(segment, total, flags, MREMAP_FIXED);

        if (segment != MAP_FAILED) {
            size = total;
            return;
        }
        status = CodeBlockStatus::MemoryMappingFailed;
    }
}

#include <sys/mman.h>
#include <armored/code_blocks.h>
namespace Plusnx::Armored {
    CodeBlocks::CodeBlocks(const std::shared_ptr<EmitterInterface>& arm) : emitter(arm) {}
    void CodeBlocks::Initialize(const u64 count) {
        if (count)
            Expand(count / sysconf(_SC_PAGE_SIZE));

        Protect(0, 0, CodeBlockProtectionStatus::Disable);
        const u64 instructions{count / emitter->details->GetInstructionSize()};

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
        assert(munmap(executable, size) == 0);
    }

    void CodeBlocks::Protect(const u64 starts, u64 ends, const CodeBlockProtectionStatus action) const {
        auto flags{PROT_READ | PROT_WRITE | PROT_EXEC};
        if (action == CodeBlockProtectionStatus::Enable)
            flags &= ~PROT_EXEC;

        if (!ends)
            ends = size;

        assert(mprotect(executable, ends - starts, flags) == 0);
    }

    void CodeBlocks::Expand(const u64 holes) {
        const auto total{holes * sysconf(_SC_PAGE_SIZE)};
        if (size > total)
            return;
        constexpr auto flags{PROT_NONE};
        if (!executable)
            executable = mmap(nullptr, total, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        else
            executable = mremap(executable, total, flags, MREMAP_FIXED);

        if (executable != MAP_FAILED) {
            size = total;
            return;
        }
        status = CodeBlockStatus::MemoryMappingFailed;
    }
}

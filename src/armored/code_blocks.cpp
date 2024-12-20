#include <sys/mman.h>

#include <armored/backend/x86_64_emitter_context.h>
#include <armored/code_blocks.h>
namespace Plusnx::Armored {
    CodeBlocks::CodeBlocks(const std::shared_ptr<Backend::EmitterGenerator>& generator) : emitter(generator) {}

    void CodeBlocks::Initialize(const u64 code) {
        if (code)
            Expand(code / sysconf(_SC_PAGE_SIZE));

        Protect(exec, &exec[code], CodeBlockProtectionStatus::Disable);
        const u64 instructions{code / emitter->backing->GetInstructionSize(true)};
        for (u64 slot{}; slot < instructions; slot++) {
            emitter->EmitNop();
        }
        Protect(exec, &exec[code], CodeBlockProtectionStatus::Enable);
    }

    CodeBlocks::~CodeBlocks() {
        emitter->blockMap = {};
        emitter->codeMap = {};
        assert(munmap(exec, size) == 0);
    }

    void CodeBlocks::Protect(u8* begin, const u8* end, const CodeBlockProtectionStatus action) const {
        const auto flags = [&] {
            auto base{PROT_READ | PROT_WRITE | PROT_EXEC};
            if (action == CodeBlockProtectionStatus::Enable)
                base &= ~PROT_WRITE;
            else
                base &= ~PROT_EXEC;
            return base;
        }();

        if (!end)
            end = exec + size;
        assert(mprotect(begin, end - begin, flags) == 0);
    }

    void CodeBlocks::Expand(const u64 pages) {
        const auto total{pages * sysconf(_SC_PAGE_SIZE)};
        if (size > total)
            return;

        constexpr auto flags{PROT_NONE};
        if (!exec)
            exec = static_cast<u8*>(mmap(nullptr, total, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        else
            exec = static_cast<u8*>(mremap(exec, total, flags, MREMAP_FIXED));

        if (exec == MAP_FAILED) {
            status = CodeBlockStatus::MemoryMappingFailed;
            return;
        }
        size = total;
    }
}

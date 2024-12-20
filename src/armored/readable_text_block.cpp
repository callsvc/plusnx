#include <sys/mman.h>

#include <armored/backend/x86_64_emitter_context.h>
#include <armored/readable_text_block.h>
namespace Plusnx::Armored {
    ReadableTextBlock::ReadableTextBlock(const std::shared_ptr<Backend::EmitterGenerator>& generator) : emitter(generator) {}

    void ReadableTextBlock::Initialize(const u64 code) {
        if (code) {
            Expand(code / sysconf(_SC_PAGE_SIZE));
        }

        {
            Protect(text, &text[code], TextProtectionStatus::Disable);
            const u64 instructions{code / emitter->backing->GetInstructionSize(true)};
            for (u64 slot{}; slot < instructions; slot++) {
                emitter->EmitNop();
            }
            Protect(text, &text[code], TextProtectionStatus::Enable);
        }
    }

    ReadableTextBlock::~ReadableTextBlock() {
        emitter->blocks.reset();
        emitter->mapping = {};
        assert(munmap(text, size) == 0);
    }

    void ReadableTextBlock::Protect(u8* begin, const u8* end, const TextProtectionStatus action) const {
        const auto flags = [&] {
            auto base{PROT_READ | PROT_WRITE | PROT_EXEC};
            if (action == TextProtectionStatus::Enable)
                base &= ~PROT_WRITE;
            else
                base &= ~PROT_EXEC;
            return base;
        }();

        if (!end)
            end = text + size;
        assert(mprotect(begin, end - begin, flags) == 0);
    }

    void ReadableTextBlock::Expand(const u64 pages) {
        const auto total{pages * sysconf(_SC_PAGE_SIZE)};
        if (size > total)
            return;

        constexpr auto flags{PROT_NONE};
        if (!text)
            text = static_cast<u8*>(mmap(nullptr, total, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        else
            text = static_cast<u8*>(mremap(text, total, flags, MREMAP_FIXED));

        if (text == MAP_FAILED) {
            status = TextStatus::MemoryMappingFailed;
            return;
        }
        size = total;
    }
}

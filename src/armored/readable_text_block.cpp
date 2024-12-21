#include <sys/mman.h>

#include <boost/align/align_up.hpp>
#include <armored/backend/emitter_generator.h>
#include <armored/readable_text_block.h>

namespace Plusnx::Armored {
    ReadableTextBlock::ReadableTextBlock(const std::shared_ptr<Backend::EmitterGenerator>& _emitter) : emitter(_emitter) {}
    void ReadableTextBlock::Initialize(const u64 code) {
        Expand(code ? code : 10 * 4 * 1024);
        Protect(text, &text[code], TextProtectionStatus::Disable);

        const u64 instructions{code / emitter->backing->GetInstructionSize(true)};

        if (const auto context{emitter->GetThreadCtx()}; !context->next) {
            context->next = text;
        }
        for (u64 slot{}; slot < instructions; slot += 4) {
            emitter->EmitNop(); emitter->EmitNop(); emitter->EmitNop(); emitter->EmitNop();
        }
        emitter->ResetBuffer();

        Protect(text, &text[code], TextProtectionStatus::Enable);
    }

    ReadableTextBlock::~ReadableTextBlock() {
        for (const auto& [identifier, thrContext] : emitter->cpus) {
            if (thrContext->segment == shared_from_this()) {
                emitter->cpus.erase(identifier);
            }
        }
        assert(munmap(text, size) == 0);
    }

    void ReadableTextBlock::Protect(u8* begin, const u8* end, const TextProtectionStatus action) const {
        const auto flags = [&] {
            constexpr auto base{PROT_READ | PROT_WRITE | PROT_EXEC};
            if (action == TextProtectionStatus::Enable)
                return base & ~PROT_WRITE;
            return base & ~PROT_EXEC;
        }();

        if (!end) {
            end = text + size;
        }
        assert(mprotect(begin, end - begin, flags) == 0);
    }
    void ReadableTextBlock::Protect(const TextProtectionStatus prot) const {
        Protect(text, text + size, prot);
    }

    void ReadableTextBlock::Expand(u64 expansion) {
        if (!expansion) {
            expansion = size * 2;
            if (!expansion)
                return;
        }
        expansion = boost::alignment::align_up(expansion, 4096);

        const auto* last{text};
        if (!text) {
            if (auto* result{static_cast<u8*>(mmap(nullptr, expansion, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0))}; result != MAP_FAILED)
                text = result;
        } else {
            if (size > expansion)
                assert(0);
            if (auto* result{static_cast<u8*>(mremap(text, size, expansion, MREMAP_MAYMOVE))}; result != MAP_FAILED) {
                if (result == last)
                    last = nullptr;

                text = result;
            } else {
                throw exception("Failed to expand text, {}", GetOsErrorString());
            }
        }

        if (last == text || text == MAP_FAILED) {
            status = TextStatus::MemoryMappingFailed;
            return;
        }
        size = expansion;
    }
}

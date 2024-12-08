#include <armored/backend/emitter_generator.h>

#include <armored/code_blocks.h>
namespace Plusnx::Armored::Backend {
    void EmitterGenerator::PushCpuContext(CpuContext& core, const std::shared_ptr<CodeBlocks>& blocks, const u64 size) {
        cpus.emplace_back(core, blocks);

        contexts.emplace(codeMap, blockMap);

        blockMap = cpus.back().second;
        codeMap = {};

        blocks->Initialize(size);
        if (const auto it{contexts.find(codeMap)}; it != contexts.end()) {
            blockMap = it->second;
            codeMap = it->first;
            contexts.erase(it);
        }
    }

    void EmitterGenerator::PopCpuContext(const CpuContext& core) {
        auto it{cpus.begin()};
        for (; it != cpus.end(); ++it) {
            if (it == cpus.end()) {
                return;
            }
            if (it->first.ccid == core.ccid)
                break;
        }

        codeMap = [&] -> u64 {
            for (const auto& [map, block] : contexts) {
                if (block == it->second)
                    codeMap = map;
            }
            return {};
        }();
        blockMap = it->second;
        cpus.erase(it);
    }

    void EmitterGenerator::WriteI(const std::span<u8>& instruction) {
        auto image = [&] -> u8* {
            if (const std::shared_ptr cbs{blockMap})
                return cbs->exec + codeMap;
            return nullptr;
        }();

        assert(instruction.size() == backing->GetInstructionSize(true));
        for (const auto part : instruction) {
            *image++ = part;
        }

        Advance();
    }

    void EmitterGenerator::Advance() {
        codeMap += backing->GetInstructionSize(true);
    }
}

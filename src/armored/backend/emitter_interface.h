#pragma once

#include <armored/ir/ir_constructor.h>
#include <armored/arm_types.h>
namespace Plusnx::Armored::Backend {
    class EmitterGenerator;

    class EmitterInterface {
    public:
        virtual ~EmitterInterface() = default;
        virtual u64 GetInstructionSize(bool fromHost = false) = 0;
        virtual void Translate(const u8* pc, u64 count) = 0;

        virtual std::list<std::unique_ptr<Ir::IrDescriptor>>& GetList() = 0;

        std::weak_ptr<EmitterGenerator> parent;
    };
}

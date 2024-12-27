#include <nxk/base/k_process_handle_table.h>
namespace Plusnx::Nxk::Base {
    constexpr auto InternalTableSize{1 * 1024};

    KProcessHandleTable::KProcessHandleTable(Kernel& _kernel, const u64 _size) : size(_size), kernel(_kernel) {
        if (size == 0)
            size = InternalTableSize;

        handles.resize(size);
        handleHead = handles.begin();
    }

    KProcessHandleTable::~KProcessHandleTable() {
        for (auto& [handle, _] : handles) {
            Destroy(handle);
        }
    }

    void KProcessHandleTable::Destroy(const u16 handle) {
        auto it{handles.begin()};
        for (; it != handles.end(); ++it) {
            if (it->handle == handle)
                break;
        }
        if (it != handles.end() && it->handle) {
            it->object->DecrementReference();

            {
                const auto destroyIt{std::move(it->object)};
            }
            it->handle = {};
            if (handleHead != handles.end())
                freeList.emplace_back(handleHead);
            handleHead = it;
        }
    }
}

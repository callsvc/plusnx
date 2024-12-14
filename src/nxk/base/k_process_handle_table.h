#pragma once

#include <list>
#include <memory>

#include <nxk/types/kthread.h>
namespace Plusnx::Nxk::Base {
    struct KHandleEntry {
        u16 handle;
        std::shared_ptr<KAutoObject> object;
    };
    class KProcessHandleTable {
    public:
        KProcessHandleTable(Kernel& _kernel, u64 _size = 0);
        ~KProcessHandleTable();

        template <typename T>
        std::shared_ptr<T> Create(u16& handle) {
            handle = counter++;
            if (!counter)
                return {};

            if (handle == size)
                return {};

            if (handleHead->handle) {
                if (freeList.empty())
                    return {};
                handleHead = freeList.back();
                freeList.pop_back();
                assert(handleHead->handle == 0);
            }

            handles.emplace(handleHead, handle, std::make_shared<T>(kernel));
            ++handleHead;

            return GetHandle<T>(handle);
        }
        template <typename T>
        std::shared_ptr<T> GetHandle(const u16 handle) {
            auto entry{handles.begin()};
            for (; entry != handles.end(); ++entry) {
                if (entry->handle != handle)
                    continue;

                const auto validObject = [&] -> bool {
                    if constexpr (std::is_same_v<T, Types::KThread>)
                        if (entry->object->type == KAutoType::KThread)
                            return true;
                    assert(0);
                }();
                if (validObject) {
                    entry->object->IncrementReference();
                    return std::dynamic_pointer_cast<T>(entry->object);
                }
                return {};
            }
            return {};
        }

        auto GetThread(const u16 handle) {
            return GetHandle<Types::KThread>(handle);
        }

        void Destroy(u16 handle);

        u64 size;
        Kernel& kernel;
    private:
        std::list<KHandleEntry> handles;
        decltype(handles)::iterator handleHead;
        std::list<decltype(handleHead)> freeList;

        u16 counter{1};
        u16 activeSlots{};

        std::mutex lock;
    };
}
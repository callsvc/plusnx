#include <cxxabi.h>
#include <execinfo.h>

#include <ranges>
#include <runtime_exception.h>
#include <types.h>
namespace Plusnx {
    inline void GetCallStack(std::vector<void*>& tracer, std::map<void*, std::string>& symbols) {
        // Android lacks a native function to read the call stack, so we need to implement it ourselves
        tracer.resize(backtrace(tracer.data(), tracer.size()));
        const auto strings{backtrace_symbols(tracer.data(), tracer.size())};
        if (!strings)
            return;

        for (u64 trace{}; trace < tracer.size() && strings[trace]; trace++) {
            symbols.emplace(tracer[trace], strings[trace]);
        }
        free(strings);
    }

    inline void DemangleFunctionName(const std::string_view& name, std::string& demangle, u64& length) {
        __cxxabiv1::__cxa_demangle(name.data(), nullptr, &length, nullptr);
        if (demangle.capacity() < length)
            demangle.reserve(length);

        demangle.resize(length);
        __cxxabiv1::__cxa_demangle(name.data(), demangle.data(), &length, nullptr);
        demangle.resize(std::strlen(demangle.data()));
    }

    constexpr auto SymbolNameSize{0x50};
    void runtime_exception::PrintPrettyMessage() const {
        for (const auto& function : GetStackTrace()) {
            std::print("{}\n", function);
        }
        std::print("Call stack dump completed successfully\n");
#if PLUSNX_GUI_SDL2
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "An exception was thrown", runtime_error::what(), nullptr);
#endif
    }

    std::vector<std::string> runtime_exception::GetStackTrace() {
        std::vector<std::string> result;

        std::vector<void*> tracer(32);
        std::map<void*, std::string> symbols;
        GetCallStack(tracer, symbols);

        std::string demangle;
        u64 count{};
        for (auto** symbol{tracer.data()}; *symbol != nullptr; symbol++) {
            if (!symbols.contains(*symbol)) {
                result.emplace_back(std::format("(): {}", *symbol));
                continue;
            }
            auto function{symbols.find(*symbol)->second};
            u64 length{};
            {
                const auto content{function.begin() + function.find_first_of('(') + 1};
                const auto addrPrefix{function.find_last_of('+')};
                function[addrPrefix] = '\0';

                DemangleFunctionName(&*content, demangle, length);
                function[addrPrefix] = '+';
            }

            if (length) {
                if (length > SymbolNameSize)
                    length = SymbolNameSize;
                if (demangle.size() < length)
                    length = demangle.size();

                function.replace(0, length, demangle);
                if (demangle.size() > length)
                    function.replace(length - 3, 3, "...");

                const auto final{function.substr(function.find_last_of('+'))};
                function.replace(length, final.size(), final);

                if (function.size() > length + final.size())
                    function.erase(length + final.size());
                function.insert(0, "(");
            }

            result.emplace_back(std::format("{:02}: {}", count++, std::move(function)));
        }

        return result;
    }
}
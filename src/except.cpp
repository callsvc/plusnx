#include <cxxabi.h>
#include <execinfo.h>

#include <except.h>
#include <ranges>
#include <types.h>
namespace Plusnx {
    void GetCallStack(std::vector<void*>& tracer, std::map<void*, std::string>& symbols) {
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

    constexpr auto SymbolNameSize{0x34};
    std::vector<std::string> Except::GetStackTrace() {
        std::vector<std::string> result;

        std::vector<void*> tracer(32);
        std::map<void*, std::string> symbols;
        GetCallStack(tracer, symbols);

        std::string demangle;
        for (auto** symbol{tracer.data()}; *symbol != nullptr; symbol++) {
            if (!symbols.contains(*symbol)) {
                result.emplace_back(std::format("(): {}", *symbol));
                continue;
            }
            auto& function{symbols.find(*symbol)->second};
            const auto begin{function.begin() + function.find_first_of('(') + 1};
            const auto end{function.begin() + function.find_last_of('+')};

            u64 length{};
            *end = '\0';
            __cxxabiv1::__cxa_demangle(&*begin, nullptr, &length, nullptr);

            if (demangle.size() < length)
                demangle.reserve(length);
            demangle.resize(length);
            __cxxabiv1::__cxa_demangle(&*begin, demangle.data(), &length, nullptr);
            *end = '+';

            if (length) {
                if (length > SymbolNameSize)
                    length = SymbolNameSize;
                function.replace(0, length - 3, demangle);
                function.replace(length - 3, 3, "...");

                const auto total{std::strlen(&*end)};
                function.replace(length, total, &*end);

                function.insert(0, "(");
                if (function.size() > length + total + 1)
                    function.erase(length + total + 1);
                result.emplace_back(function);
            }
        }

        return result;
    }
}
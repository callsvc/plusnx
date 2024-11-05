#include <boost/endian/conversion.hpp>

#include <security/key_types.h>
namespace Plusnx::Security {
    // https://gist.github.com/SciresM/fe8a631d13c069bd66e9c656ab5b3f7f
    std::array<u8, 16> GetSwitchCounter(u64 counter) {
        std::array<u8, 16> result{};
        boost::endian::native_to_big_inplace(counter);
        std::memcpy(&result[8], &counter, sizeof(counter));

        return result;
    }
}
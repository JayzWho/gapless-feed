#include "gapless/build_info.h"

#include "build_config.h"

namespace gapless {

BuildInfo build_info() noexcept {
    return {
        .version = GAPLESS_VERSION,
        .compiler = GAPLESS_COMPILER,
        .build_type = GAPLESS_BUILD_TYPE,
        .cxx_flags = GAPLESS_CXX_FLAGS,
        .sanitizers = GAPLESS_SANITIZERS,
#ifdef NDEBUG
        .assertions = false,
#else
        .assertions = true,
#endif
    };
}

} // namespace gapless

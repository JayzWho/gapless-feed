#pragma once

#include <string_view>

namespace gapless {

/**
 * How this binary was built. Measurement results record it, so that a number can be traced
 * back to the compiler and options that produced it.
 */
struct BuildInfo {
    std::string_view version;    ///< Project version, e.g. "0.1.0".
    std::string_view compiler;   ///< Compiler and its version, e.g. "GNU 15.2.0".
    std::string_view build_type; ///< CMake build type, e.g. "Release".
    std::string_view cxx_flags;  ///< CMAKE_CXX_FLAGS plus the flags of the build type.
    std::string_view sanitizers; ///< Enabled sanitizers, or "none".
    bool assertions;             ///< Whether assert() is compiled in.
};

[[nodiscard]] BuildInfo build_info() noexcept;

} // namespace gapless

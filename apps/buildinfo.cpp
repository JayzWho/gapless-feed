// Prints how this build was configured: version, compiler, options, sanitizers.

#include "gapless/build_info.h"

#include <print>

int main() {
    const gapless::BuildInfo info = gapless::build_info();
    std::println("gapless-feed {}", info.version);
    std::println("compiler:   {}", info.compiler);
    std::println("build type: {}", info.build_type);
    std::println("cxx flags:  {}", info.cxx_flags);
    std::println("sanitizers: {}", info.sanitizers);
    std::println("assertions: {}", info.assertions ? "on" : "off");
}

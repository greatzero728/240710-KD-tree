#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "Math.hpp"
#include "Shapes.hpp"

namespace glm {
    std::istream& operator>>(std::istream& is, ivec2& v);
    std::ostream& operator<<(std::ostream& os, ivec2 const& p);
    std::istream& operator>>(std::istream& is, vec2& v);
    std::ostream& operator<<(std::ostream& os, vec2 const& p);
    std::istream& operator>>(std::istream& is, vec3& v);
    std::ostream& operator<<(std::ostream& os, vec3 const& p);
    std::istream& operator>>(std::istream& is, vec4& v);
    std::ostream& operator<<(std::ostream& os, vec4 const& p);
    std::istream& operator>>(std::istream& is, mat3& v);
    std::ostream& operator<<(std::ostream& os, mat3 const& v);
    std::istream& operator>>(std::istream& is, mat4& v);
    std::ostream& operator<<(std::ostream& os, mat4 const& v);
}

namespace CS350 {    
    std::istream& operator>>(std::istream& is, Sphere& v);
    std::ostream& operator<<(std::ostream& os, Sphere const& sphere);
    std::istream& operator>>(std::istream& is, Aabb& v);
    std::ostream& operator<<(std::ostream& os, Aabb const& aabb);
}

#if FMT_VERSION >= 90000
// Fix for windows
template <> struct fmt::formatter<vec2> : ostream_formatter {};
template <> struct fmt::formatter<vec3> : ostream_formatter {};
template <> struct fmt::formatter<vec4> : ostream_formatter {};
template <> struct fmt::formatter<CS350::Sphere> : ostream_formatter {};
template <> struct fmt::formatter<CS350::Aabb> : ostream_formatter {};
#endif

#include <stdexcept>

#endif // LOGGING_HPP

#include <format>
#include <string>
#include <iostream>
#include <xmmintrin.h>

struct alignas(1) rgba {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    auto to_vec4() const -> std::string {
        __m128 vector = _mm_setr_ps(r, g, b, a);
        vector = _mm_div_ps(vector, _mm_set1_ps(std::numeric_limits<std::uint8_t>::max()));
        const auto& [x, y, z, w] = vector.m128_f32;
        return std::format("vec4({:.5f}, {:.5f}, {:.5f}, {:.5f})", x, y, z, w);
    }
};

auto operator<<(std::ostream& os, const rgba& color) -> std::ostream& {
    return os << std::format("rgb(r = {:#x}, b = {:#x}, a = {:#x})", color.r, color.g, color.b);
}

auto from_color_code(const std::string& code) -> rgba {
    const auto color = std::stoul(code, nullptr, 16);
    return {
        static_cast<std::uint8_t>((color >> 16) & 0xFF),
        static_cast<std::uint8_t>((color >> 8) & 0xFF),
        static_cast<std::uint8_t>(color & 0xFF),
    };
}

auto main() -> int {
    std::string input;
    
    while (true) {
        std::cout << "Enter hex color string: ";
        std::getline(std::cin, input);

        if (input == "quit")
            break;

        std::cout << from_color_code(input).to_vec4() << '\n';
    }
}

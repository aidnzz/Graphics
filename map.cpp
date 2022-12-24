#include <array>
#include <span>
#include <fstream>
#include <ranges>
#include <algorithm>

using rgba = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>;
using position = std::pair<std::size_t, std::size_t>;

constexpr auto pack(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a = 255) noexcept -> std::uint32_t {
    return (a << 24) | (b << 16) | (g << 8) | r;
}

constexpr auto unpack(const std::uint32_t color) noexcept -> rgba {
    return { color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, (color >> 24) & 0xFF };
}

template<std::size_t W, std::size_t H>
auto save_ppm_image(const char* const path, const std::span<const std::uint32_t, W * H> buffer) -> void {
    std::ofstream fs(path, std::ostream::binary);

    fs << "P6\n" << W << ' ' << H << '\n' << "255" << '\n';

    for (const auto n : buffer) {
        const auto [r, g, b, a] = unpack(n);
        fs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
}

template<std::size_t W, std::size_t H>
auto draw_square(const std::span<std::uint32_t, W * H> data, const position& pos, const std::size_t size, const std::uint32_t color) noexcept -> position {
    const auto [x, y] = pos;

    const auto row_end = x + size;
    const auto column_end = y + size;

    for (const auto i : std::views::iota(y, column_end)) {
        for (const auto j : std::views::iota(x, row_end)) {
            data[j + i * W] = color;
        }
    }

    return { row_end % W, y + size * (row_end / W) };
}

auto main() -> int {
    using namespace std::ranges;

    constexpr std::size_t width = 512;
    constexpr std::size_t height = 512;
    
    constexpr auto pink = pack(255, 192, 203);
    constexpr auto black = pack(0, 0, 0);

    auto buffer = std::make_unique<std::uint32_t[]>(width * height);
    const auto view = std::span<std::uint32_t, width * height>(buffer.get(), width * height);
    
    fill(view, pink);

    constexpr std::array<char[17], 16> map = {
        "################",
        "#      #       #",
        "#===#  #       #",
        "#   #  #       #",
        "#   #  #       #",
        "#   #  #=======#",
        "#   #          #",
        "#   #=======#  #",
        "#           #  #",
        "#           #  #",
        "#           #  #",
        "#           #  #",
        "#           #  #",
        "#           #  #",
        "#           #  #",
        "################",
    };

    constexpr std::size_t square_size = 32;

    auto position = std::make_pair(0, 0);
    
    for (const auto& row : map) {
        for (const auto c : views::take(row, 16)) {
            const auto background = c == ' ' ? pink : black;
            position = draw_square<width, height>(view, position, square_size, background);
        }
    }
    
    save_ppm_image<width, height>("test.ppm", view);
}

#include <optional>
#include <ranges>
#include <iostream>
#include <charconv>
#include <string_view>

std::optional<int32_t> to_int(std::string_view sv)
{
    int r{};
    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), r)};
    if (ec == std::errc())
        return r;
    else
        return std::nullopt;
}

auto to_string(int32_t x) -> std::string { return std::to_string(x); };

std::ostream& operator<<(std::ostream& os, const std::optional<int32_t>& opt)
{
    os << opt.transform( to_string ).value_or("-");
    return os;
}

std::istream& operator>> (std::istream& is, std::optional<int32_t>& optInt)
{
    std::string str;
    is >> str; 
    optInt = to_int(str);
    return is;
}

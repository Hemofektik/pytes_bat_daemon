#pragma once

#include <optional>
#include <ranges>
#include <iostream>
#include <charconv>
#include <string_view>

std::optional<int32_t> to_int(std::string_view sv)
{
    int32_t result{};
    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), result)};
    if (ec == std::errc())
    {
        return result;
    }
    
    return std::nullopt;
}


std::istream& operator>> (std::istream& is, std::optional<int32_t>& optInt)
{
    std::string str;
    is >> str; 
    optInt = to_int(str);
    return is;
}

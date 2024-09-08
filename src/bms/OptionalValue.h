#pragma once

#include <optional>
#include <iostream>
#include <charconv>
#include <string_view>

namespace pytes::bms
{

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

std::istream& operator>> (std::istream& is, std::optional<std::string>& optStr)
{
    std::string str;
    is >> str; 
    if(str == "-")
    {
        optStr.reset();
    }
    else
    {
        optStr.emplace(str);
    }
    return is;
}


}
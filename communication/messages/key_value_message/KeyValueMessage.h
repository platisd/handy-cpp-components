#pragma once

struct KeyValueMessage
{
    KeyValueMessage(const std::string& k, const std::string& v)
        : key{k}
        , value{v}
    {
    }

    const std::string key;
    const std::string value;
};

#pragma once

struct KeyValueMessage
{
    KeyValueMessage(const std::string& k, const std::string& v)
        : key{k}
        , value{v}
    {
    }

    inline bool operator==(const KeyValueMessage& other) const
    {
        return (key == other.key) && (value == other.value);
    }

    const std::string key;
    const std::string value;
};

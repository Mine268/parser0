#pragma once

#include <vector>
#include <algorithm>
#include <string>

namespace lexer0 {

    class bit_flagger {
    private:
        std::vector<bool> bit_storage;

    public:
        bit_flagger(std::size_t n, bool v);
        void set(std::size_t ix, bool v);
        bool get(std::size_t ix);
        std::string to_string();
        friend bool operator==(const bit_flagger& lhs, const bit_flagger& rhs);
    };

}

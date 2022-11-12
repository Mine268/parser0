#pragma once

#include <tuple>
#include <string>

namespace lexer0 {

    struct token {
        std::size_t token_id;
        std::size_t token_start;
        std::size_t token_length;
        std::string token_string;

        [[nodiscard]] std::string to_string() const;

        explicit operator std::tuple<std::size_t,std::size_t, std::size_t, std::string>() const;
    };

}
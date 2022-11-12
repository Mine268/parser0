#include "token.hpp"

std::string lexer0::token::to_string() const {
    return '[' + std::to_string(this->token_id) + ',' + std::to_string(this->token_start)
           + ',' + std::to_string(this->token_length)
           + ',' + this->token_string + ']';
}

lexer0::token::operator std::tuple<std::size_t, std::size_t, std::size_t, std::string>() const {
    return std::make_tuple(token_id, token_start, token_length, token_string);
}

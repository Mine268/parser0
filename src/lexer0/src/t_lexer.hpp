#pragma once

#include "t_reg_expr.hpp"
#include "token.hpp"

namespace lexer0 {

    template<typename... Regs>
    class t_lexer {
        static_assert(sizeof...(Regs) > 0, "More than zero regex-es should be designated.");
    private:
        std::vector<dfa> reg_vector;

    public:
        t_lexer();

        std::vector<token> lexer(const std::string& sv);

        std::string to_string();
    };

    template<typename... Regs>
    t_lexer<Regs...>::t_lexer() {
        (reg_vector.push_back(t_get_nfa<Regs>().get_dfa().get_optimize()), ..., 0);
    }

    template<typename... Regs>
    std::string t_lexer<Regs...>::to_string() {
        return ((Regs::to_string() + '\n') + ...);
    }

    template<typename... Regs>
    std::vector<token> t_lexer<Regs...>::lexer(const std::string& sv) {
        std::size_t start_ix{0}, curr_ix{0};
        std::vector<token> token_stream;

        while (curr_ix < sv.size()) {
            bool reg_match = false;
            bool all_trap;
            std::size_t recent_match_reg = 0;
            std::size_t recent_match_ix = 0;
            do {
                all_trap = true;
                for (std::size_t r_reg_ix = 0; r_reg_ix < reg_vector.size(); ++r_reg_ix) {
                    auto [acc, trap] = reg_vector.at(reg_vector.size() - r_reg_ix - 1).trans_on(sv.at(curr_ix));
                    reg_match |= acc;
                    all_trap &= trap;
                    if (acc) {
                        recent_match_reg = reg_vector.size() - r_reg_ix - 1;
                        recent_match_ix = curr_ix;
                    }
                }
                ++curr_ix;
            } while (!all_trap && curr_ix < sv.size());

            if (reg_match) {
                token_stream.push_back(
                        token{recent_match_reg,
                              start_ix,
                              recent_match_ix - start_ix + 1,
                              sv.substr(start_ix, recent_match_ix - start_ix + 1)});
                start_ix = curr_ix = recent_match_ix + 1;
                for (auto &dfa : reg_vector) {
                    dfa.reset();
                }
            } else {
                break;
            }
        }

        return token_stream;
    }

}
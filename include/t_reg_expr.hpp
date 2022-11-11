#pragma once

#include <functional>

#include "nfa.hpp"

namespace lexer0 {

    template<int Termination>
    class t_terminate_expr {
    public:
        static constexpr std::size_t get_size();

        static void create_nfa(nfa &, status_type zero_status);

        static std::string to_string();
    };

    template<int Termination>
    constexpr std::size_t t_terminate_expr<Termination>::get_size() {
        return 2;
    }

    template<int Termination>
    void t_terminate_expr<Termination>::create_nfa(lexer0::nfa &fa, lexer0::status_type zero_status) {
        fa.add_trans(zero_status, zero_status + 1, Termination);
    }

    template<int Termination>
    std::string t_terminate_expr<Termination>::to_string() {
        return std::to_string(Termination);
    }

    template<typename Regex>
    class t_repeat_expr {
        using Check = decltype((Regex::get_size, Regex::create_nfa, int{}));
    public:
        static constexpr std::size_t get_size();

        static void create_nfa(nfa &, status_type zero_status);

        static std::string to_string();
    };

    template<typename Regex>
    constexpr std::size_t t_repeat_expr<Regex>::get_size() {
        return Regex::get_size() + 2;
    }

    template<typename Regex>
    void t_repeat_expr<Regex>::create_nfa(lexer0::nfa &fa, lexer0::status_type zero_status) {
        std::size_t zero_r{zero_status + 1}, acc_r{zero_r + Regex::get_size() - 1}, acc{acc_r + 1};
        Regex::create_nfa(fa, zero_r);
        fa.add_trans(zero_status, zero_r);
        fa.add_trans(zero_r, acc);
        fa.add_trans(acc_r, acc);
        fa.add_trans(acc, zero_r);
    }

    template<typename Regex>
    std::string t_repeat_expr<Regex>::to_string() {
        return '(' + Regex::to_string() + ")*";
    }

    template<typename... Regex>
    class t_cat_expr {
        static_assert(sizeof...(Regex) >= 2, "Provide more than 1 regex.");
    public:
        static constexpr std::size_t get_size();

        static void create_nfa(nfa &, status_type zero_status);

        static std::string to_string();

    private:
        template<typename FReg, typename... Regs>
        static void create_nfa_recur(nfa &, status_type zero_status);
    };

    template<typename... Regex>
    constexpr std::size_t t_cat_expr<Regex...>::get_size() {
        return (Regex::get_size() + ... + 1) - sizeof...(Regex);
    }

    template<typename... Regex>
    void t_cat_expr<Regex...>::create_nfa(nfa &fa, status_type zero_status) {
        create_nfa_recur<Regex...>(fa, zero_status);
    }

    template<typename... Regex>
    template<typename FReg, typename... Regs>
    void t_cat_expr<Regex...>::create_nfa_recur(nfa &fa, status_type zero_status) {
        status_type acc_status = zero_status + FReg::get_size() - 1;
        FReg::create_nfa(fa, zero_status);
        if constexpr (sizeof...(Regs)) {
            create_nfa_recur<Regs...>(fa, acc_status);
        }
    }

    template<typename... Regex>
    std::string t_cat_expr<Regex...>::to_string() {
        return (Regex::to_string() + ... + "");
    }

    template<typename... Regex>
    class t_or_expr {
        static_assert(sizeof...(Regex) >= 2, "Provide more than 1 regex.");
    public:
        static constexpr std::size_t get_size();

        static void create_nfa(nfa &, status_type zero_status);

        static std::string to_string();

    private:
        template<typename FReg, typename... Regs>
        static void create_nfa_recur(nfa &,
                                     status_type zero_status,
                                     status_type or_zero_status,
                                     status_type or_acc_status);
    };

    template<typename... Regex>
    constexpr std::size_t t_or_expr<Regex...>::get_size() {
        return (Regex::get_size() + ... + 2);
    }

    template<typename... Regex>
    void t_or_expr<Regex...>::create_nfa(nfa &fa, status_type zero_status) {
        status_type or_zero_status, or_acc_status;
        or_zero_status = zero_status;
        or_acc_status = get_size() + or_zero_status - 1;
        create_nfa_recur<Regex...>(fa,
                                   zero_status + 1,
                                   or_zero_status,
                                   or_acc_status);
    }

    template<typename... Regex>
    template<typename FReg, typename... Regs>
    void t_or_expr<Regex...>::create_nfa_recur(nfa &fa,
                                               status_type zero_status,
                                               status_type or_zero_status,
                                               status_type or_acc_status) {
        status_type acc_status = zero_status + FReg::get_size() - 1;
        FReg::create_nfa(fa, zero_status);
        fa.add_trans(or_zero_status, zero_status);
        fa.add_trans(acc_status, or_acc_status);
        if constexpr (sizeof...(Regs)) {
            create_nfa_recur<Regs...>(fa,
                                      acc_status + 1,
                                      or_zero_status,
                                      or_acc_status);
        }
    }

    template<typename... Regex>
    std::string t_or_expr<Regex...>::to_string() {
        std::string ret = ("" + ... + ('|' + Regex::to_string()));
        ret = ret.substr(1);
        return '(' + ret + ')';
    }

    template<typename Regex>
    class t_exist_not_expr {
    public:
        using Check = decltype((Regex::get_size, Regex::create_nfa, int{}));
    public:
        static constexpr std::size_t get_size();

        static void create_nfa(nfa &, status_type zero_status);

        static std::string to_string();
    };

    template<typename Regex>
    constexpr std::size_t t_exist_not_expr<Regex>::get_size() {
        return Regex::get_size();
    }

    template<typename Regex>
    void t_exist_not_expr<Regex>::create_nfa(nfa &fa, status_type zero_status) {
        Regex::create_nfa(fa, zero_status);
        fa.add_trans(zero_status, zero_status + Regex::get_size() - 1);
    }

    template<typename Regex>
    std::string t_exist_not_expr<Regex>::to_string() {
        return '(' + Regex::to_string() + ")?";
    }

    template<typename Reg>
    nfa t_get_nfa() {
        nfa ret{Reg::get_size()};
        Reg::create_nfa(ret, 0);
        ret.add_accept(Reg::get_size() - 1);
        return ret;
    }

    template<typename Reg>
    using t_more_than_one = t_cat_expr<Reg, t_repeat_expr<Reg>>;

    using t_dec_digit_reg = t_or_expr<
            t_terminate_expr<'0'>,
            t_terminate_expr<'1'>,
            t_terminate_expr<'2'>,
            t_terminate_expr<'3'>,
            t_terminate_expr<'4'>,
            t_terminate_expr<'5'>,
            t_terminate_expr<'6'>,
            t_terminate_expr<'7'>,
            t_terminate_expr<'8'>,
            t_terminate_expr<'9'>>;
    using t_dec_digit_nonzero_reg = t_or_expr<
            t_terminate_expr<'1'>,
            t_terminate_expr<'2'>,
            t_terminate_expr<'3'>,
            t_terminate_expr<'4'>,
            t_terminate_expr<'5'>,
            t_terminate_expr<'6'>,
            t_terminate_expr<'7'>,
            t_terminate_expr<'8'>,
            t_terminate_expr<'9'>>;
    using t_oct_digit_reg = t_or_expr<
            t_terminate_expr<'0'>,
            t_terminate_expr<'1'>,
            t_terminate_expr<'2'>,
            t_terminate_expr<'3'>,
            t_terminate_expr<'4'>,
            t_terminate_expr<'5'>,
            t_terminate_expr<'6'>,
            t_terminate_expr<'7'>>;
    using t_hex_digit_reg = t_or_expr<
            t_terminate_expr<'0'>,
            t_terminate_expr<'1'>,
            t_terminate_expr<'2'>,
            t_terminate_expr<'3'>,
            t_terminate_expr<'4'>,
            t_terminate_expr<'5'>,
            t_terminate_expr<'6'>,
            t_terminate_expr<'7'>,
            t_terminate_expr<'8'>,
            t_terminate_expr<'9'>,
            t_terminate_expr<'a'>,
            t_terminate_expr<'b'>,
            t_terminate_expr<'c'>,
            t_terminate_expr<'d'>,
            t_terminate_expr<'e'>,
            t_terminate_expr<'f'>
    >;
    using t_alpha_reg = t_or_expr<
            t_terminate_expr<'a'>,
            t_terminate_expr<'b'>,
            t_terminate_expr<'c'>,
            t_terminate_expr<'d'>,
            t_terminate_expr<'e'>,
            t_terminate_expr<'f'>,
            t_terminate_expr<'g'>,
            t_terminate_expr<'h'>,
            t_terminate_expr<'i'>,
            t_terminate_expr<'j'>,
            t_terminate_expr<'k'>,
            t_terminate_expr<'l'>,
            t_terminate_expr<'m'>,
            t_terminate_expr<'n'>,
            t_terminate_expr<'o'>,
            t_terminate_expr<'p'>,
            t_terminate_expr<'q'>,
            t_terminate_expr<'r'>,
            t_terminate_expr<'s'>,
            t_terminate_expr<'t'>,
            t_terminate_expr<'u'>,
            t_terminate_expr<'v'>,
            t_terminate_expr<'w'>,
            t_terminate_expr<'x'>,
            t_terminate_expr<'y'>,
            t_terminate_expr<'z'>>;
    using t_Alpha_reg = t_or_expr<
            t_terminate_expr<'A'>,
            t_terminate_expr<'B'>,
            t_terminate_expr<'C'>,
            t_terminate_expr<'D'>,
            t_terminate_expr<'E'>,
            t_terminate_expr<'F'>,
            t_terminate_expr<'G'>,
            t_terminate_expr<'H'>,
            t_terminate_expr<'I'>,
            t_terminate_expr<'J'>,
            t_terminate_expr<'K'>,
            t_terminate_expr<'L'>,
            t_terminate_expr<'M'>,
            t_terminate_expr<'N'>,
            t_terminate_expr<'O'>,
            t_terminate_expr<'P'>,
            t_terminate_expr<'Q'>,
            t_terminate_expr<'R'>,
            t_terminate_expr<'S'>,
            t_terminate_expr<'T'>,
            t_terminate_expr<'U'>,
            t_terminate_expr<'V'>,
            t_terminate_expr<'W'>,
            t_terminate_expr<'X'>,
            t_terminate_expr<'Y'>,
            t_terminate_expr<'Z'>>;

    using t_c_identifier_reg = t_cat_expr<
            t_or_expr<t_alpha_reg, t_Alpha_reg, t_terminate_expr<'_'>>,
            t_repeat_expr<
                    t_or_expr<
                            t_dec_digit_reg,
                            t_alpha_reg,
                            t_Alpha_reg,
                            t_terminate_expr<'_'>>>
    >;

    using t_dec_integer_reg = t_or_expr<
            t_terminate_expr<'0'>,
            t_cat_expr<t_dec_digit_nonzero_reg,
                    t_repeat_expr<t_dec_digit_reg>>>;
    using t_oct_integer_reg = t_cat_expr<
            t_terminate_expr<'0'>,
            t_more_than_one<t_oct_digit_reg>>;
    using t_hex_integer_reg = t_cat_expr<
            t_terminate_expr<'0'>,
            t_or_expr<t_terminate_expr<'x'>, t_terminate_expr<'X'>>,
            t_more_than_one<t_hex_digit_reg>
    >;
    using t_integer_reg = t_or_expr<
            t_dec_integer_reg,
            t_oct_integer_reg,
            t_hex_integer_reg>;

    using t_left_float_prefix_reg = t_cat_expr<
            t_dec_integer_reg, t_terminate_expr<'.'>>;
    using t_right_float_prefix_reg = t_cat_expr<
            t_terminate_expr<'.'>, t_repeat_expr<t_dec_digit_reg>>;
    using t_both_float_prefix_reg = t_cat_expr<
            t_dec_integer_reg, t_right_float_prefix_reg>;
    using t_float_reg = t_cat_expr<
            t_or_expr<
                    t_left_float_prefix_reg,
                    t_right_float_prefix_reg,
                    t_both_float_prefix_reg,
                    t_dec_integer_reg>,
            t_exist_not_expr<
                    t_cat_expr<
                            t_terminate_expr<'e'>,
                            t_exist_not_expr<t_terminate_expr<'-'>>,
                            t_dec_integer_reg
                            >
                    >,
            t_exist_not_expr<t_or_expr<t_terminate_expr<'f'>, t_terminate_expr<'F'>>>
    >;

    using t_blank_reg = t_repeat_expr<t_or_expr<
            t_terminate_expr<' '>,
            t_terminate_expr<'\t'>,
            t_terminate_expr<'\v'>,
            t_terminate_expr<'\r'>,
            t_terminate_expr<'\n'>,
            t_terminate_expr<'\a'>,
            t_terminate_expr<'\b'>,
            t_terminate_expr<'\f'>
    >>;
}

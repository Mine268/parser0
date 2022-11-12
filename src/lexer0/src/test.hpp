#pragma once

#include "t_lexer.hpp"
#include "t_reg_expr.hpp"
#include "reg_expr.hpp"
#include "nfa.hpp"
#include <string>
#include <iostream>

using namespace lexer0;

template<typename>
void test_nfa();

template<typename>
void test_reg();

template<typename>
void test_nfa2dfa();

template<typename>
void test_dfa_optim1();

template<typename>
void test_dfa_optim2();

template<typename = void>
void test_t_reg();

template<typename = void>
void test_r_reg2();

template<typename = void>
[[noreturn]] void test_r_reg3();

template<typename = void>
void test_r_str();

template<typename = void>
void test_lexer();

template<typename>
void test_lexer() {
    t_lexer<
            t_terminate_expr<';'>,
            t_terminate_expr<':'>,
            t_terminate_expr<','>,
            t_terminate_expr<'='>,
            t_terminate_expr<'('>,
            t_terminate_expr<')'>,
            t_terminate_expr<'+'>,
            t_terminate_expr<'-'>,
            t_terminate_expr<'*'>,
            t_terminate_expr<'/'>,
            t_c_identifier_reg,
            t_float_reg,
            t_blank_reg
    > the_lexer;

    const std::string test_str[] = {";func(x1,x2)=x1+x2*x1+x2",
                                    ":f(1,g(223+koo(var1))*5.e3f)+52",
                                    "45e",
                                    "__foo",
                                    ":300e4f+.3f * (.0002f-15.f)",
                                    "var1+var2-var3*(var4/var5-45.23)",
                                    "-var"};

    for (auto &str: test_str) {
        std::cout << str << std::endl;
        auto ts = the_lexer.lexer(str);
        for (auto &t: ts) {
            std::cout << t.to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}

template<typename>
void test_r_str() {
    using REG = t_float_reg;
    std::cout << REG::to_string() << std::endl;
}

template<typename>
[[noreturn]] void test_r_reg3() {
    using REG = t_float_reg;

    auto fa = t_get_nfa<REG>();
//    std::cout << fa.to_string() << std::endl;
    auto tfa = fa.get_dfa();
//    std::cout << tfa.to_string() << std::endl;
    auto ofa = tfa.get_optimize();
    std::cout << ofa.to_string() << std::endl;
    while (true) {
        std::string str;
        std::getline(std::cin, str);
        for (auto c: str) {
            auto res = ofa.trans_on(c);
            std::cout << "after [" << c << "], ";
            if (std::get<0>(res)) {
                std::cout << "accept: ";
            } else if (std::get<1>(res)) {
                std::cout << "trap: ";
            } else {
                std::cout << "read: ";
            }
            std::cout << ofa.status_code() << std::endl;
        }
        ofa.reset();
    }
}

template<typename>
void test_r_reg2() {
    // abw*(p|s(k|zt)*)m
    using REG = t_cat_expr<
            t_terminate_expr<'a'>,
            t_terminate_expr<'b'>,
            t_repeat_expr<t_terminate_expr<'w'>>,
            t_or_expr<
                    t_terminate_expr<'p'>,
                    t_cat_expr<
                            t_terminate_expr<'s'>,
                            t_repeat_expr<
                                    t_or_expr<
                                            t_terminate_expr<'k'>,
                                            t_cat_expr<
                                                    t_terminate_expr<'z'>,
                                                    t_terminate_expr<'t'>
                                            >
                                    >
                            >
                    >
            >,
            t_terminate_expr<'m'>
    >;
    auto fa = t_get_nfa<REG>();
    std::cout << fa.to_string() << std::endl;

    auto tfa = fa.get_dfa();
    std::cout << tfa.to_string() << std::endl;
    auto ofa = tfa.get_optimize();
    std::cout << ofa.to_string() << std::endl;

    std::string str;
    std::cin >> str;
    for (auto c: str) {
        auto res = ofa.trans_on(c);
        std::cout << "after [" << c << "], ";
        if (std::get<0>(res)) {
            std::cout << "accept: ";
        } else if (std::get<1>(res)) {
            std::cout << "trap: ";
        } else {
            std::cout << "read: ";
        }
        std::cout << ofa.status_code() << std::endl;
    }
}

template<typename>
void test_t_reg() {
    // a|b*|(ef)*|c
    using REG =
            t_or_expr<
                    t_terminate_expr<'a'>,
                    t_repeat_expr<t_terminate_expr<'b'>>,
                    t_repeat_expr<t_cat_expr<
                            t_terminate_expr<'e'>,
                            t_terminate_expr<'f'>
                    >>,
                    t_terminate_expr<'c'>
            >;
    nfa fa{REG::get_size()};
    REG::create_nfa(fa, 0);
    std::cout << fa.to_string() << std::endl;
}

template<typename>
void test_dfa_optim2() {
    // ab(xy)*(w|zt)*k
    reg_expr *r =
            cat_expr::get_cat(
                    new terminate_expr{'a'},
                    new terminate_expr{'b'},
                    new repeat_expr{
                            new cat_expr{
                                    new terminate_expr{'x'},
                                    new terminate_expr{'y'}
                            }
                    },
                    new repeat_expr{
                            or_expr::get_or(
                                    new terminate_expr{'w'},
                                    new cat_expr{
                                            new terminate_expr{'z'},
                                            new terminate_expr{'t'}
                                    }
                            )
                    },
                    new terminate_expr{'k'}
            );
    auto fa = reg_expr::get_nfa(r).get_dfa();
    delete r;

    auto ofa = fa.get_optimize();
    std::cout << ofa.to_string() << std::endl;

    std::string str;
    std::cin >> str;
    for (auto c: str) {
        auto res = ofa.trans_on(c);
        std::cout << "after [" << c << "], ";
        if (std::get<0>(res)) {
            std::cout << "accept: ";
        } else if (std::get<1>(res)) {
            std::cout << "trap: ";
        } else {
            std::cout << "read: ";
        }
        std::cout << ofa.status_code() << std::endl;
    }
}

template<typename>
void test_dfa_optim1() {
    dfa f{5};

    f.add_trans(0, 1, 'a');
    f.add_trans(0, 0, 'b');
    f.add_trans(1, 3, 'a');
    f.add_trans(1, 2, 'b');
    f.add_trans(2, 4, 'a');
    f.add_trans(2, 4, 'b');
    f.add_trans(3, 1, 'a');
    f.add_trans(3, 2, 'b');
    f.add_trans(4, 3, 'a');
    f.add_trans(4, 2, 'b');
    f.add_accept(1);
    f.add_accept(3);

    auto of = f.get_optimize();

    std::cout << of.to_string() << std::endl;
}

template<typename>
void test_nfa2dfa() {
    // ab(xy)*(w|zt)*k
    reg_expr *r =
            cat_expr::get_cat(
                    new terminate_expr{'a'},
                    new terminate_expr{'b'},
                    new repeat_expr{
                            new cat_expr{
                                    new terminate_expr{'x'},
                                    new terminate_expr{'y'}
                            }
                    },
                    new repeat_expr{
                            or_expr::get_or(
                                    new terminate_expr{'w'},
                                    new cat_expr{
                                            new terminate_expr{'z'},
                                            new terminate_expr{'t'}
                                    }
                            )
                    },
                    new terminate_expr{'k'}
            );
    auto fa = reg_expr::get_nfa(r).get_dfa();
    std::cout << fa.to_string() << std::endl;
    delete r;

    std::string str;
    std::cin >> str;
    for (auto c: str) {
        auto res = fa.trans_on(c);
        std::cout << "after [" << c << "], ";
        if (std::get<0>(res)) {
            std::cout << "accept: ";
        } else if (std::get<1>(res)) {
            std::cout << "trap: ";
        } else {
            std::cout << "read: ";
        }
        std::cout << fa.status_code() << std::endl;
    }
}

template<typename = void>
void test_reg() {
    // ab(xy)*(w|zt)*k
    reg_expr *r =
            cat_expr::get_cat(
                    new terminate_expr{'a'},
                    new terminate_expr{'b'},
                    new repeat_expr{
                            new cat_expr{
                                    new terminate_expr{'x'},
                                    new terminate_expr{'y'}
                            }
                    },
                    new repeat_expr{
                            or_expr::get_or(
                                    new terminate_expr{'w'},
                                    new cat_expr{
                                            new terminate_expr{'z'},
                                            new terminate_expr{'t'}
                                    }
                            )
                    },
                    new terminate_expr{'k'}
            );
    auto fa = reg_expr::get_nfa(r);
    std::cout << fa.to_string() << std::endl;
    delete r;

    std::string str;
    std::cin >> str;
    for (auto c: str) {
        auto res = fa.trans_on(c);
        std::cout << "after [" << c << "], ";
        if (std::get<0>(res)) {
            std::cout << "accept: ";
        } else if (std::get<1>(res)) {
            std::cout << "trap: ";
        } else {
            std::cout << "read: ";
        }
        std::cout << fa.status_code().to_string() << std::endl;
    }
}

template<typename = void>
void test_nfa() {
    // a(xy)*(z|w)*b
    lexer0::nfa nfa0{6};
    nfa0.add_trans(0, 1, 'a');
    nfa0.add_trans(1, 2, 'x');
    nfa0.add_trans(2, 1, 'y');
    nfa0.add_trans(1, 3);
    nfa0.add_trans(3, 4, 'z');
    nfa0.add_trans(3, 4, 'w');
    nfa0.add_trans(4, 3);
    nfa0.add_trans(3, 5, 'b');
    nfa0.add_accept(5);

    std::string str;
    std::cin >> str;
    for (auto c: str) {
        auto res = nfa0.trans_on(c);
        std::cout << "after [" << c << "], ";
        if (std::get<0>(res)) {
            std::cout << "accept: ";
        } else if (std::get<1>(res)) {
            std::cout << "trap: ";
        } else {
            std::cout << "read: ";
        }
        std::cout << nfa0.status_code().to_string() << std::endl;
    }
}
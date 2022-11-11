#include "t_lexer.hpp"

#include <iostream>

using namespace lexer0;

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
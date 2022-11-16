#include <iostream>

#include "ctx_free_syntax.hpp"

using namespace parser0;

void test_first();

int main() {

    test_first();

    return 0;
}

void test_first() {
    using cfs = ctx_free_syntax;

    /*
     * A : BCc | eDB
     * B : * | bCD
     * C : DaB | ca
     * D : * | dD
     * */
    ctx_free_syntax syn {0};
    syn.add_generator(0, {cfs::non_terminate_word(1), cfs::non_terminate_word(2), cfs::token_word(12)});
    syn.add_generator(0, {cfs::token_word(14), cfs::non_terminate_word(3), cfs::non_terminate_word(1)});
    syn.add_generator(1, {cfs::empty_word()});
    syn.add_generator(1, {cfs::token_word(11), cfs::non_terminate_word(2), cfs::non_terminate_word(3)});
    syn.add_generator(2, {cfs::non_terminate_word(3), cfs::token_word(10), cfs::non_terminate_word(1)});
    syn.add_generator(2, {cfs::token_word(12), cfs::token_word(10)});
    syn.add_generator(3, {cfs::empty_word()});
    syn.add_generator(3, {cfs::token_word(13), cfs::non_terminate_word(3)});

    syn.fill();

    std::cout << syn.first_to_string() << std::endl;
    std::cout << syn.follow_to_string() << std::endl;
}
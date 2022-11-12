#include <cassert>

#include "reg_expr.hpp"

std::size_t lexer0::terminate_expr::get_size(bool force) {
    return 2;
}

lexer0::terminate_expr::terminate_expr(int tc) : reg_expr{}, terminate_char{tc} {}

std::size_t lexer0::repeat_expr::get_size(bool force) {
    assert(rep_item != nullptr);
    if (force || fa_size == 0) {
        return fa_size = rep_item->get_size(force) + 2;
    } else {
        return fa_size;
    }
}

lexer0::repeat_expr::~repeat_expr() {
    delete rep_item;
}

lexer0::repeat_expr::repeat_expr(lexer0::reg_expr *item) : rep_item{item} {}

lexer0::cat_expr::~cat_expr() {
    delete cat_item1;
    delete cat_item2;
}

std::size_t lexer0::cat_expr::get_size(bool force) {
    assert(cat_item1 != nullptr && cat_item2 != nullptr);
    if (force || fa_size == 0) {
        return fa_size = cat_item1->get_size(force) + cat_item2->get_size(force) + 1;
    } else {
        return fa_size;
    }
}

lexer0::cat_expr::cat_expr(lexer0::reg_expr *i1, lexer0::reg_expr *i2) : cat_item1{i1}, cat_item2{i2} {}

lexer0::or_expr::or_expr(lexer0::reg_expr *i1, lexer0::reg_expr *i2) : or_item1{i1}, or_item2{i2} {}

lexer0::or_expr::~or_expr() {
    delete or_item1;
    delete or_item2;
}

std::size_t lexer0::or_expr::get_size(bool force) {
    assert(or_item1 != nullptr && or_item2 != nullptr);
    if (force || fa_size) {
        return fa_size = or_item1->get_size(force) + or_item2->get_size(force) + 2;
    } else {
        return fa_size;
    }
}

void lexer0::terminate_expr::create_nfa(lexer0::nfa &fa, std::size_t zero_status) const {
    fa.add_trans(zero_status, zero_status + 1, terminate_char);
}

void lexer0::or_expr::create_nfa(lexer0::nfa &fa, std::size_t zero_status) const {
    std::size_t zero_status_r1{zero_status + 1},
            zero_status_r2{zero_status + 1 + or_item1->get_size(false)};
    std::size_t acc_status_r1{zero_status_r2 - 1},
            acc_status_r2{zero_status_r2 + or_item2->get_size(false) - 1};
    std::size_t acc_status = acc_status_r2 + 1;
    or_item1->create_nfa(fa, zero_status_r1);
    or_item2->create_nfa(fa, zero_status_r2);
    fa.add_trans(zero_status, zero_status_r1);
    fa.add_trans(zero_status, zero_status_r2);
    fa.add_trans(acc_status_r1, acc_status);
    fa.add_trans(acc_status_r2, acc_status);
}

void lexer0::cat_expr::create_nfa(nfa &fa, std::size_t zero_status) const {
    std::size_t zero_r1{zero_status}, zero_r2{zero_status + cat_item1->get_size(false)};
    std::size_t acc_r1{zero_r2 - 1}, acc_r2{zero_r2 + cat_item2->get_size(false) - 1};
    std::size_t acc = acc_r2 + 1;
    cat_item1->create_nfa(fa, zero_r1);
    cat_item2->create_nfa(fa, zero_r2);
    fa.add_trans(acc_r1, zero_r2);
    fa.add_trans(acc_r2, acc);
}

void lexer0::repeat_expr::create_nfa(lexer0::nfa &fa, std::size_t zero_status) const {
    std::size_t zero_r{zero_status + 1}, acc_r{zero_r + rep_item->get_size(false) - 1}, acc{acc_r + 1};
    rep_item->create_nfa(fa, zero_r);
    fa.add_trans(zero_status, zero_r);
    fa.add_trans(zero_r, acc);
    fa.add_trans(acc_r, acc);
    fa.add_trans(acc, zero_r);
}

lexer0::nfa lexer0::reg_expr::get_nfa(lexer0::reg_expr* r) {
    auto size = r->get_size(true);
    nfa fa{size};
    r->create_nfa(fa, 0);
    fa.add_accept(size - 1);
    return fa;
}

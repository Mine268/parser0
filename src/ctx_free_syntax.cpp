#include <queue>

#include "ctx_free_syntax.hpp"

namespace parser0 {

    ctx_free_syntax::syntax_word ctx_free_syntax::token_word(std::size_t t_id) {
        return std::make_tuple(true, false, t_id);
    }

    ctx_free_syntax::syntax_word ctx_free_syntax::non_terminate_word(std::size_t w_id) {
        return std::make_tuple(false, false, w_id);
    }

    ctx_free_syntax::syntax_word ctx_free_syntax::empty_word() {
        return std::make_tuple(true, true, 0);
    }

    ctx_free_syntax::syntax_word ctx_free_syntax::end_word() {
        return std::make_tuple(false, true, 0);
    }

    ctx_free_syntax::ctx_free_syntax(std::size_t start_word_ix) : start_word_ix{start_word_ix} {}

    ctx_free_syntax::ctx_free_syntax(
            std::size_t start_word_ix,
            std::initializer_list<std::pair<std::size_t, std::vector<syntax_word>>> ini_list) : start_word_ix{
            start_word_ix} {
        for (auto &gen_expr: ini_list) {
            gen_list.emplace_back(gen_expr.first, gen_expr.second);
            registered_non_terminate.insert(gen_expr.first);
            for (auto &w: gen_expr.second) {
                if (is_terminate(w)) {
                    registered_token.insert(std::get<2>(w));
                } else {
                    registered_non_terminate.insert(std::get<2>(w));
                }
            }
        }

        fill_first();
        fill_follow();
    }

    void ctx_free_syntax::add_generator(std::size_t lhs, std::vector<syntax_word> expr) {
        gen_list.emplace_back(lhs, expr);
        registered_non_terminate.insert(lhs);
        for (auto &w: expr) {
            if (is_terminate(w)) {
                registered_token.insert(std::get<2>(w));
            } else {
                registered_non_terminate.insert(std::get<2>(w));
            }
        }
    }

    void ctx_free_syntax::fill() {
        fill_first();
        fill_follow();
    }

    void ctx_free_syntax::fill_first() {
        // 初始化first的(key, value)对，简化数据添加
        for (auto non_terminate_ix: registered_non_terminate) {
            first.insert(std::make_pair(
                    non_terminate_ix,
                    std::set<syntax_word>{}
            ));
        }

        bool first_set_update = true;
        while (first_set_update) {
            decltype(first) first_new{};

            // 从原来的first集合构造新的集合
            for (auto &gen_expr: gen_list) {
                // 从产生式的开始遍历到结尾或者遇到某个终结字符
                std::size_t expr_ix = 0;
                // 指示当前遍历的前expr_ix个非终结字符first中是否均包含空串
                bool contain_empty = true;
                // 当前产生式左侧的非终结字符在first_new中的迭代器
                if (first_new.find(gen_expr.first) == first_new.end()) {
                    first_new.insert(std::make_pair(
                            gen_expr.first,
                            std::set<syntax_word>{}
                    ));
                }
                auto first_new_entry_on_current = first_new.find(gen_expr.first);
                while (expr_ix < gen_expr.second.size()) {
                    // 如果是终结字符，那么添加该字符到该非终结字符的first中并退出遍历
                    if (is_terminate(gen_expr.second.at(expr_ix))) {
                        first_new_entry_on_current->second.insert(gen_expr.second.at(expr_ix));
                        contain_empty = false;
                        break;
                    } else {
                        // 指示当前的非终结符first中是否包含空串
                        bool curr_contain_empty = false;
                        for (auto first_word: first.find(std::get<2>(gen_expr.second.at(expr_ix)))->second) {
                            if (is_empty_word((first_word))) {
                                curr_contain_empty = true;
                            } else {
                                first_new_entry_on_current->second.insert(first_word);
                            }
                        }
                        contain_empty &= curr_contain_empty;
                        if (!curr_contain_empty) {
                            break;
                        }
                    }
                    ++expr_ix;
                }
                if (expr_ix == gen_expr.second.size() && contain_empty) {
                    first_new_entry_on_current->second.insert(std::make_tuple(true, true, 0));
                }
            }

            first_set_update = !(first == first_new);
            first = std::move(first_new);
        }
    }

    void ctx_free_syntax::fill_follow() {
        // 将「图」中的非终结字符展开掉的过程中使用的数据结构
        std::map<std::size_t, bool> map_flag;
        // 初始化follow的(key, value)对，简化数据添加
        for (auto non_terminate_ix: registered_non_terminate) {
            follow.insert(std::make_pair(
                    non_terminate_ix,
                    std::set<syntax_word>{}
            ));
            map_flag.insert(std::make_pair(
                    non_terminate_ix,
                    false));
        }
        follow.find(start_word_ix)->second.insert(end_word());

        // 用于将某一段产生式的FIRST集合插入到set中
        // 返回值指示了该产生式是否包含空串
        auto insert_first_to = [&](std::vector<syntax_word>::iterator curr, std::vector<syntax_word>::iterator end,
                                   std::set<syntax_word> &set) -> bool {
            bool continue_add = true;
            while (continue_add && ++curr != end) {
                bool contain_empty = false;
                if (is_non_terminate(*curr)) {
                    const auto &first_set = first.find(std::get<2>(*curr))->second;
                    for (auto &word: first_set) {
                        if (is_empty_word(word)) {
                            contain_empty = true;
                        } else {
                            set.insert(word);
                        }
                    }
                } else {
                    set.insert(*curr);
                }
                continue_add &= contain_empty;
            }
            return continue_add;
        };
        // 在follow集合中建图
        for (auto &gen_expr: gen_list) {
            for (auto word_it = gen_expr.second.begin(); word_it != gen_expr.second.end(); ++word_it) {
                if (is_non_terminate(*word_it)) {
                    auto &curr_set = follow.find(std::get<2>(*word_it))->second;
                    auto contain_empty = insert_first_to(word_it, gen_expr.second.end(),
                            curr_set);
                    if (contain_empty) {
                        curr_set.insert(non_terminate_word(gen_expr.first));
                    }
                }
            }
        }
        for (auto &[ix, set] : follow) {
            set = expand_on(ix);
        }
    }

    std::set<ctx_free_syntax::syntax_word> ctx_free_syntax::expand_on(std::size_t non_ix) {
        std::set<syntax_word> new_follow {};
        std::queue<syntax_word> que;
        std::map<std::size_t, bool> map_flag;
        for (auto &word_ix : registered_non_terminate) {
            map_flag.insert(std::make_pair(word_ix, word_ix == non_ix));
        }
        for (auto & word : follow.find(non_ix)->second) {
            que.push(word);
        }
        while (!que.empty()) {
            auto top = que.front();
            que.pop();
            if (is_terminate(top)) {
                new_follow.insert(top);
            } else if (!map_flag.find(std::get<2>(top))->second) {
                map_flag.find(std::get<2>(top))->second = true;
                for (auto &word: follow.find(std::get<2>(top))->second) {
                    que.push(word);
                }
            }
        }
        return new_follow;
    }

    bool ctx_free_syntax::is_terminate(const ctx_free_syntax::syntax_word &word) {
        return std::get<0>(word) || (!std::get<0>(word) && std::get<1>(word));
    }

    bool ctx_free_syntax::is_empty_word(const ctx_free_syntax::syntax_word &word) {
        return std::get<0>(word) && std::get<1>(word);
    }

    bool ctx_free_syntax::is_end_word(const ctx_free_syntax::syntax_word &word) {
        return !std::get<0>(word) && std::get<1>(word);
    }

    bool ctx_free_syntax::is_non_terminate(const ctx_free_syntax::syntax_word &word) {
        return !std::get<0>(word) && !std::get<1>(word);
    }

    std::string ctx_free_syntax::syntax_to_string(const syntax_word &sw) {
        if (is_empty_word(sw)) {
            return std::string{"empty"};
        } else if (is_end_word(sw)) {
            return std::string{"#"};
        } else if (is_terminate(sw)) {
            return '"' + std::to_string(std::get<2>(sw)) + '"';
        } else {
            return '<' + std::to_string(std::get<2>(sw)) + '>';
        }
    }

    std::string ctx_free_syntax::first_to_string() const {
        std::string ret;
        for (auto &[a, bs]: first) {
            ret += '<' + std::to_string(a) + "> :";
            for (auto &w: bs) {
                ret += ' ' + syntax_to_string(w);
            }
            ret += '\n';
        }
        return ret;
    }

    std::string ctx_free_syntax::follow_to_string() const {
        std::string ret;
        for (auto &[a, bs]: follow) {
            ret += '<' + std::to_string(a) + "> :";
            for (auto &w: bs) {
                ret += ' ' + syntax_to_string(w);
            }
            ret += '\n';
        }
        return ret;
    }

}

#pragma once

#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <string>
#include <initializer_list>

namespace parser0 {
    class ctx_free_syntax {
        /*
         * true, true, * 表示空字符
         * true, false, c 表示终结字符，其中c表示终结字符对应的token编号
         * false, true, * 表示终结字符中的末尾字符（EOF、#）
         * false, false, c 表示非终结字符，其中c唯一标识这个非终结字符
         * */
        using syntax_word = std::tuple<bool, bool, std::size_t>;

        const syntax_word terminate_eof = std::make_tuple(true, true, 0);
        std::size_t start_word_ix;
        std::vector<std::pair<std::size_t, std::vector<syntax_word>>> gen_list;
        std::set<std::size_t> registered_token, registered_non_terminate;
        // 所有字符的first集合
        std::map<std::size_t, std::set<syntax_word>> first;
        // 所有字符的follow集合
        std::map<std::size_t, std::set<syntax_word>> follow;

        // 计算first集合
        void fill_first();
        // 计算follow集合
        void fill_follow();
        // 计算follow的辅助函数
        std::set<syntax_word> expand_on(std::size_t non_ix);
        // 查询某一字符是否是终结字符
        static bool is_terminate(const syntax_word& word) ;
        // 查询某一字符是否是空字符
        static bool is_empty_word(const syntax_word& word);
        // 查询某一字符是否是结尾字符
        static bool is_end_word(const syntax_word& word);
        // 查询某一字符是否是非终结字符
        static bool is_non_terminate(const syntax_word& word);

    public:
        static syntax_word token_word(std::size_t t_id);

        static syntax_word non_terminate_word(std::size_t w_id);

        static syntax_word empty_word();

        static syntax_word end_word();

        // 提供初始字符
        explicit ctx_free_syntax(std::size_t start_word_ix);

        // 构造的时候动态输入所有的产生式
        ctx_free_syntax(
                std::size_t start_word_ix,
                std::initializer_list<std::pair<std::size_t, std::vector<syntax_word>>>);

        // 添加产生式
        void add_generator(std::size_t lhs, std::vector<syntax_word>);

        // 添加完了产生式之后记得调用一下这个
        void fill();

        // 将文法字符转换为字符串
        [[nodiscard]] static std::string syntax_to_string(const syntax_word&);

        // 打印first集合
        [[nodiscard]] std::string first_to_string() const;

        // 答应follow集合
        [[nodiscard]] std::string follow_to_string() const;
    };
}

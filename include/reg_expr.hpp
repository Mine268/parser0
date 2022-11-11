#pragma once

#include <type_traits>
#include <cassert>

#include "nfa.hpp"

namespace lexer0 {

    class reg_expr {
    protected:
        std::size_t fa_size{0};
    public:
        /**
         * Create the NFA recognized the regular expression
         * @param fa The NFA to be created
         * @param zero_status The initial status code current NFA
         */
        virtual void create_nfa(nfa &fa, std::size_t zero_status) const = 0;

        virtual ~reg_expr() = default;

        /**
         * Return the size of the FA for this regular expression
         * @param force Force-ly update the size
         * @return
         */
        [[nodiscard]] virtual std::size_t get_size(bool force) = 0;

        /**
         * Get NFA for the regex
         * @param r Regex
         * @return NFA
         */
        static nfa get_nfa(reg_expr *r);
    };


    class terminate_expr : public virtual reg_expr {
    public:
        int terminate_char{0};

        explicit terminate_expr(int tc);

        [[nodiscard]] std::size_t get_size(bool force) override;

    protected:
        void create_nfa(nfa &fa, std::size_t zero_status) const override;
    };


    class repeat_expr : public virtual reg_expr {
    public:
        explicit repeat_expr(reg_expr *item);

        ~repeat_expr() override;

        reg_expr *rep_item{nullptr};

        [[nodiscard]] std::size_t get_size(bool force) override;

    protected:
        void create_nfa(nfa &fa, std::size_t zero_status) const override;
    };


    class cat_expr : public virtual reg_expr {
    public:
        cat_expr(reg_expr *i1, reg_expr *i2);

        ~cat_expr() override;

        reg_expr *cat_item1{nullptr}, *cat_item2{nullptr};

        [[nodiscard]] std::size_t get_size(bool force) override;

        template<typename... T>
        static reg_expr *get_cat(reg_expr *ft, T...);

    protected:
        void create_nfa(nfa &fa, std::size_t zero_status) const override;
    };

    template<typename... T>
    lexer0::reg_expr *lexer0::cat_expr::get_cat(reg_expr *ft, T... ts) {
        if constexpr (sizeof...(ts) == 0) {
            return ft;
        } else {
            return new cat_expr{ft, cat_expr::get_cat(ts...)};
        }
    }


//    template<typename... Ts>
//    class cat_expr_compact : public virtual reg_expr {
//        static_assert(sizeof...(Ts) >= 1, "You should provide more than 1 regex");
//    public:
//        explicit cat_expr_compact(Ts... ts);
//
//        ~cat_expr_compact() override = default;
//
//        [[nodiscard]] std::size_t get_size(bool force) override;
//
//    protected:
//        void create_nfa(nfa &fa, std::size_t zero_status) const override;
//
//    private:
//        std::array<reg_expr *, sizeof...(Ts)> cats;
//
//        template<std::size_t IX>
//        void initial_cats(reg_expr *fs, Ts... ts);
//    };
//
//    template<typename... Ts>
//    template<std::size_t IX>
//    void cat_expr_compact<Ts...>::initial_cats(reg_expr *fs, Ts... ts) {
//        cats.at(IX) = fs;
//        if constexpr (sizeof...(Ts)) {
//            this->template initial_cats<IX + 1>(ts...);
//        }
//    }
//
//    template<typename... Ts>
//    cat_expr_compact<Ts...>::cat_expr_compact(Ts... ts) : cats{} {
//        initial_cats<0>(ts...);
//    }
//
//
//    template<typename... Ts>
//    std::size_t cat_expr_compact<Ts...>::get_size(bool force) {
//        if (force || fa_size == 0) {
//            std::size_t ret = 0;
//            for (auto reg_ptr : cats) {
//                assert(reg_ptr != nullptr);
//                ret += reg_ptr->get_size(force);
//            }
//            return fa_size = ret + 1;
//        } else {
//            return fa_size;
//        }
//    }
//
//    template<typename... Ts>
//    void cat_expr_compact<Ts...>::create_nfa(nfa &fa, std::size_t zero_status) const {
//        ;
//    }


    class or_expr : public virtual reg_expr {
    public:
        or_expr(reg_expr *i1, reg_expr *i2);

        ~or_expr() override;

        reg_expr *or_item1{nullptr}, *or_item2{nullptr};

        [[nodiscard]] std::size_t get_size(bool force) override;

        template<typename... T>
        static reg_expr *get_or(reg_expr *ft, T...);

    protected:
        void create_nfa(nfa &fa, std::size_t zero_status) const override;
    };

    template<typename... T>
    lexer0::reg_expr *lexer0::or_expr::get_or(lexer0::reg_expr *ft, T... ts) {
        if constexpr (sizeof...(ts) == 0) {
            return ft;
        } else {
            return new or_expr{ft, or_expr::get_or(ts...)};
        }
    }

}

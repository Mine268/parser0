#pragma once

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <numeric>

#include "bit_flagger.hpp"
#include "dfa.hpp"

namespace lexer0 {

    class nfa {
    private:
        size_type size;
        std::vector<status_type> curr_status;
        bool is_trapped;

        // true: empty
        std::vector<std::multimap<std::tuple<bool, input_type>, status_type>> trans;
        std::vector<std::vector<status_type>> inv_trans;
        std::vector<bool> accept_status;
        std::vector<bool> trap_status;

        // input code seen
        std::set<input_type> registered_input;

        void untrap(status_type acc_status);
        void trans_empty();

        void trans_empty_on(std::vector<status_type> &status);
        [[nodiscard]] std::vector<status_type> trans_on_on(const std::vector<status_type> &status, input_type v);

    public:
        /**
         * Create a NFA with designated size
         */
        explicit nfa(size_type);

        /**
         * Create a transition from status "from" to status "to"
         * @param from status "from"
         * @param to status "to"
         * @param v the transition input, "bool" is true iff. the inputs is empty
         * string, "input_type" is the input code
         */
        void add_trans(status_type from, status_type to, std::tuple<bool, input_type> v);
        /**
         * Create a transition from status "from" to status "to" on input "v"
         * @param from status "from"
         * @param to status "to"
         * @param v input
         */
        void add_trans(status_type from, status_type to, input_type v);
        /**
         * Create a emtpy-string transition from status "from" to status "to"
         * @param from status "from"
         * @param to status "to"
         */
        void add_trans(status_type from, status_type to);
        /**
         * Add a accept status to NFA
         * @param acc_status Accept status
         */
        void add_accept(status_type acc_status);
        /**
         * Transit on the input "v"
         * @param v Input code
         * @return The status Acc/Read/Trap
         */
        std::tuple<bool, bool> trans_on(input_type v);
        /**
         * Reset the NFA
         */
        void reset();
        /**
         * Get the current status code of the NFA
         * @return status code
         */
        [[nodiscard]] bit_flagger status_code() const;
        /**
         * String description of the NFA
         * @return description in string
         */
        [[nodiscard]] std::string to_string() const;
        /**
         * Get the determined version of this NFA
         * @return determined version of this NFA
         */
        [[nodiscard]] dfa get_dfa();
    };

}
